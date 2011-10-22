precision highp float;

uniform sampler2D u_diffuseTex;
uniform float u_tileWidth;
uniform bool u_tileBlend;

varying float v_tileIndex;
varying vec4 v_color;
varying vec2 v_texcoord;

void main()
{
    float tIndex = floor(v_tileIndex);
    
    float tMix = 0.0;
    
    if (u_tileBlend)
    {
        tMix = v_tileIndex - tIndex;
    }
    
    float uOffset = u_tileWidth * tIndex;
    
    vec4 colorA = texture2D(u_diffuseTex, vec2(v_texcoord.x * u_tileWidth + uOffset, v_texcoord.y));
    vec4 colorB = texture2D(u_diffuseTex, vec2((v_texcoord.x + 1.0) * u_tileWidth + uOffset, v_texcoord.y));

    gl_FragColor = mix(colorA, colorB, tMix) * v_color;
}
