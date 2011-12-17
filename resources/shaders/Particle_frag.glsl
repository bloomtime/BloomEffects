precision highp float;

uniform sampler2D u_diffuseTex;
uniform sampler2D u_secondaryTex;
uniform float u_tileWidth;
uniform bool u_hasNormal;
uniform bool u_hasDistort;
uniform bool u_tileBlend;
uniform float u_worldTime;

varying float v_tileMix;
varying vec4 v_color;
varying vec3 v_keyLightDir;
varying vec2 v_texcoordA;
varying vec2 v_texcoordB;
varying mat3 v_rotation;

void main()
{
    vec4 colorA = texture2D(u_diffuseTex, v_texcoordA);
    vec4 colorB = colorA;
    
    if (u_tileBlend)
    {
        colorB = texture2D(u_diffuseTex, v_texcoordB);
    }
    
    vec4 diffuseColor = mix(colorA, colorB, v_tileMix) * v_color;
    float lightAmt = 1.0;
    
    if (u_hasNormal)
    {
        vec4 normalA = texture2D(u_secondaryTex, v_texcoordA);
        vec4 normalB = normalA;
        
        if (u_tileBlend)
        {
            normalB = texture2D(u_secondaryTex, v_texcoordB);
        }
        
         vec3 normalColor = mix(normalA, normalA, v_tileMix).xyz;
         normalColor = normalColor * 2.0 - 1.0;
         
         //normalColor = normalize(v_rotation * normalize(normalColor));
         
         lightAmt = max(dot(normalColor, v_keyLightDir), 0.0);
    }
    
    gl_FragColor = vec4(diffuseColor.xyz * lightAmt, diffuseColor.w);
}
