#include "ft_particles1.h"
#include <fstream>
#include "SOIL.h"
#include "texture.h"
#include <GLFW/glfw3.h>
struct Particle{
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

const int MaxParticles = 100000;
static Particle ParticlesContainer[MaxParticles];
static GLfloat g_particule_position_size_data[MaxParticles * 4];
static GLfloat g_particule_color_data[MaxParticles * 4];

static int LastUsedParticle = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
static int FindUnusedParticle(){

	for (int i = LastUsedParticle; i<MaxParticles; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i<LastUsedParticle; i++){
		if (ParticlesContainer[i].life < 0){
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

static void SortParticles(){
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

static const GLfloat g_vertex_cc_data[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
};
const GLfloat wu = 1.0 / 6;
const GLfloat hu = 1.0 / 5;

static GLfloat uvs[]=
{
	   0.f,    0.f,     wu,   0.f,
	   0.f,    hu,      wu,    hu,    
	    wu,    0.f, 2 * wu,   0.f,
	    wu,     hu, 2 * wu,     hu,     
	2 * wu,    0.f, 3 * wu,   0.f,
	2 * wu,    hu,  3 * wu,    hu, 
	3 * wu,    0.f, 4 * wu,   0.f,
	3 * wu,    hu,  4 * wu,    hu, 
	4 * wu,    0.f, 5 * wu,   0.f,
	4 * wu,    hu,  5 * wu,    hu, 
	5 * wu,    0.f, 6 * wu,   0.f,
	5 * wu,    hu,  6 * wu,    hu, 

	   0.f,     hu,     wu,     hu,
	   0.f, 2 * hu,     wu, 2 * hu,    
	    wu,     hu, 2 * wu,     hu,
	    wu, 2 * hu, 2 * wu, 2 * hu,     
	2 * wu,     hu, 3 * wu,     hu,
	2 * wu, 2 * hu, 3 * wu, 2 * hu, 
	3 * wu,     hu, 4 * wu,     hu,
	3 * wu, 2 * hu, 4 * wu, 2 * hu, 
	4 * wu,     hu, 5 * wu,     hu,
	4 * wu, 2 * hu, 5 * wu, 2 * hu, 
	5 * wu,     hu, 6 * wu,     hu,
	5 * wu, 2 * hu, 6 * wu, 2 * hu, 

	   0.f, 2 * hu,     wu, 2 * hu,
	   0.f, 3 * hu,     wu, 3 * hu,    
	    wu, 2 * hu, 2 * wu, 2 * hu,
	    wu, 3 * hu, 2 * wu, 3 * hu,     
	2 * wu, 2 * hu, 3 * wu, 2 * hu,
	2 * wu, 3 * hu, 3 * wu, 3 * hu, 
	3 * wu, 2 * hu, 4 * wu, 2 * hu,
	3 * wu, 3 * hu, 4 * wu, 3 * hu, 
	4 * wu, 2 * hu, 5 * wu, 2 * hu,
	4 * wu, 3 * hu, 5 * wu, 3 * hu, 
	5 * wu, 2 * hu, 6 * wu, 2 * hu,
	5 * wu, 3 * hu, 6 * wu, 3 * hu, 

	   0.f, 3 * hu,     wu, 3 * hu,
	   0.f, 4 * hu,     wu, 4 * hu,    
	    wu, 3 * hu, 2 * wu, 3 * hu,
	    wu, 4 * hu, 2 * wu, 4 * hu,     
	2 * wu, 3 * hu, 3 * wu, 3 * hu,
	2 * wu, 4 * hu, 3 * wu, 4 * hu, 
	3 * wu, 3 * hu, 4 * wu, 3 * hu,
	3 * wu, 4 * hu, 4 * wu, 4 * hu, 
	4 * wu, 3 * hu, 5 * wu, 3 * hu,
	4 * wu, 4 * hu, 5 * wu, 4 * hu, 
	5 * wu, 3 * hu, 6 * wu, 3 * hu,
	5 * wu, 4 * hu, 6 * wu, 4 * hu, 

	   0.f, 4 * hu,     wu, 4 * hu,
	   0.f, 5 * hu,     wu, 5 * hu,    
	    wu, 4 * hu, 2 * wu, 4 * hu,
	    wu, 5 * hu, 2 * wu, 5 * hu,    
	2 * wu, 4 * hu, 3 * wu, 4 * hu,
	2 * wu, 5 * hu, 3 * wu, 5 * hu, 
	3 * wu, 4 * hu, 4 * wu, 4 * hu,
	3 * wu, 5 * hu, 4 * wu, 5 * hu, 
	4 * wu, 4 * hu, 5 * wu, 4 * hu,
	4 * wu, 5 * hu, 5 * wu, 5 * hu, 
	5 * wu, 4 * hu, 6 * wu, 4 * hu,
	5 * wu, 5 * hu, 6 * wu, 5 * hu, 
};
extern string g_cureent_project_file_path;
static const char* shaders_fold = "shaders\\";

ft_particles1::ft_particles1()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	string str_shader_code_path = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	str_shader_code_path += shaders_fold;
	string particles_vs_path= str_shader_code_path + "particles1.vt.glsl", particles_fs_path = str_shader_code_path + "particles1.fg.glsl";

	string particles_vs_code, particles_fs_code;
	ifstream ifs;
	ifs.open(particles_vs_path);
	if (!ifs.is_open())
	{
		printf("fail to open:%s\n", particles_vs_path.c_str());
		return;
	}
	filebuf* pbuf = ifs.rdbuf();
	size_t sz_code = pbuf->pubseekoff(0, ifs.end, ifs.in);
	pbuf->pubseekpos(0, ifs.in);
	particles_vs_code.reserve(sz_code);
	particles_vs_code.resize(sz_code);
	pbuf->sgetn(&particles_vs_code[0], sz_code);
	ifs.close();
	ifs.open(particles_fs_path);
	pbuf = ifs.rdbuf();
	sz_code = pbuf->pubseekoff(0, ifs.end, ifs.in);
	pbuf->pubseekpos(0, ifs.in);
	particles_fs_code.reserve(sz_code);
	particles_fs_code.resize(sz_code);
	pbuf->sgetn(&particles_fs_code[0], sz_code);
	ifs.close();
	_particle_shader = make_shared<af_shader>(particles_vs_code.c_str(), particles_fs_code.c_str());
	float cmr_wp[] = { 0.999137f, -0.000177f, 0.041540f };
	_particle_shader->uniform("CameraRight_worldspace", cmr_wp);
	//glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
	float cmu_wp[] = { -0.009298f, 0.973666f, 0.227788f };
	_particle_shader->uniform("CameraUp_worldspace", cmu_wp);
	//glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
	float vwpj[] = { 1.809097f, -0.022448f, 0.040568f, 0.040486f,
		-0.000320f, 2.350639f, 0.228434f, 0.227978f,
		0.075215f, 0.549929f, -0.974772f, -0.972824f,
		-0.376075f, -2.749644f, 4.673659f, 4.864121f };
	_particle_shader->uniform("VP", vwpj);
	_particle_shader->uniform("uvcol[0]", uvs);
	string str_texture_file = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	str_texture_file += "explosion_magic.png";
	int iw, ih;
	_txt_unit = TextureHelper::load2DTexture(str_texture_file.c_str(), iw, ih, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA);

	glGenBuffers(1, &_vbo_uv);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_cc_data), g_vertex_cc_data, GL_STATIC_DRAW);
	glGenBuffers(1, &_vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glGenBuffers(1, &_vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_color);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	for (int i = 0; i<MaxParticles; i++){
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
	}
	lastTime = glfwGetTime();
}


ft_particles1::~ft_particles1()
{
}
#include <chrono>
#define _calcu_color
void ft_particles1::draw()
{
	double currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	lastTime = currentTime;
	int newparticles = (int)(delta*10000.0);
	if (newparticles > (int)(0.016f*10000.0))
		newparticles = (int)(0.016f*10000.0);

	for (int i = 0; i<newparticles; i++){
		int particleIndex = FindUnusedParticle();
		ParticlesContainer[particleIndex].life = _pt._life; // This particle will live 5 seconds.
		ParticlesContainer[particleIndex].pos = glm::vec3(_pt._pos0.x, _pt._pos0.y, _pt._pos0.z);

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(_pt._v0.x, _pt._v0.y, _pt._v0.z);
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
			);

		ParticlesContainer[particleIndex].speed = maindir + randomdir*spread;


		// Very bad way to generate a random color
		ParticlesContainer[particleIndex].r = rand() % 256;
		ParticlesContainer[particleIndex].g = rand() % 256;
		ParticlesContainer[particleIndex].b = rand() % 256;
		ParticlesContainer[particleIndex].a = (rand() % 256) / 3;

		ParticlesContainer[particleIndex].size = rand() % 30;// (rand() % 1000) / 2000.0f + 0.1f;

	}

	glm::vec3 CameraPosition(0.f, 0.f, 5.f);

	// Simulate all particles
	int ParticlesCount = 0;
	for (int i = 0; i<MaxParticles; i++){

		Particle& p = ParticlesContainer[i]; // shortcut

		if (p.life > 0.0f){

			// Decrease life
			p.life -= delta;
			if (p.life > 0.0f){

				// Simulate simple physics : gravity only, no collisions
				switch (_pt._pa)
				{
				case en_normal:
				    {
						//auto prev_pos = p.pos;
						//auto gforce = glm::vec3(1,1,0);
						//p.pos = p.pos + p.speed * (float)delta + (float)(0.5*delta*delta / 50.f)*gforce;
						//p.speed = (p.pos - prev_pos) / (float)delta;

						static auto t_start = std::chrono::high_resolution_clock::now();
						auto t_now = std::chrono::high_resolution_clock::now();
						float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
	
						glm::vec3 av(0, -7, 0);
						glm::mat4 rtm(1);
						rtm = glm::rotate(rtm, time* glm::radians(60.0f),
							glm::vec3(0.0f, 0.0f, 1.0f));
						av = glm::vec3(rtm * glm::vec4(av, 1.0));
						p.speed += av * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;

				    }
					break;
				case en_gravity:
					p.speed += glm::vec3(_pt._a0.x, _pt._a0.y, _pt._a0.z) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					break;
				case en_fire:
					break;
				case en_fire_with_smoke:
					break;
				case en_fountain:
					break;
				}

				p.cameradistance = glm::length2(p.pos - CameraPosition);
				//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

				// Fill the GPU buffer
				g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
				g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
				g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

				g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;
#ifdef _calcu_color

				g_particule_color_data[4 * ParticlesCount + 0] = p.r;
				g_particule_color_data[4 * ParticlesCount + 1] = p.g;
				g_particule_color_data[4 * ParticlesCount + 2] = p.b;
				g_particule_color_data[4 * ParticlesCount + 3] = p.a;
#endif

			}
			else{
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}

			ParticlesCount++;

		}
	}

	SortParticles();
	printf("particles count:%d\n", ParticlesCount);
// Use our shader
	_particle_shader->use();
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, ParticlesCount * 4 * sizeof(GLfloat), g_particule_position_size_data, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	//glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

#ifdef _calcu_color
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_color);
	glBufferData(GL_ARRAY_BUFFER, ParticlesCount * 4 * sizeof(GLubyte), g_particule_color_data, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	//glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _txt_unit);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(_txt_unit, 0);

	// Same as the billboards tutorial


	//glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size : x + y + z + size => 4
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1

#ifdef _calcu_color
	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_color);
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		4,                                // size : r + g + b + a => 4
		GL_UNSIGNED_BYTE,                 // type
		GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		0,                                // stride
		(void*)0                          // array buffer offset
		);
	glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
#endif

	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
	// but faster.
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
	//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, ParticlesCount);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
void ft_particles1::draw_peroperty_page()
{
	ImGui::Text("Emitting position:");
	ImGui::SliderFloat("px", &_pt._pos0.x, -100.f, 100.f);
	ImGui::SliderFloat("py", &_pt._pos0.y, -100.f, 100.f);
	ImGui::SliderFloat("pz", &_pt._pos0.z, -100.f, 100.f);
	ImGui::Text("Emitting velocity:");
	ImGui::SliderFloat("vx", &_pt._v0.x, -100.f, 100.f);
	ImGui::SliderFloat("vy", &_pt._v0.y, -100.f, 100.f);
	ImGui::SliderFloat("vz", &_pt._v0.z, -100.f, 100.f);
	ImGui::Text("Accelerated velocity:");
	ImGui::SliderFloat("ax", &_pt._a0.x, -100.f, 100.f);
	ImGui::SliderFloat("ay", &_pt._a0.y, -100.f, 100.f);
	ImGui::SliderFloat("az", &_pt._a0.z, -100.f, 100.f);
	ImGui::Text("Life(seconds):");
	ImGui::SliderFloat("life", &_pt._life, 0.f, 20.f);

}
bool ft_particles1::init_from_json(Value& jvalue)
{
	return true;
}
bool ft_particles1::init_json_unit(Value& junit)
{
	return true;
}
#endif