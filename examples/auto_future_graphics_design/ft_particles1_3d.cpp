#include "ft_particles1_3d.h"
#include <fstream>
#include "SOIL.h"
#include "texture.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <random>

namespace auto_future
{
	enum dimensions { X, Y, Z };
	struct Particle{
		glm::vec3 pos, speed;
		unsigned char r, g, b, age; // Color
		float size, angle, weight, ax;
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

	static GLfloat uvs[] =
	{
		0.f, 0.f, wu, 0.f,
		0.f, hu, wu, hu,
		wu, 0.f, 2 * wu, 0.f,
		wu, hu, 2 * wu, hu,
		2 * wu, 0.f, 3 * wu, 0.f,
		2 * wu, hu, 3 * wu, hu,
		3 * wu, 0.f, 4 * wu, 0.f,
		3 * wu, hu, 4 * wu, hu,
		4 * wu, 0.f, 5 * wu, 0.f,
		4 * wu, hu, 5 * wu, hu,
		5 * wu, 0.f, 6 * wu, 0.f,
		5 * wu, hu, 6 * wu, hu,

		0.f, hu, wu, hu,
		0.f, 2 * hu, wu, 2 * hu,
		wu, hu, 2 * wu, hu,
		wu, 2 * hu, 2 * wu, 2 * hu,
		2 * wu, hu, 3 * wu, hu,
		2 * wu, 2 * hu, 3 * wu, 2 * hu,
		3 * wu, hu, 4 * wu, hu,
		3 * wu, 2 * hu, 4 * wu, 2 * hu,
		4 * wu, hu, 5 * wu, hu,
		4 * wu, 2 * hu, 5 * wu, 2 * hu,
		5 * wu, hu, 6 * wu, hu,
		5 * wu, 2 * hu, 6 * wu, 2 * hu,

		0.f, 2 * hu, wu, 2 * hu,
		0.f, 3 * hu, wu, 3 * hu,
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

		0.f, 3 * hu, wu, 3 * hu,
		0.f, 4 * hu, wu, 4 * hu,
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

		0.f, 4 * hu, wu, 4 * hu,
		0.f, 5 * hu, wu, 5 * hu,
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

	ft_particles1_3d::ft_particles1_3d()
	{
		_pt._pos0_shd = { 0.f, 7.f, -20.f };
		_pt._v0_shd = { 0.f, -7.f, 0.f };
		_pt._a0_shd = { 0.f, 9.81f, 0.f };
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		reg_value_range(&_pt, 3, 0.f, 20.f);
		reg_value_range(&_pt, 4, 0.f, 50.f);
		reg_value_range(&_pt, 5, -40.f, 40.f);
#endif
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		auto& mut = g_material_list.find("particles1");
		_particle_material = mut->second;
		
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
		_particle_material->set_value("uvcol[0]", uvs, sizeof(uvs));

		auto& mtx = g_mtexture_list.find("flame_fire.png");
		_texture = mtx->second;
		

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
		for (int i = 0; i < MaxParticles; i++){
			ParticlesContainer[i].life = -1.0f;
			ParticlesContainer[i].cameradistance = -1.0f;
		}
		lastTime = glfwGetTime();

	}


	ft_particles1_3d::~ft_particles1_3d()
	{
	}

	default_random_engine generator;
	uniform_real_distribution<float> distribution(-5.f, 5.f);
	uniform_real_distribution<float> distribution1(-3.f, 0.f);
	//#define _calcu_color
	unsigned char ptclsize[] =
	{
		0, 1, 2, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 12, 12, 13, 13, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 26, 27, 28, 29,
	};

	void ft_particles1_3d::draw()
	{
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;
		int newparticles = (int)(delta*10000.0);
		if (newparticles > (int)(0.016f*10000.0))
			newparticles = (int)(0.016f*10000.0);
		for (int i = 0; i < newparticles; i++){
			int particleIndex = FindUnusedParticle();
			Particle& p = ParticlesContainer[particleIndex];
			p.age = 0;
			switch (_pt._pa)
			{
			case en_fire:
			{
				p.pos.x = _pt._pos0_shd.x;
				p.pos.y = _pt._pos0_shd.y;
				p.pos.z = _pt._pos0_shd.z;
				p.life = _pt._life;
				//ParticlesContainer[particleIndex].life =rand() % 20 + 1.f;
				//printf("life=%f\n", ParticlesContainer[particleIndex].life);
				p.speed.x = distribution(generator);

				p.speed.y = distribution1(generator);
				p.speed.z = 0;// distribution1(generator);
				p.ax = -p.speed.x / 10000;
				//cout << "(" << p.speed.x << "," << p.speed.y << "," << p.speed.z << ")" << endl;
				//glm::vec3 postg(_pt._pos0_shd.x, _pt._pos0_shd.y-_pt._y1, _pt._pos0_shd.z);
				//p.speed = postg - p.pos;
			}
			break;
			case en_fire_with_smoke:
			{
				p.pos.z = _pt._pos0_shd.z;
				p.life = 2.f;
				//ParticlesContainer[particleIndex].life =rand() % 20 + 1.f;
				//printf("life=%f\n", ParticlesContainer[particleIndex].life);
				p.pos.x = _pt._pos0_shd.x + distribution(generator);

				p.pos.y = _pt._pos0_shd.y + distribution1(generator);

				glm::vec3 postg(_pt._pos0_shd.x, _pt._pos0_shd.y - _pt._y1, _pt._pos0_shd.z);
				p.speed = postg - p.pos;
				//p.pos.x = (rand() % 30);
				//p.pos.y = _pt._pos0_shd.y;
				//p.pos.z = _pt._pos0_shd.z;
				//p.size = rand() % 30;// (rand() % 1000) / 2000.0f + 0.1f;
				//p.speed.x = (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.005) - (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.005);
				//p.speed.y = ((((((5) * rand() % 11) + 5)) * rand() % 11) + 1) * 0.02;
				//p.speed.z = (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.005) - (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.005);
				//cout << "pos:" << p.pos.x << " " <<p.pos.y << endl;
				//cout << "speed:" << p.speed.x << " " << p.speed.y << " " << p.speed.z << endl;


			}

			break;
			case en_fountain:
				p.life = (((rand() % 125 + 1) / 10.0) + 5);
				p.pos.x = (rand() % 30);
				p.pos.y = _pt._pos0_shd.y;
				p.pos.z = _pt._pos0_shd.z;
				p.speed.x = (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.005) - (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.005);
				p.speed.y = ((((((5) * rand() % 11) + 5)) * rand() % 11) + 10) * 0.02;
				p.speed.z = (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.005) - (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.005);

				break;

			default:
			{
				p.life = _pt._life; // This particle will live 5 seconds.
				p.pos = glm::vec3(_pt._pos0_shd.x, _pt._pos0_shd.y, _pt._pos0_shd.z);

				//float spread = 1.5f;
				glm::vec3 maindir = glm::vec3(_pt._v0_shd.x, _pt._v0_shd.y, _pt._v0_shd.z);
				glm::vec3 randomdir = glm::vec3(
					(rand() % 2000 - 1000.0f) / 1000.0f,
					(rand() % 2000 - 1000.0f) / 1000.0f,
					(rand() % 2000 - 1000.0f) / 1000.0f
					);

				p.speed = maindir + randomdir*_pt._spread;
				p.size = rand() % 30;// (rand() % 1000) / 2000.0f + 0.1f;
			}
			break;
			}

			// Very bad way to generate a random color
			//ParticlesContainer[particleIndex].r = rand() % 256;
			//ParticlesContainer[particleIndex].g = rand() % 256;
			//ParticlesContainer[particleIndex].b = rand() % 256;
			//ParticlesContainer[particleIndex].a = (rand() % 256) / 3;
			//printf("{%f,%f,%f} {%f,%f,%f}\n", p.pos.x, p.pos.y, p.pos.z, p.speed.x, p.speed.x, p.speed.x);


		}
		glm::vec3 CameraPosition(0.f, 0.f, 5.f);

		// Simulate all particles
		int ParticlesCount = 0;
		for (int i = 0; i < MaxParticles; i++){

			Particle& p = ParticlesContainer[i]; // shortcut

			if (p.life > 0.0f){

				// Decrease life
				//if (_pt._pa == en_fire)
				//{
				//	p.life -= 0.02;
				//}
				//else
				{
					p.life -= delta;
				}
				p.age++;
				if (p.age >= sizeof(ptclsize))
				{
					p.age = sizeof(ptclsize) - 1;
				}
				p.size = ptclsize[p.age];
				if (i == 100)
				{
					printf("particle[%d].age=%d \n", i, p.age);

				}
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
					case en_fire:
						p.speed += glm::vec3(p.ax, _pt._a0_shd.y, _pt._a0_shd.z) * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;
						break;
					case en_fountain:
					case en_fire_with_smoke:
					case en_gravity:
						p.speed += glm::vec3(_pt._a0_shd.x, _pt._a0_shd.y, _pt._a0_shd.z) * (float)delta * 0.5f;
						p.pos += p.speed * (float)delta;
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

		//SortParticles();
		//printf("particles count:%d\n", ParticlesCount);
		// Use our shader
		_particle_material->use();
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

}