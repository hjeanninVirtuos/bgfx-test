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
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static bool s_showStats = false;

static void glfw_errorCallback(int error, const char *description)
{
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
		s_showStats = !s_showStats;
}

int main(int argc, char **argv)
{
// GLFW
	glfwSetErrorCallback(glfw_errorCallback);
	if (!glfwInit())
		return 1;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window = glfwCreateWindow(1024, 768, "BGFX test", nullptr, nullptr);
	if (!window)
		return 1;
	glfwSetKeyCallback(window, glfw_keyCallback);

// Init
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

// Matrix
    const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
    const bx::Vec3 eye = {0.0f, 0.0f, -35.0f};
    {
        float view[16];
        bx::mtxLookAt(view, eye, at);
        float proj[16];
        bx::mtxProj(proj, 60.0f, float(init.resolution.width) / float(init.resolution.height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);
        bgfx::setViewRect(0, 0, 0, uint16_t(init.resolution.width), uint16_t(init.resolution.height));
    }

// Buffer
    // Create vertex stream declaration.
    PosColorVertex::init();
    bgfx::VertexBufferHandle m_vbh;
    m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::ms_layout);

// Shader
    bgfx::ProgramHandle m_program;
	bgfx::ShaderHandle dmapack_vs = bgfx::createEmbeddedShader(&dg_dmapack_vs_embed, bgfx::RendererType::Direct3D11, "dg_dmapack_vs");
	bgfx::ShaderHandle dmapack_fs = bgfx::createEmbeddedShader(&dg_dmapack_fs_embed, bgfx::RendererType::Direct3D11, "dg_dmapack_fs");
    m_program = bgfx::createProgram(dmapack_vs, dmapack_fs, true);

    // Create program from shaders.
    //m_program = loadProgram("vs_cubes", "fs_cubes");  
    uint64_t state = 0
    	| BGFX_STATE_WRITE_R
    	| BGFX_STATE_WRITE_G
    	| BGFX_STATE_WRITE_B
    	| BGFX_STATE_WRITE_A
    	| BGFX_STATE_WRITE_Z
    	| BGFX_STATE_DEPTH_TEST_LESS
    	| BGFX_STATE_CULL_CW
    	| BGFX_STATE_MSAA
    	| UINT64_C(0) // Triangle list is 0
    	;

    const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

// LOOP
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
        // Stats
		bgfx::touch(kClearView);
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
		const bgfx::Stats* stats = bgfx::getStats();
		bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);

// Render
        float mtx[16];
        bgfx::setTransform(mtx);
        bgfx::setVertexBuffer(0, m_vbh);
        bgfx::setState(state);
        bgfx::submit(0, m_program);

        // Advance to next frame. Process submitted rendering primitives.
		bgfx::frame();
	}
    bgfx::destroy(m_vbh);
    bgfx::destroy(m_program);
    bgfx::shutdown();
    glfwTerminate();
    return 0;
}