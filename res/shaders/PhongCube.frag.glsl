uniform samplerCube TextureCube;

uniform lowp vec4 CubemapColor;
uniform lowp float BlendIntensity;

varying mediump vec3 vNormal;
varying mediump vec3 vViewDirection;
varying lowp vec4 vColor;

void main()
{
    precision mediump float;
    
    vec3 R = reflect(normalize(vViewDirection), normalize(vNormal));
    lowp vec4 baseColor = textureCube(TextureCube, R) * CubemapColor;
    lowp vec4 color = vColor + baseColor;

    gl_FragColor.rgba = color * BlendIntensity;
}
