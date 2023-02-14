/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */
#include <stdio.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <bx/math.h>
#include <bgfx/embedded_shader.h>
#include <dg_dmapack_vs.sc.glsl.bin.h>
#include <dg_dmapack_vs.sc.essl.bin.h>
#include <dg_dmapack_vs.sc.spv.bin.h>
#include <dg_dmapack_fs.sc.glsl.bin.h>
#include <dg_dmapack_fs.sc.essl.bin.h>
#include <dg_dmapack_fs.sc.spv.bin.h>
#ifdef _WIN32
#include <dg_dmapack_vs.sc.dx9.bin.h>
#include <dg_dmapack_vs.sc.dx11.bin.h>
#include <dg_dmapack_fs.sc.dx9.bin.h>
#include <dg_dmapack_fs.sc.dx11.bin.h>
#endif // _WIN32

const bgfx::EmbeddedShader dg_dmapack_vs_embed = BGFX_EMBEDDED_SHADER(dg_dmapack_vs);
const bgfx::EmbeddedShader dg_dmapack_fs_embed = BGFX_EMBEDDED_SHADER(dg_dmapack_fs);

struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;

	static void init()
	{
		ms_layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
{
	{-0.5f, -0.5f,  0.5f, 0xff0000ff },
	{ 0.5f,  0.5f,  0.5f, 0xff00ff00 },
	{-0.5f,  0.5f,  0.5f, 0xffff0000 },
};

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argc, char **argv)
{
	// Init
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window = glfwCreateWindow(1024, 768, "BGFX test", nullptr, nullptr);
	if (!window)
		return 1;
	glfwSetKeyCallback(window, glfw_keyCallback);
	bgfx::renderFrame();
	bgfx::Init init;
	init.platformData.nwh = glfwGetWin32Window(window);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init))
		return 1;

	// Buffer
    PosColorVertex::init();
    bgfx::VertexBufferHandle m_vbh;
    m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::ms_layout);

    // Shader
    bgfx::ProgramHandle m_program;
	bgfx::ShaderHandle dmapack_vs = bgfx::createEmbeddedShader(&dg_dmapack_vs_embed, bgfx::RendererType::Direct3D11, "dg_dmapack_vs");
	bgfx::ShaderHandle dmapack_fs = bgfx::createEmbeddedShader(&dg_dmapack_fs_embed, bgfx::RendererType::Direct3D11, "dg_dmapack_fs");
    m_program = bgfx::createProgram(dmapack_vs, dmapack_fs, true);

	// Clear
    const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

	// Loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		bgfx::touch(kClearView);

        bgfx::setVertexBuffer(0, m_vbh);
        bgfx::setState(0 | BGFX_STATE_WRITE_R | BGFX_STATE_WRITE_G | BGFX_STATE_WRITE_B | UINT64_C(0));
        bgfx::submit(0, m_program);

        // Advance to next frame. Process submitted rendering primitives.
		bgfx::frame();
	}

	// Clean
    bgfx::destroy(m_vbh);
    bgfx::destroy(m_program);
    bgfx::shutdown();
    glfwTerminate();
    return 0;
}