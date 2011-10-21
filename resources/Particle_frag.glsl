precision highp float;

uniform sampler2D u_diffuseTex;

varying vec4 v_color;
varying vec2 v_texcoord;

void main()
{
    vec4 color = texture2D(u_diffuseTex, v_texcoord);
    gl_FragColor = color * v_color;
}
