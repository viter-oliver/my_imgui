#include "Mesh.h"

void transform_FBX_2_Object(ofbx::IScene& scene, std::vector<Vertex>& vertData)
{

	std::vector<VertexCombineIndex> vertComIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_textCoords;
	std::vector<glm::vec3> temp_normals;

	int obj_idx = 0;
	int indices_offset = 0;
	int normals_offset = 0;
	int mesh_count = scene.getMeshCount();

	printf("mesh_count:%d\n", mesh_count);
	for (int i = 0; i < mesh_count; ++i)
	{
		const ofbx::Mesh& mesh = *scene.getMesh(i);
		const ofbx::Geometry& geom = *mesh.getGeometry();
		int vertex_count = geom.getVertexCount();
		const ofbx::Vec3* vertices = geom.getVertices();
		for (int i = 0; i < vertex_count; ++i)
		{
			//ofbx::Vec3 v = vertices[i];
			glm::vec3 v;
			v.x = vertices[i].x;
			v.y = vertices[i].y;
			v.z = vertices[i].z;
			temp_vertices.push_back(v);
		}

		bool has_normals = geom.getNormals() != nullptr;
		if (has_normals)
		{
			const ofbx::Vec3* normals = geom.getNormals();
			int count = geom.getVertexCount();
			for (int i = 0; i < count; ++i)
			{
				//ofbx::Vec3 n = normals[i];
				glm::vec3 n;
				n.x = normals[i].x;
				n.y = normals[i].y;
				n.z = normals[i].z;
				temp_normals.push_back(n);
			}
		}

		bool has_uvs = geom.getUVs() != nullptr;
		if (has_uvs)
		{
			const ofbx::Vec2* uvs = geom.getUVs();
			int count = geom.getVertexCount();
			for (int i = 0; i < count; ++i)
			{
				//ofbx::Vec2 uv = uvs[i];
				glm::vec2 uv;
				uv.x = uvs[i].x;
				uv.y = 1 - uvs[i].y;  //这里取纹理坐标的反即用1-该值
				temp_textCoords.push_back(uv);
			}
		}

		bool new_face = true;
		int count = geom.getVertexCount();
		for (int i = 0; i < count; ++i)
		{
			if (new_face)
			{
				new_face = false;
			}
			VertexCombineIndex ff;
			int idx = i + 1;
			int vertex_idx = indices_offset + idx;
			ff.posIndex = vertex_idx;

			if (has_normals)
			{
				ff.normIndex = idx;
			}
			
			if (has_uvs)
			{
				ff.textCoordIndex = idx;
				
			}

			new_face = idx < 0;
			vertComIndices.push_back(ff);
		}

		indices_offset += vertex_count;
		++obj_idx;
	}

	printf("vertices.size:%d, uv.size:%d, normal.size:%d", temp_vertices.size(), temp_textCoords.size(), temp_normals.size());
	//法向量、顶点、材质坐标可能不对应，目前先取最小的(注：可能丢失点)
#define Custom_Max(a, b) a>b?a:b
#define Custom_Min(a, b) a<b?a:b

	int tmp = Custom_Min(temp_vertices.size(), temp_textCoords.size());
	tmp = Custom_Min(tmp, temp_normals.size());
	for (int i = 0; i < tmp/*temp_vertices.size()*/; ++i)
	{
		Vertex vert;
		vert.position = temp_vertices[i];
		vert.texCoords = temp_textCoords[i];
		vert.normal = temp_normals[i];

		vertData.push_back(vert);
	}
}
void loadFBXFile(const char *path, std::vector<Vertex>& vertData)
{
	FILE* fp = fopen(path, "rb");
	if (!fp) return;
	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	auto* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);
	auto pscene = ofbx::load((ofbx::u8*)content, file_size);
	transform_FBX_2_Object(*pscene, vertData);
	pscene->destroy();
}