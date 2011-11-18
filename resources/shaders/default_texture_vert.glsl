precision highp float;

uniform mat4 u_mvp_matrix;

attribute vec3 a_position;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;

void main()
{
	gl_Position = u_mvp_matrix * vec4(a_position, 1.0);

    v_texcoord = a_texcoord;
}
