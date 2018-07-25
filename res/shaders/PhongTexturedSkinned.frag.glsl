uniform sampler2D Texture;

uniform lowp vec4 Emissive;
uniform lowp float BlendIntensity;

varying mediump vec2 vTexCoord;
varying lowp vec3 vAmbDif;
varying lowp vec3 vSpec;

void main()
{
    precision mediump float;

    lowp vec4 baseColor = texture2D(Texture, vTexCoord).rgba;
    lowp vec3 color = vAmbDif * baseColor.rgb + vSpec + Emissive.rgb;

    gl_FragColor.rgb = color * BlendIntensity;
    gl_FragColor.a = baseColor.a * BlendIntensity;
}
