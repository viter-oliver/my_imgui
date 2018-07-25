uniform sampler2D Texture;
uniform samplerCube TextureCube;

uniform lowp vec4 CubemapColor;
uniform lowp float BlendIntensity;

varying mediump vec3 vNormal;
varying mediump vec3 vViewDirection;
varying mediump vec2 vTexCoord;
varying lowp vec3 vAmbDif;
varying lowp vec3 vSpec;

void main()
{
    precision mediump float;

    vec3 R = reflect(normalize(vViewDirection), normalize(vNormal));
    lowp vec3 cubemapColor = textureCube(TextureCube, R).rgb * CubemapColor.rgb;
    lowp vec4 baseColor = texture2D(Texture, vTexCoord).rgba;
    lowp vec3 color = vAmbDif * baseColor.rgb + vSpec + cubemapColor;

    gl_FragColor.rgb = color * BlendIntensity;
    gl_FragColor.a = baseColor.a * BlendIntensity;
}
