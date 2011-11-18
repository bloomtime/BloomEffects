precision highp float;

uniform sampler2D u_tex_sampler;

varying vec2 v_texcoord;

void main()
{
	gl_FragColor = vec4(texture2D(u_tex_sampler, v_texcoord).xyz, 1.0);
}
