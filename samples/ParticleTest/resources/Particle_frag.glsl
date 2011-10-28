precision highp float;

uniform sampler2D u_diffuseTex;
uniform float u_tileWidth;

varying float v_tileMix;
varying vec4 v_color;
varying vec2 v_texcoordA;
varying vec2 v_texcoordB;

void main()
{
    vec4 colorA = texture2D(u_diffuseTex, v_texcoordA);
    vec4 colorB = texture2D(u_diffuseTex, v_texcoordB);

    gl_FragColor = mix(colorA, colorB, v_tileMix) * v_color;
}
