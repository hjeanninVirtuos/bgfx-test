// BGFX
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bgfx/embedded_shader.h>

// GLFW
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// Shader
#include <shader_vs.sc.glsl.bin.h>
#include <shader_vs.sc.essl.bin.h>
#include <shader_vs.sc.spv.bin.h>
#include <shader_fs.sc.glsl.bin.h>
#include <shader_fs.sc.essl.bin.h>
#include <shader_fs.sc.spv.bin.h>
#ifdef _WIN32
#include <shader_vs.sc.dx9.bin.h>
#include <shader_vs.sc.dx11.bin.h>
#include <shader_fs.sc.dx9.bin.h>
#include <shader_fs.sc.dx11.bin.h>
#endif // _WIN32

static void glfw_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

const bgfx::EmbeddedShader shader_vs_embed = BGFX_EMBEDDED_SHADER(shader_vs);
const bgfx::EmbeddedShader shader_fs_embed = BGFX_EMBEDDED_SHADER(shader_fs);

static float s_vertices[] =
{
	-0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
};

void mainLoop(GLFWwindow* window)
{
	// Vertex layout
	bgfx::VertexLayout layout;
	layout
		.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.end();

	// Buffer
	bgfx::VertexBufferHandle vbh;
	vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_vertices, sizeof(s_vertices)), layout);

	// Shader
	bgfx::ProgramHandle program;
	bgfx::ShaderHandle shader_vs = bgfx::createEmbeddedShader(&shader_vs_embed, bgfx::RendererType::Direct3D11, "shader_vs");
	bgfx::ShaderHandle shader_fs = bgfx::createEmbeddedShader(&shader_fs_embed, bgfx::RendererType::Direct3D11, "shader_fs");
	program = bgfx::createProgram(shader_vs, shader_fs, true);

	// Clear
	const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

	// Loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		bgfx::touch(kClearView);

		bgfx::setVertexBuffer(0, vbh);
		bgfx::submit(0, program);

		// Advance to next frame. Process submitted rendering primitives.
		bgfx::frame();
	}

	// Clean
	bgfx::destroy(vbh);
	bgfx::destroy(program);
}

int main(int argc, char **argv)
{
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

	if (!bgfx::init(init))
		return 1;

	mainLoop(window);

    bgfx::shutdown();
    glfwTerminate();
    return 0;
}