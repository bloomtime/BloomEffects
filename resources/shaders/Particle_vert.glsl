precision highp float;

uniform mat4 u_mvp_matrix;
uniform float u_tileWidth;
uniform bool u_tileBlend;
uniform vec4 u_tintColor;
uniform vec3 u_keyLightDir;

attribute vec2 a_texcoord;
attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec3 a_bitangent;
attribute vec4 a_color;
attribute float a_tileIndex;

varying vec2 v_texcoordA;
varying vec2 v_texcoordB;
varying vec4 v_color;
varying vec3 v_keyLightDir;
varying float v_tileMix;
varying mat3 v_rotation;

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
    
    mat3 inverseRot = mat3(a_tangent, a_bitangent, a_normal);
    
    v_keyLightDir.x = dot(u_keyLightDir, a_tangent);
    v_keyLightDir.y = dot(u_keyLightDir, a_bitangent);
    v_keyLightDir.z = dot(u_keyLightDir, a_normal);
    
    //v_keyLightDir = normalize(inverseRot * normalize(u_keyLightDir));
    //v_keyLightDir = u_keyLightDir;
    
    v_rotation = inverseRot; 
    
    float tileOffset = u_tileWidth * tIndex;
    
	v_texcoordA = vec2(a_texcoord.x * u_tileWidth + tileOffset, a_texcoord.y);
    v_texcoordB = vec2((a_texcoord.x + 1.0) * u_tileWidth + tileOffset, a_texcoord.y);
}
