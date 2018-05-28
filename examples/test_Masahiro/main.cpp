#include <iostream>
#include <fstream>

#include <msgpack11.hpp>

using namespace msgpack11;

int main(int argc, char const *argv[]) {
	std::ofstream fout;
	fout.open("./dump.mp");
	MsgPack mg_pj;
	MsgPack::object mg_root = {
		{ "screenw", 512 },
		{ "screenh", 512 },
		{ "current_txt_index", 15 }
	};
	MsgPack::array mg_res_list;
	for (int i = 0; i < 64; i++)
	{
		MsgPack::binary txtdata;
		txtdata.reserve(512 * 512 * 4);
		txtdata.resize(512 * 512 * 4);
		MsgPack::array texture_coordinate_list;
		for (int j = 0; j < 64; j++)
		{
			texture_coordinate_list.push_back(MsgPack::object{
				{ "file_name", "aaaaaaaa" },
				{ "x0", 256 },
				{ "y0", 512 },
				{ "x1", 1024 },
				{ "y1", 2048 }
			});
		}
		mg_res_list.push_back(MsgPack::object{
			{ "txtpk_file", txtdata },
			{ "txtpk_fmt", texture_coordinate_list }
		});
	}
	mg_root["txtres_list"] = mg_res_list;

	mg_root["control_objects"] = MsgPack::object{};
	mg_pj = mg_root;
	std::string str_output;
	mg_pj.dump(str_output);
	fout << str_output << std::endl;
	fout.close();
	return 0;
}