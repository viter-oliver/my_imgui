#include "common_functions.h"
#include <math.h>
#include "SOIL.h"
#include "texture.h"
#include "json.h"
#include <locale>
#include <codecvt>
#include <fstream>
#include <sstream>
std::string wstringToUtf8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t> > strCnv;
	return strCnv.to_bytes(str);
}
using namespace std;
bool handle_file_data(const std::string& file_name, std::function<void(char*, unsigned int)>buff_handle)
{
	ifstream fin;
	fin.open(file_name, ios::binary);
	if (!fin.is_open())
	{
		return false;
	}
	auto file_size = fin.tellg();
	fin.seekg(0, ios::end);
	file_size = fin.tellg() - file_size;
	fin.seekg(0, ios::beg);
	char* pbuff = new char[file_size];
	fin.read(pbuff, file_size);
	buff_handle(pbuff, file_size);
	delete[] pbuff;
	fin.close();
	return true;
}
std::wstring utf8ToWstring(const std::string& str)
{
	std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;
	return strCnv.from_bytes(str);
}
/*
x'=(x-a)cosα+(y-b)sinα+a
y'=-(x-a)sinα+(y-b)cosα+b
*/
using namespace Json;

GLfloat* get_txt_uvs(const char* data_file, int& retn_len)
{
	GLfloat* puvs = NULL;
	Reader reader;
	Value jvalue;
	if (reader.parse(data_file, jvalue, false))
	{
		Value& meta = jvalue["meta"];
		Value& jsize = meta["size"];
		float w = jsize["w"].asInt();
		float h = jsize["h"].asInt();
		Value& frames = jvalue["frames"];
		if (frames.isArray())
		{
			int isize = frames.size();
			retn_len = isize * 8;
			puvs = new GLfloat[retn_len];
			for (int ix = 0; ix < isize; ix++)
			{
				Value& frame_unit = frames[ix];
				Value& frame = frame_unit["frame"];
				int sbidx = ix * 8;
				float x0 = frame["x"].asInt();
				float y0 = frame["y"].asInt();
				float x1 = x0 + frame["w"].asInt();
				float y1 = y0 + frame["h"].asInt();

				puvs[sbidx + 0] = x0 / w;
				puvs[sbidx + 1] = y0 / h;

				puvs[sbidx + 2] = x1 / w;
				puvs[sbidx + 3] = y0 / h;
				puvs[sbidx + 4] = x0 / w;
				puvs[sbidx + 5] = y1 / h;
				puvs[sbidx + 6] = x1 / w;
				puvs[sbidx + 7] = y1 / h;
			}
		}
		if (frames.isObject())
		{
			Value::Members memb(frames.getMemberNames());
			retn_len = memb.size() * 8;
			puvs = new GLfloat[retn_len];
			int idx = 0;
			for (auto itmem = memb.begin(); itmem != memb.end(); ++itmem, ++idx)
			{
				auto& mname = *itmem;
				Value& junif = frames[mname];
				Value& frame = junif["frame"];
				int sbidx = idx * 8;
				float x0 = frame["x"].asInt();
				float y0 = frame["y"].asInt();
				float x1 = x0 + frame["w"].asInt();
				float y1 = y0 + frame["h"].asInt();

				puvs[sbidx + 0] = x0 / w;
				puvs[sbidx + 1] = y0 / h;

				puvs[sbidx + 2] = x1 / w;
				puvs[sbidx + 3] = y0 / h;
				puvs[sbidx + 4] = x0 / w;
				puvs[sbidx + 5] = y1 / h;
				puvs[sbidx + 6] = x1 / w;
				puvs[sbidx + 7] = y1 / h;
				/*puvs[sbidx + 0] = x1 / w;
				puvs[sbidx + 1] = y0 / h;
				puvs[sbidx + 2] = x1 / w;
				puvs[sbidx + 3] = y1 / h;
				puvs[sbidx + 4] = x0 / w;
				puvs[sbidx + 5] = y0 / h;
				puvs[sbidx + 6] = x1 / w;
				puvs[sbidx + 7] = y0 / h;*/

			}
		}
		
	}
	return puvs;
}
void get_txt_uv_vector(const char* data_file, vres_txt_cd& vtxt_cd)
{
	Reader reader;
	Value jvalue;
	if (reader.parse(data_file, jvalue, false))
	{
		Value& meta = jvalue["meta"];
		Value& jsize = meta["size"];
		float w = jsize["w"].asInt();
		float h = jsize["h"].asInt();
		Value& frames = jvalue["frames"];
		if (frames.isArray())
		{
			int isize = frames.size();
			for (int ix = 0; ix < isize; ix++)
			{
				vtxt_cd.emplace_back();
				res_texture_coordinate& res_txt_cd = vtxt_cd[ix];
				Value& frame_unit = frames[ix];
				Value& frame = frame_unit["frame"];
				Value& filename = frame_unit["filename"];
				res_txt_cd._file_name = filename.asString();

				bool rotated = frame["rotated"].asBool();
				res_txt_cd._x0 = frame["x"].asInt();
				res_txt_cd._y0 = frame["y"].asInt();
				res_txt_cd._x1 = frame["x"].asInt() + frame["w"].asInt();
				res_txt_cd._y1 = frame["y"].asInt() + frame["h"].asInt();

				if (rotated)
				{
					res_txt_cd._x1 = frame["x"].asInt() + frame["h"].asInt();;
					res_txt_cd._y1 = frame["y"].asInt() + frame["w"].asInt();
				}
			}
		}
		if (frames.isObject())
		{
			Value::Members memb(frames.getMemberNames());
			int idx = 0;
			for (auto itmem = memb.begin(); itmem != memb.end(); ++itmem, ++idx)
			{
				vtxt_cd.emplace_back();
				res_texture_coordinate& res_txt_cd = vtxt_cd[idx];
				auto& mname = *itmem;
				Value& junif = frames[mname];
				Value& frame = junif["frame"];
				Value& filename = junif["filename"];
				res_txt_cd._file_name = filename.asString();

				bool rotated = frame["rotated"].asBool();
				res_txt_cd._x0 = frame["x"].asInt();
				res_txt_cd._y0 = frame["y"].asInt();
				res_txt_cd._x1 = frame["x"].asInt() + frame["w"].asInt();
				res_txt_cd._y1 = frame["y"].asInt() + frame["h"].asInt();

				if (rotated)
				{
					res_txt_cd._x1 = frame["x"].asInt() + frame["h"].asInt();;
					res_txt_cd._y1 = frame["y"].asInt() + frame["w"].asInt();
				}
			}
		}

	}
}
AFG_EXPORT ImVec2 rotate_point_by_zaxis( ImVec2& tar, float angle, ImVec2& basePoint )
{
	ImVec2 des;
	des.x = (tar.x - basePoint.x)*cos(angle) + (tar.y - basePoint.y)*sin(angle) + basePoint.x;
	des.y = -(tar.x - basePoint.x)*sin(angle) + (tar.y - basePoint.y)*cos(angle) + basePoint.y;
	return des;
}
bool prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height)
{
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// 附加 纹理 color attachment
	colorTextId = TextureHelper::makeAttachmentTexture(0, GL_RGBA, frame_width, frame_height, GL_RGBA, GL_UNSIGNED_BYTE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextId, 0);
	// 附加 depth stencil texture attachment
	depthStencilTextId = TextureHelper::makeAttachmentTexture(0, GL_DEPTH24_STENCIL8, frame_width,
		frame_height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTextId, 0);
	// 检测完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
bool af_prepareFBO1(GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId, GLuint frame_width, GLuint frame_height)
{
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// 附加 纹理 color attachment
	glGenTextures(1, &colorTextId);
	glBindTexture(GL_TEXTURE_2D, colorTextId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, frame_width, frame_height, 0, GL_DEPTH_COMPONENT,GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,colorTextId, 0);

	// 附加 depth stencil texture attachment
	glGenTextures(1, &depthStencilTextId);
	glBindTexture(GL_TEXTURE_2D, depthStencilTextId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, frame_width, frame_height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilTextId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTextId, 0);

	// 检测完整性
	auto chk = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (chk != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("chk=0x%x\n", chk);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
bool frame_buffer()
{
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	GLuint texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	GLuint rboDepthStencil;
	glGenRenderbuffers(1, &rboDepthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	return true;
}
/*
* 附加纹理到Color Attachment
* 同时附加RBO到depth stencil Attachment
*/
bool prepareFBO2(GLuint& textId, GLuint& fboId, GLuint frame_width, GLuint frame_height)
{
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// 附加纹理 color attachment
	textId = TextureHelper::makeAttachmentTexture(0, GL_RGB, frame_width, frame_height, GL_RGB, GL_UNSIGNED_BYTE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textId, 0);
	// 附加 depth stencil RBO attachment
	GLuint rboId;
	glGenRenderbuffers(1, &rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
		frame_width, frame_height); // 预分配内存
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

#include <io.h>
#include<direct.h>

bool fileExist(const char* fileName)
{
	WIN32_FIND_DATA wfd;
	HANDLE hHandle = ::FindFirstFile(fileName, &wfd);
	if (hHandle == INVALID_HANDLE_VALUE)
		return false;
	else
		return (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;

}

bool directoryExist(const char* dir)
{
	WIN32_FIND_DATA wfd;
	HANDLE hHandle = ::FindFirstFile(dir, &wfd);
	if (hHandle == INVALID_HANDLE_VALUE)
		return access(dir, 0) == 0; // if dir is a drive disk path like c:\,we thought is a directory too.  	
	else
		return (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}


bool createDirectory(const char* pathName)
{
	char path[MAX_PATH] = { 0 };
	const char* pos = pathName;
	while ((pos = strchr(pos, '\\')) != NULL)
	{
		memcpy(path, pathName, pos - pathName + 1);
		pos++;
		if (directoryExist(path))
		{
			continue;
		}
		else
		{
			int ret = _mkdir(path);
			if (ret == -1)
			{
				return false;
			}
		}
	}
	pos = pathName + strlen(pathName) - 1;
	if (*pos != '\\')
	{
		return _mkdir(pathName) == 0;
	}
	return true;
}



bool createFileWithDirectory(const char* pathName)
{
	if (fileExist(pathName))
		return true;
	int len = strlen(pathName);
	if (len <= 0)
		return false;

	char strTmpPath[MAX_PATH] = { 0 };
	strcpy(strTmpPath, pathName);
	char* q = strTmpPath + len - 1;
	for (int i = 0; i < len - 1; i++, q--)
	{
		if (*q == '\\')
		{
			*q = '\0';
			q++;
			break;
		}
	}
	if (strlen(strTmpPath) > 0 && strlen(q) > 0)
	{
		createDirectory(strTmpPath);
		FILE* hFile = fopen(pathName, "w");
		if (hFile)
		{
			fclose(hFile);
			return true;
		}
		else
			return false;

	}
	else
	{
		return false;
	}

}
void align_expression(string& exp, string&expo)
{
	stringstream sexp(exp);
	string line;
	while (getline(sexp,line))
	{
		expo += "  ";
		expo += line;
		expo += '\n';
	}
}
void trim_align_expression(string& exp, string&expo)
{
	stringstream sexp(exp);
	string line;
	while (getline(sexp, line))
	{
	//	expo += "  ";
		expo += line.substr(2);
		expo += '\n';
	}
}

const char hex_enc_table[] = "abcdefghijklmnop";
void convert_binary_to_string(char* pbin, int len, string& out_str)
{
	out_str.resize(len * 2);
	int ix = 0;
	for (; ix < len; ix++)
	{
		auto bin_value = *(unsigned char*)pbin;
		out_str[ix * 2] = hex_enc_table[bin_value >> 4];
		out_str[ix * 2 + 1] = hex_enc_table[bin_value & 0x0f];
		pbin++;
	}
}

void convert_string_to_binary(string& in_str, string& out_bin)
{
	auto out_size = in_str.size() / 2;
	out_bin.resize(out_size);
	int ix = 0;
	for (; ix < out_size; ix++)
	{
		unsigned char high = in_str[ix * 2] - 'a';
		unsigned char low = in_str[ix * 2 + 1] - 'a';
		out_bin[ix] = high << 4 | low;
	}
}
unsigned int conver_track_buff_to_pair(char* pbuff,unsigned int buff_len, vector<ImVec2>& vtrack0, vector<ImVec2>& vtrack1)
{
	int* parray_len = (int*)pbuff;
	int array_len = *parray_len;
	int wtrack_sz = sizeof(ImVec2)*array_len ;
	int bf_track_sz = buff_len - sizeof(int);
	if (wtrack_sz* 2!=bf_track_sz)
	{
		printf("invalid slider track buff\n");
		return 0;
	}
	char* phead0 = pbuff + sizeof(int);
	vtrack0.resize(array_len);
	memcpy(&vtrack0[0], phead0, wtrack_sz);
	char* phead1 = phead0 + wtrack_sz;
	vtrack1.resize(array_len);
	memcpy(&vtrack1[0], phead1, wtrack_sz);
	return array_len;
}
//5点3次平滑法
bool smooth_algorithm_5_points_3_times(vector<ImVec2>& point_list, bool x_direction)
{
	int pt_cnt = point_list.size();
	if (pt_cnt<5)
	{
		return false;
	}
	vector<float> t_v;
	t_v.resize(pt_cnt);
	if (x_direction)
	{
		t_v[0] = (5 * point_list[0].x + 2 * point_list[1].x - point_list[2].x)  / 6;
		t_v[pt_cnt - 1] = (2 * point_list[pt_cnt - 2].x - point_list[pt_cnt - 3].x + 5 * point_list[pt_cnt - 1].x) / 6;
		for (int i = 1; i < pt_cnt - 1;i++)
		{
			t_v[i] = (point_list[i - 1].x + point_list[i].x + point_list[i + 1].x) / 3;
		}
		for (int i = 0; i < pt_cnt; i++)
		{
			point_list[i].x = t_v[i];
		}
	}
	else
	{
		t_v[0] = (5 * point_list[0].y + 2 * point_list[1].y - point_list[2].y) / 6;
		t_v[pt_cnt - 1] = (2 * point_list[pt_cnt - 2].y - point_list[pt_cnt - 3].y + 5 * point_list[pt_cnt - 1].y) / 6;
		for (int i = 1; i < pt_cnt - 1; i++)
		{
			t_v[i] = (point_list[i - 1].y + point_list[i].y + point_list[i + 1].y) / 3;
		}
		for (int i = 0; i < pt_cnt; i++)
		{
			point_list[i].y = t_v[i];
		}

	}
	return true;
}
//直线3点平均平滑法
bool smooth_algoritm_3_points_average(vector<ImVec2>& point_list, bool x_direction)
{
	int pt_cnt = point_list.size();
	if (pt_cnt < 4)
	{
		return false;
	}
	vector<float> t_v;
	t_v.resize(pt_cnt);
	if (x_direction)
	{
		t_v[0] = (3 * point_list[0].x + 2 * point_list[1].x+point_list[2].x - point_list[4].x) / 5;
		t_v[1] = (4 * point_list[0].x + 3 * point_list[1].x + 2 * point_list[2].x + point_list[3].x) / 10;
		t_v[pt_cnt - 2] = (point_list[pt_cnt - 4].x + 2 * point_list[pt_cnt - 3].x + 3 * point_list[pt_cnt - 2].x+ 4 * point_list[pt_cnt - 1].x) /10;
		t_v[pt_cnt - 1] = (point_list[pt_cnt - 3].x - point_list[pt_cnt - 5].x + 2 * point_list[pt_cnt - 2].x + 3 * point_list[pt_cnt - 1].x) / 5;
		for (int i = 2; i < pt_cnt - 2; i++)
		{
			t_v[i] = (point_list[i - 2].x + point_list[i - 1].x + point_list[i].x + point_list[i + 1].x + point_list[i + 2].x) / 5;
		}
		for (int i = 0; i < pt_cnt; i++)
		{
			point_list[i].x = t_v[i];
		}
	}
	else
	{
		t_v[0] = (3 * point_list[0].y + 2 * point_list[1].y + point_list[2].y - point_list[4].y) / 5;
		t_v[1] = (4 * point_list[0].y + 3 * point_list[1].y + 2 * point_list[2].y + point_list[3].y) / 10;
		t_v[pt_cnt - 2] = (point_list[pt_cnt - 4].y + 2 * point_list[pt_cnt - 3].y + 3 * point_list[pt_cnt - 2].y + 4 * point_list[pt_cnt - 1].y) / 10;
		t_v[pt_cnt - 1] = (point_list[pt_cnt - 3].y - point_list[pt_cnt - 5].y + 2 * point_list[pt_cnt - 2].y + 3 * point_list[pt_cnt - 1].y) / 5;
		for (int i = 2; i < pt_cnt - 2; i++)
		{
			t_v[i] = (point_list[i - 2].y + point_list[i - 1].y + point_list[i].y + point_list[i + 1].y + point_list[i + 2].y) / 5;
		}
		for (int i = 0; i < pt_cnt; i++)
		{
			point_list[i].y = t_v[i];
		}
	}
	return true;
}
//直线5点平均平滑法
bool smooth_algorithm_5_points_average(vector<ImVec2>& point_list, bool x_direction)
{
	int pt_cnt = point_list.size();
	if (pt_cnt <5)
	{
		return false;
	}
	vector<float> t_v;
	t_v.resize(pt_cnt);
	if (x_direction)
	{
		t_v[0] = (69 * point_list[0].x + 4 * (point_list[1].x + point_list[3].x) - 6*point_list[3].x - point_list[5].x) / 70;
		t_v[1] = (2 * (point_list[0].x + point_list[4].x) + 27 * point_list[1].x + 12 * point_list[2].x - 8*point_list[3].x) / 35;
		t_v[pt_cnt - 2] = (2 * (point_list[pt_cnt - 5].x+point_list[pt_cnt - 1].x) - 8 * point_list[pt_cnt - 4].x + 12 * point_list[pt_cnt - 3].x + 27 * point_list[pt_cnt - 2].x) / 35;
		t_v[pt_cnt - 1] = (4 * (point_list[pt_cnt - 4].x + point_list[pt_cnt - 2].x) - point_list[pt_cnt - 5].x + 6 * point_list[pt_cnt - 3].x + 69 * point_list[pt_cnt - 1].x) / 70;
		for (int i = 2; i < pt_cnt - 2; i++)
		{
			t_v[i] = (12*(point_list[i - 1].x + point_list[i + 1].x) -3*( point_list[i-2].x + point_list[i + 2].x) + 17*point_list[i].x) / 35;
		}
		for (int i = 0; i < pt_cnt; i++)
		{
			point_list[i].x = t_v[i];
		}
	}
	else
	{
		t_v[0] = (69 * point_list[0].y + 4 * (point_list[1].y + point_list[3].y) - 6 * point_list[3].y - point_list[5].y) / 70;
		t_v[1] = (2 * (point_list[0].y + point_list[4].y) + 27 * point_list[1].y + 12 * point_list[2].y - 8 * point_list[3].y) / 35;
		t_v[pt_cnt - 2] = (2 * (point_list[pt_cnt - 5].y + point_list[pt_cnt - 1].y) - 8 * point_list[pt_cnt - 4].y + 12 * point_list[pt_cnt - 3].y + 27 * point_list[pt_cnt - 2].y) / 35;
		t_v[pt_cnt - 1] = (4 * (point_list[pt_cnt - 4].y + point_list[pt_cnt - 2].y) - point_list[pt_cnt - 5].y + 6 * point_list[pt_cnt - 3].y + 69 * point_list[pt_cnt - 1].y) / 70;
		for (int i = 2; i < pt_cnt - 2; i++)
		{
			t_v[i] = (12 * (point_list[i - 1].y + point_list[i + 1].y) - 3 * (point_list[i - 2].y + point_list[i + 2].y) + 17 * point_list[i].y) / 35;
		}
		for (int i = 0; i < pt_cnt; i++)
		{
			point_list[i].y = t_v[i];
		}
	}
	return true;
	return true;
}


static BYTE Clip(int clr)
{
	return (BYTE)(clr < 0 ? 0 : (clr > 255 ? 255 : clr));
}

static RGBQUAD ConvertYCrCbToRGB(int y, int cr, int cb)
{
	RGBQUAD rgbq = { 0 };

	int c = y - 16;
	int d = cb - 128;
	int e = cr - 128;

	rgbq.rgbRed = Clip((298 * c + 409 * e + 128) >> 8);
	rgbq.rgbGreen = Clip((298 * c - 100 * d - 208 * e + 128) >> 8);
	rgbq.rgbBlue = Clip((298 * c + 516 * d + 128) >> 8);
	/*rgbq.rgbRed =y+1.402f*(cr-128);
	rgbq.rgbGreen =y-0.34414f*(cb-128)-0.71414f*(cr-128);
	rgbq.rgbBlue = y+1.772f*(cb-128);*/

	//rgbq.rgbRed = 0xff - rgbq.rgbRed;
	//rgbq.rgbGreen = 0xff - rgbq.rgbGreen;
	rgbq.rgbReserved = 0xff;
	return rgbq;
}

void FromYUY2ToRGB32(LPVOID lpDest, LPVOID lpSource, LONG lWidth, LONG lHeight)
{
	RGBQUAD *pDestPel = (RGBQUAD*)lpDest;
	WORD *pSrcPel = (WORD*)lpSource;

	for (int y = 0; y < lHeight; y++)
	{
		for (int x = 0; x < lWidth; x += 2)
		{
			//
			// Byte order is U0 Y0 V0 Y1
			//
			int y0 = (int)LOBYTE(pSrcPel[x]);
			int u0 = (int)HIBYTE(pSrcPel[x]);
			int y1 = (int)LOBYTE(pSrcPel[x + 1]);
			int v0 = (int)HIBYTE(pSrcPel[x + 1]);

			pDestPel[x] = ConvertYCrCbToRGB(y0, v0, u0);
			pDestPel[x + 1] = ConvertYCrCbToRGB(y1, v0, u0);
		}

		pSrcPel += lWidth;
		pDestPel += lWidth;
	}
}