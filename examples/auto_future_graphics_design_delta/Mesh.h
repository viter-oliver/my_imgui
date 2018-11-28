#pragma once
#if !defined(IMGUI_WAYLAND)
#include <GL/gl3w.h>
#else
#include"../../deps/glad/glad.h"
#endif
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/string_cast.hpp>
#include <string>       
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "ofbx.h"
// 表示一个顶点属性
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

// 表示obj文件中一个顶点的位置、纹理坐标和法向量 索引
struct VertexCombineIndex
{
	GLuint posIndex;
	GLuint textCoordIndex;
	GLuint normIndex;
};

class ObjLoader
{
public:
	static bool loadFromFile(const char* path,
		std::vector<Vertex>& vertData)
	{

		std::vector<VertexCombineIndex> vertComIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_textCoords;
		std::vector<glm::vec3> temp_normals;

		std::ifstream file(path);
		if (!file)
		{
			std::cerr << "Error::ObjLoader, could not open obj file:"
				<< path << " for reading." << std::endl;
			return false;
		}
		std::string line;
		while (getline(file, line))
		{
			if (line.substr(0, 2) == "vt") // 顶点纹理坐标数据
			{
				printf("uv find\n");
				std::istringstream s(line.substr(2));
				glm::vec2 v;
				s >> v.x;
				s >> v.y;
				v.y = -v.y;  // 注意这里加载的dds纹理 要对y进行反转
				temp_textCoords.push_back(v);
			}
			else if (line.substr(0, 2) == "vn") // 顶点法向量数据
			{
				printf("normal find\n");
				std::istringstream s(line.substr(2));
				glm::vec3 v;
				s >> v.x; s >> v.y; s >> v.z;
				temp_normals.push_back(v);
			}
			else if (line.substr(0, 1) == "v") // 顶点位置数据
			{
				printf("vertex find\n");
				std::istringstream s(line.substr(2));
				glm::vec3 v;
				s >> v.x; s >> v.y; s >> v.z;
				temp_vertices.push_back(v);
			}
			else if (line.substr(0, 1) == "f") // 面数据
			{
				printf("face find\n");
				std::istringstream vtns(line.substr(2));
				std::string vtn;
				while (vtns >> vtn) // 处理一行中多个顶点属性
				{
					VertexCombineIndex vertComIndex;
					std::replace(vtn.begin(), vtn.end(), '/', ' ');
					std::istringstream ivtn(vtn);
					if (vtn.find("  ") != std::string::npos) // 没有纹理数据
					{
						std::cerr << "Error:ObjLoader, no texture data found within file:"
							<< path << std::endl;
						return false;
					}
					ivtn >> vertComIndex.posIndex
						>> vertComIndex.textCoordIndex
						>> vertComIndex.normIndex;

					vertComIndex.posIndex--;
					vertComIndex.textCoordIndex--;
					vertComIndex.normIndex--;
					vertComIndices.push_back(vertComIndex);
				}
			}
			else if (line[0] == '#') // 注释忽略
			{
			}
			else
			{
				// 其余内容 暂时不处理
			}
		}
		//for (/*std::vector<GLuint>::size_type*/int i = 0; i < vertComIndices.size(); ++i)
		//{
		//	Vertex vert;
		//	VertexCombineIndex comIndex = vertComIndices[i];
		//	if (temp_vertices.size() <= comIndex.posIndex || \
				//		temp_vertices.size() <= comIndex.textCoordIndex || \
				//		temp_vertices.size() <= comIndex.normIndex)
		//	{
		//	//	continue;
		//	//}
		//	//{
		//		printf("size:%d, index:%u", temp_vertices.size(), comIndex.textCoordIndex);
		//		vert.position = temp_vertices[comIndex.posIndex];
		//		vert.texCoords = temp_textCoords[comIndex.textCoordIndex];
		//		vert.normal = temp_normals[comIndex.normIndex];

		//		vertData.push_back(vert);
		//	}
		//	
		//}
		printf("temp_vertices.size:%d, temp_textCoords.size:%d, temp_normals.size:%d, vertComIndices.size:%d\n", \
			temp_vertices.size(), temp_textCoords.size(), temp_normals.size(), vertComIndices.size());
		for (int i = 0; i < temp_vertices.size(); ++i)
		{
			printf("vertex:%f, %f, %f\n", temp_vertices[i].x, temp_vertices[i].y, temp_vertices[i].z);
		}
		for (int i = 0; i < temp_textCoords.size(); ++i)
		{
			printf("uv:%f, %f\n", temp_textCoords[i].x, temp_textCoords[i].y);
		}

		for (int i = 0; i < temp_normals.size(); ++i)
		{
			printf("normal:%f, %f, %f\n", temp_normals[i].x, temp_normals[i].y, temp_normals[i].z);
		}

		for (int i = 0; i < temp_vertices.size(); ++i)
		{
			Vertex vert;
			vert.position = temp_vertices[i];
			vert.texCoords = temp_textCoords[i];
			vert.normal = temp_normals[i];

			vertData.push_back(vert);
		}

		return true;
	}
};
extern void transform_FBX_2_Object(ofbx::IScene& scene, std::vector<Vertex>& vertData);
extern void loadFBXFile(const char *path, std::vector<Vertex>& vertData);
