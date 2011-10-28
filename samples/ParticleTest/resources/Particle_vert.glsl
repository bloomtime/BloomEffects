precision highp float;

uniform mat4 u_mvp_matrix;
uniform float u_tileWidth;
uniform bool u_tileBlend;
uniform vec4 u_tintColor;

attribute vec2 a_texcoord;
attribute vec4 a_position;
attribute vec4 a_color;
attribute float a_tileIndex;

varying vec2 v_texcoordA;
varying vec2 v_texcoordB;
varying vec4 v_color;
varying float v_tileMix;

void main()
{
	gl_Position = u_mvp_matrix * a_position;
    
    v_color = a_color * u_tintColor;
    
    float tIndex = floor(a_tileIndex);
    
    v_tileMix = 0.0;
    
    if (u_tileBlend)
    {
        v_tileMix = a_tileIndex - tIndex;
    }
    
    float tileOffset = u_tileWidth * tIndex;
    
	v_texcoordA = vec2(a_texcoord.x * u_tileWidth + tileOffset, a_texcoord.y);
    v_texcoordB = vec2((a_texcoord.x + 1.0) * u_tileWidth + tileOffset, a_texcoord.y);
}
