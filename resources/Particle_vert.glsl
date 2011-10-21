precision highp float;

uniform mat4 u_mvp_matrix;

attribute vec2 a_texcoord;
attribute vec4 a_position;
attribute vec4 a_color;

varying vec2 v_texcoord;
varying vec4 v_color;

void main()
{
	gl_Position = u_mvp_matrix * a_position;

	v_texcoord = a_texcoord;
    v_color = a_color;
}
