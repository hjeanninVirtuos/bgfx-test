$input a_position, a_color0, a_texcoord0
$output v_color, v_uv

uniform vec4 u_coordinateScale;
uniform vec4 u_baseColor;
uniform vec4 u_projectionMatrix0;
uniform vec4 u_projectionMatrix1;
uniform vec4 u_projectionMatrix3;
uniform vec4 u_textureParam;

void main()
{
    vec2 position = vec2(a_position.xy) * u_coordinateScale.xy;
    vec2 uv = vec2(a_texcoord0.xy) * u_coordinateScale.zw;
    v_uv = uv * u_textureParam.xy + u_textureParam.zw;
    v_color = a_color0.abgr * u_baseColor;
    gl_Position = vec4(position.xxx * u_projectionMatrix0.xyz + position.yyy * u_projectionMatrix1.xyz + u_projectionMatrix3.xyz, 1.f);
}
