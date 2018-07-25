uniform sampler2D Texture;
uniform sampler2D ShadowMapTexture0;


uniform mediump float ShadowBias;
uniform lowp float ShadowIntensity;
uniform lowp float BlendIntensity;


varying mediump vec2 vTexCoord;
varying lowp vec3 vAmbDif;
varying lowp vec3 vSpec;
varying mediump vec4 vShadowPosition[1];

void main()
{
    precision mediump float;
    
    lowp vec4 baseColor = texture2D(Texture, vTexCoord).rgba;
    lowp vec3 color = vAmbDif * baseColor.rgb + vSpec;
   
    vec2 coord = vShadowPosition[0].xy / vShadowPosition[0].w;
    float bias = 1.0 / 512.0 * ShadowBias / vShadowPosition[0].w;
    float biasX = 1.0 * bias;
    if(vShadowPosition[0].w > 1.0 && coord.x >= 0.0 && coord.x < 1.0 && coord.y >= 0.0 && coord.y < 1.0)
    {
        lowp float depth = ((vShadowPosition[0].z - 1.0)/ vShadowPosition[0].w);

        color *= depth > texture2D(ShadowMapTexture0, coord.xy).r ? ShadowIntensity : 1.0;
        color *= depth > texture2D(ShadowMapTexture0, coord.xy + vec2(biasX, 0.0)).r ? ShadowIntensity : 1.0;
        color *= depth > texture2D(ShadowMapTexture0, coord.xy - vec2(biasX, 0.0)).r ? ShadowIntensity : 1.0;
        color *= depth > texture2D(ShadowMapTexture0, coord.xy + vec2(0.0, biasX)).r ? ShadowIntensity : 1.0;
        color *= depth > texture2D(ShadowMapTexture0, coord.xy - vec2(0.0, biasX)).r ? ShadowIntensity : 1.0;
    }
   
    gl_FragColor.rgb = color * BlendIntensity;
    gl_FragColor.a = baseColor.a * BlendIntensity;
}
