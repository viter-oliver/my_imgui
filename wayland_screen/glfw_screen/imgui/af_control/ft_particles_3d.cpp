#include "ft_particles_3d.h"
#include <fstream>
#include "SOIL.h"
#include "texture.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <chrono>

namespace auto_future
{
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

		for (int i = LastUsedParticle; i < MaxParticles; i++){
			if (ParticlesContainer[i].life < 0){
				LastUsedParticle = i;
				return i;
			}
		}

		for (int i = 0; i < LastUsedParticle; i++){
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


	ft_particles_3d::ft_particles_3d()
	{
		_pt._pos0_shd = { 0.f, 7.f, -20.f };
		_pt._v0_shd = { 0.f, -7.f, 0.f };
		_pt._a0_shd = { 0.f, 9.81f, 0.f };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_value_range(&_pt, 3, 0.f, 20.f);
#endif
		#ifdef GLFW_INCLUDE_ES3
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		#endif
		auto mut = g_material_list.find("particles");
		_particle_material = mut->second;

		// Same as the billboards tutorial
		float cmr_wp[] = { 0.999137f, -0.000177f, 0.041540f };
		_particle_material->set_value("CameraRight_worldspace", cmr_wp, 3);

		//glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		float cmu_wp[] = { -0.009298f, 0.973666f, 0.227788f };
		_particle_material->set_value("CameraUp_worldspace", cmu_wp, 3);

		//glUniform3f(CameraUp_worldspace_ID, ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		float vwpj[] = { 1.809097f, -0.022448f, 0.040568f, 0.040486f,
			-0.000320f, 2.350639f, 0.228434f, 0.227978f,
			0.075215f, 0.549929f, -0.974772f, -0.972824f,
			-0.376075f, -2.749644f, 4.673659f, 4.864121f };
		_particle_material->set_value("VP", vwpj, 16);

		auto mtx = g_mtexture_list.find("fire.png");
		_texture = mtx->second;


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
		for (int i = 0; i < MaxParticles; i++){
			ParticlesContainer[i].life = -1.0f;
			ParticlesContainer[i].cameradistance = -1.0f;
		}
		lastTime = glfwGetTime();
	}


	ft_particles_3d::~ft_particles_3d()
	{
	}

#define _calcu_color
	void ft_particles_3d::draw()
	{
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;
		int newparticles = (int)(delta*10000.0);
		if (newparticles > (int)(0.016f*10000.0))
			newparticles = (int)(0.016f*10000.0);

		for (int i = 0; i < newparticles; i++){
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = _pt._life; // This particle will live 5 seconds.
			ParticlesContainer[particleIndex].pos = glm::vec3(_pt._pos0_shd.x, _pt._pos0_shd.y, _pt._pos0_shd.z);

			float spread = 1.5f;
			glm::vec3 maindir = glm::vec3(_pt._v0_shd.x, _pt._v0_shd.y, _pt._v0_shd.z);
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
		for (int i = 0; i < MaxParticles; i++){

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
						p.speed += glm::vec3(_pt._a0_shd.x, _pt._a0_shd.y, _pt._a0_shd.z) * (float)delta * 0.5f;
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
		_particle_material->use();
		#ifdef GLFW_INCLUDE_ES3
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
		glBindTexture(GL_TEXTURE_2D, _texture->_txt_id());
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(_texture->_txt_id(), 0);
		


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
		#endif
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, ParticlesCount);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

#endif
}
