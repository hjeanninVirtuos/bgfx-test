$input v_color, v_uv

//uniform vec4 u_texCheckColor;

#include <bgfx_shader.sh>

SAMPLER2D(u_texture, 0);

void main()
{
    vec4 color = texture2D(u_texture, v_uv);
    color = min(color, vec4(1.0f, 1.0f, 1.0f, 1.0f));
    //color = max(color, u_texCheckColor);
    color = max(color, vec4(0.0f, 0.0f, 0.0f, 0.0f));
    gl_FragColor = color * v_color;
}
