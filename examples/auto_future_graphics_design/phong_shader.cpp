#include "phong_shader.h"
#include "control_common_def.h"
const char* phong_vertex_share = R"glsl(
attribute vec3 kzPosition;
attribute vec3 kzNormal;
uniform highp mat4 kzProjectionCameraWorldMatrix;
uniform highp mat4 kzWorldMatrix;
uniform highp mat4 kzNormalMatrix;
uniform highp vec3 kzCameraPosition;
uniform mediump vec3 PointLightPosition[2];
uniform mediump vec3 PointLightAttenuation[2];
uniform mediump vec3 DirectionalLightDirection[1];

uniform lowp vec4 PointLightColor[2];
uniform lowp vec4 DirectionalLightColor[1];
uniform lowp vec4 Emissive;

uniform lowp vec4 Ambient;
uniform lowp vec4 Diffuse;
uniform lowp vec4 SpecularColor;
uniform mediump float SpecularExponent;
uniform lowp float BlendIntensity;

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

phong_shader::phong_shader()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	//vbo
	glGenBuffers(1, &_vbo);
	//ebo;
	glGenBuffers(1, &_ebo);
	//vertex shader
	_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_vertex_shader, 1, &phong_vertex_share, NULL);
	glCompileShader(_vertex_shader);
	//fragment shader
	_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_fragment_shader, 1, &phong_fragment_shader, NULL);
	glCompileShader(_fragment_shader);
	//link
	_shader_program = glCreateProgram();
	glAttachShader(_shader_program, _vertex_shader);
	glAttachShader(_shader_program, _fragment_shader);
	glLinkProgram(_shader_program);
	//get attrib location
	_kzPosition = glGetAttribLocation(_shader_program, "kzPosition");
	_kzNormal = glGetAttribLocation(_shader_program, "kzNormal");
	//get uniform location
	_ProjectionCameraWorldMatrix = glGetUniformLocation(_shader_program, "ProjectionCameraWorldMatrix");
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), base_ui_component::screenw / base_ui_component::screenh, 1.0f, 10.0f);
	glUniformMatrix4fv(_ProjectionCameraWorldMatrix, 1, GL_FALSE, glm::value_ptr(proj));
	glm::mat4 tidentity;
	_kzWorldMatrix = glGetUniformLocation(_shader_program, "kzWorldMatrix");
	glUniformMatrix4fv(_kzWorldMatrix, 1, GL_FALSE, glm::value_ptr(tidentity));
	_kzNormalMatrix = glGetUniformLocation(_shader_program, "kzNormalMatrix");
	glUniformMatrix4fv(_kzNormalMatrix, 1, GL_FALSE, glm::value_ptr(tidentity));
	glm::mat4 cameraView = glm::lookAt(glm::vec3(-1.5f, -1.0f, 1.5f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
		);
	_kzCameraPosition = glGetUniformLocation(_shader_program, "kzCameraPosition");
	glUniformMatrix4fv(_kzCameraPosition, 1, GL_FALSE, glm::value_ptr(cameraView));
	float default_pos[6] = { 0.f};
	_PointLightPosition2 = glGetUniformLocation(_shader_program, "PointLightPosition");
	glUniform3fv(_PointLightPosition2, 2, default_pos);
	float default_pointlight_attenuation[6] = { 1.f, 0.f, 0.f, 1.f, 0.f, 0.f };
	_PointLightAttenuation2 = glGetUniformLocation(_shader_program, "PointLightAttenuation");
	glUniform3fv(_PointLightAttenuation2, 2, default_pointlight_attenuation);
	_DirectionalLightDirection1 = glGetUniformLocation(_shader_program, "DirectionalLightDirection");
	glUniform3fv(_DirectionalLightDirection1, 1, default_pos);
	_PointLightColor2 = glGetUniformLocation(_shader_program, "PointLightColor");
	float def_cl = 126.0f / 255.f;
	float default_color[8] = { def_cl, def_cl, def_cl, 1, def_cl, def_cl, def_cl, 1 };
	glUniform4fv(_PointLightColor2, 2, default_color);
	_DirectionalLightColor1 = glGetUniformLocation(_shader_program, "DirectionalLightColor");
	glUniform4fv(_DirectionalLightColor1, 1, default_color);
	_Emissive = glGetUniformLocation(_shader_program, "Emissive");
	float defcl_ems[4] = { 0.f, 0.f, 0.f, 1 };
	glUniform4fv(_Emissive, 1, defcl_ems);
	_Ambient = glGetUniformLocation(_shader_program, "Ambient");
	float defcl_amb = 51.f / 255.f;
	float defcolor_amb[4] = { defcl_amb, defcl_amb, defcl_amb, 1 };
	glUniform4fv(_Ambient, 1, defcolor_amb);
	_Diffuse = glGetUniformLocation(_shader_program, "Diffuse");
	float defcl_dif = 204.f / 255.f;
	float defcolor_dif[4] = { defcl_dif, defcl_dif, defcl_dif, 1 };
	glUniform4fv(_Diffuse, 1, defcolor_dif);
	_SpecularColor = glGetUniformLocation(_shader_program, "SpecularColor");
	float defcl_sp = 126.f / 255.f;
	float defcolor_sp[4] = { defcl_sp, defcl_sp, defcl_sp, 1 };
	glUniform4fv(_SpecularColor, 1, defcolor_sp);
	_SpecularExponent = glGetUniformLocation(_shader_program, "SpecularExponent");
	glUniform1f(_SpecularExponent, 50.f);
	_BlendIntensity = glGetUniformLocation(_shader_program, "BlendIntensity");
	glUniform1f(_BlendIntensity, 1.f);
}

phong_shader::~phong_shader()
{
	glDeleteProgram(_shader_program);
	glDeleteShader(_vertex_shader);
	glDeleteShader(_fragment_shader);
	glDeleteBuffers(1,&_ebo);
	glDeleteBuffers(1,&_vbo);
	glDeleteVertexArrays(1, &_vao);
}
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
void phong_shader::render_tri_mesh(tri_mesh& tmesh)
{
	GLint last_program, last_array_buffer, last_element_array_buffer;
	glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(_shader_program);
	glEnableVertexAttribArray(_kzPosition);
	glEnableVertexAttribArray(_kzNormal);
	glVertexAttribPointer(_kzPosition, 3, GL_FLOAT, GL_FALSE, sizeof(base_vertex), (GLvoid*)OFFSETOF(base_vertex, position));
	glVertexAttribPointer(_kzNormal, 3, GL_FLOAT, GL_FALSE, sizeof(base_vertex), (GLvoid*)OFFSETOF(base_vertex, vnormal));
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, tmesh.vertices.size()*sizeof(base_vertex), (const GLvoid*)&tmesh.vertices.at(0), GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tmesh.faces.size()*sizeof(tri_face), (const GLvoid*)&tmesh.faces.at(0), GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, (GLsizei)tmesh.faces.size(), GL_INT, 0);
	
	glDisableVertexAttribArray(_kzPosition);
	glDisableVertexAttribArray(_kzNormal);
	glUseProgram(last_program);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
}

void phong_shader::set_vertex_wrold_matrix(glm::mat4& vtmat)
{
	glUniformMatrix4fv(_kzWorldMatrix, 1, GL_FALSE, glm::value_ptr(vtmat));
	glm::mat4 inversevm = glm::inverse(vtmat);//Äæ¾ØÕó
	glm::mat4 transposvm = glm::transpose(inversevm);
	glUniformMatrix4fv(_kzNormal, 1, GL_FALSE, glm::value_ptr(transposvm));
}
