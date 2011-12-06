precision highp float;

uniform sampler2D u_diffuseTex;
uniform sampler2D u_secondaryTex;
uniform float u_tileWidth;
uniform bool u_hasNormal;
uniform bool u_hasDistort;
uniform bool u_tileBlend;

varying float v_tileMix;
varying vec4 v_color;
varying vec2 v_texcoordA;
varying vec2 v_texcoordB;

void main()
{
    if (u_hasNormal)
    {
        vec4 normalA = texture2D(u_secondaryTex, v_texcoordA);
        vec4 normalB = normalA;
        
        if (u_tileBlend)
        {
            normalB = texture2D(u_secondaryTex, v_texcoordB);
        }
    }
    
    vec4 colorA = texture2D(u_diffuseTex, v_texcoordA);
    vec4 colorB = colorA;
    
    if (u_tileBlend)
    {
        colorB = texture2D(u_diffuseTex, v_texcoordB);
    }
    
    gl_FragColor = mix(colorA, colorB, v_tileMix) * v_color;
}
