$input a_position
$output v_color0

#include <bgfx_shader.sh>

void main()
{
	gl_Position = vec4(a_position, 1.0);
}
