#include "ft_scene3d.h"
#include "common_functions.h"
#ifdef _kanzi_phong_shading
const char* phong_vertex_share = R"glsl(
attribute vec3 kzPosition;
attribute vec3 kzNormal;
uniform highp mat4 kzProjectionCameraWorldMatrix;
uniform highp mat4 kzWorldMatrix;
uniform highp mat4 kzNormalMatrix;
uniform highp vec3 kzCameraPosition;
uniform mediump vec3 PointLightPosition[2];
uniform lowp vec4 PointLightColor[2];
uniform mediump vec3 PointLightAttenuation[2];
uniform lowp vec4 DirectionalLightColor[1];
uniform mediump vec3 DirectionalLightDirection[1];
uniform lowp float BlendIntensity;
uniform lowp vec4 Emissive;

uniform lowp vec4 Ambient;
uniform lowp vec4 Diffuse;
uniform lowp vec4 SpecularColor;
uniform mediump float SpecularExponent;

varying lowp vec4 vColor;

void main()
{
    precision mediump float;
    
    vec3 pointLightDirection[2];
    vec4 positionWorld = kzWorldMatrix * vec4(kzPosition.xyz, 1.0);
    vec3 V = normalize(positionWorld.xyz - kzCameraPosition);
    vec4 Norm = kzNormalMatrix * vec4(kzNormal, 1.0);
    vec3 N = normalize(Norm.xyz);

    pointLightDirection[0] = positionWorld.xyz - PointLightPosition[0];
    pointLightDirection[1] = positionWorld.xyz - PointLightPosition[1];
    
    vec3 L[3];
    vec3 H[3];
    float LdotN, NdotH;
    float specular;
    vec3 c;
    float d, attenuation;
    
    lowp vec3 vAmbDif = vec3(0.0);
    lowp vec3 vSpec = vec3(0.0);
    
    L[0] = vec3(1.0, 0.0, 0.0);
    if(length(DirectionalLightDirection[0]) > 0.01)
    {
        L[0] = normalize(-DirectionalLightDirection[0]);
    }
    H[0] = normalize(-V + L[0]);
    
    L[1] = normalize(-pointLightDirection[0]);
    H[1] = normalize(-V + L[1]);
    
    L[2] = normalize(-pointLightDirection[1]);
    H[2] = normalize(-V + L[2]);

    vAmbDif += Ambient.rgb;
  
    // Apply directional light 0.
    {
        LdotN = max(0.0, dot(L[0], N));
        NdotH = max(0.0, dot(N, H[0]));
        specular = pow(NdotH, SpecularExponent);
        vAmbDif += (LdotN * Diffuse.rgb) * DirectionalLightColor[0].rgb;
        vSpec += SpecularColor.rgb * specular * DirectionalLightColor[0].rgb;
    }
    
    // Apply point light 0.
    {
        LdotN = max(0.0, dot(L[1], N));
        NdotH = max(0.0, dot(N, H[1]));
        specular = pow(NdotH, SpecularExponent);
        c = PointLightAttenuation[0];
        d = length(pointLightDirection[0]);
        attenuation = 1.0 / max(0.001, (c.x + c.y * d + c.z * d * d));
        vAmbDif += (LdotN * Diffuse.rgb) * attenuation * PointLightColor[0].rgb;
        vSpec +=  SpecularColor.rgb * specular * attenuation * PointLightColor[0].rgb;
    }

    // Apply point light 1.
    {
        LdotN = max(0.0, dot(L[2], N));
        NdotH = max(0.0, dot(N, H[2]));
        specular = pow(NdotH, SpecularExponent);
        c = PointLightAttenuation[1];
        d = length(pointLightDirection[1]);
        attenuation = 1.0 / max(0.001, (c.x + c.y * d + c.z * d * d));
        vAmbDif += (LdotN * Diffuse.rgb) * attenuation * PointLightColor[1].rgb;
        vSpec +=  SpecularColor.rgb * specular * attenuation * PointLightColor[1].rgb;
    }

    vColor.rgb = (vAmbDif.rgb + vSpec.rgb + Emissive.rgb) * BlendIntensity;
    vColor.a = BlendIntensity;
    gl_Position = kzProjectionCameraWorldMatrix * vec4(kzPosition.xyz, 1.0);
}
)glsl";
const char* phong_fragment_shader = R"glsl(
varying lowp vec4 vColor;
void main()
{
    precision lowp float;
    gl_FragColor.rgba = vColor;
}
)glsl";
#else
const GLchar* vertexSource = R"glsl(
    #version 150 core
   in vec3 position;
    in vec3 color;
    in vec2 texcoord;
    out vec3 Color;
    out vec2 Texcoord;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    void main()
    {
        Color = color;
        Texcoord = texcoord;
        gl_Position = proj * view * model * vec4(position, 1.0);
    }
)glsl";
const GLchar* fragmentSource = R"glsl(
    #version 150 core
    in vec3 Color;
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D texPuppy;
    void main()
    {
        //outColor = vec4(Color, 1.0) * mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
        outColor = texture(texPuppy, Texcoord);

    }

)glsl";
#endif
ft_scene3d::ft_scene3d(ft_scene3d& bsource)
{
	prepareFBO1(_colorTextId, _depthStencilTextId, _fboId, screenw, screenh);

}


ft_scene3d::~ft_scene3d()
{
}

void ft_scene3d::draw()
{

}
#if !defined(IMGUI_WAYLAND)
void ft_scene3d::draw_peroperty_page()
{

}
#endif

bool ft_scene3d::init_from_json(Value& jvalue)
{
	return true;
}

bool ft_scene3d::handle_mouse()
{
	return true;
}

void ft_scene3d::set_light_type(en_light_type ltype)
{

}

