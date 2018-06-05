#include "ft_particles_gravity.h"
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
Particle ParticlesContainer[MaxParticles];
GLfloat g_particule_position_size_data[MaxParticles * 4];
GLfloat g_particule_color_data[MaxParticles * 4];

int LastUsedParticle = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticle(){

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

void SortParticles(){
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

static const GLfloat g_vertex_buffer_data[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
};
extern string g_cureent_project_file_path;
const char* shaders_fold = "shaders\\";

ft_particles_gravity::ft_particles_gravity()
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	string str_shader_code_path = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	str_shader_code_path += shaders_fold;
	string particles_vs_path= str_shader_code_path + "particles.vs", particles_fs_path = str_shader_code_path + "particles.fs";
	string particles_vs_code, particles_fs_code;
	ifstream ifs;
	ifs.open(particles_vs_path);
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
	string str_texture_file = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	str_texture_file += "blue_ring.png";
	int iw, ih;
	_txt_unit = TextureHelper::load2DTexture(str_texture_file.c_str(), iw, ih, GL_RGBA, GL_RGBA, SOIL_LOAD_RGBA);
	GLfloat g_vertex_buffer_data[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};
	glGenBuffers(1, &_vbo_uv);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
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


ft_particles_gravity::~ft_particles_gravity()
{
}

void ft_particles_gravity::draw()
{
	double currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	lastTime = currentTime;
	int newparticles = (int)(delta*10000.0);
	if (newparticles > (int)(0.016f*10000.0))
		newparticles = (int)(0.016f*10000.0);

	for (int i = 0; i<newparticles; i++){
		int particleIndex = FindUnusedParticle();
		ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
		ParticlesContainer[particleIndex].pos = glm::vec3(0, 0, -20.0f);

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
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

		ParticlesContainer[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;

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
				p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
				p.pos += p.speed * (float)delta;
				p.cameradistance = glm::length2(p.pos - CameraPosition);
				//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

				// Fill the GPU buffer
				g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
				g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
				g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

				g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

				g_particule_color_data[4 * ParticlesCount + 0] = p.r;
				g_particule_color_data[4 * ParticlesCount + 1] = p.g;
				g_particule_color_data[4 * ParticlesCount + 2] = p.b;
				g_particule_color_data[4 * ParticlesCount + 3] = p.a;

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
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo_color);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _txt_unit);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(_txt_unit, 0);

	// Same as the billboards tutorial
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

	
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

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