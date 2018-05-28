// test_msgpack-c.cpp : Defines the entry point for the console application.
//
#include<msgpack.hpp>
#include <sstream>
#include <memory>
#include <iostream>
static char* test_content[] =
{
	"hello world",
	"hello msgpack",
	"hello test msgpack",
};
static char* child_content[] =
{
	"name0",
	"name1",
	"name2",
};

int main(int argc, char* argv[])
{
	std::stringstream sbuf;
	msgpack::packer<std::stringstream> pk(sbuf);
	float w = 1920.f, h = 720.f;
	pk.pack_array(3);
	pk.pack_float(w);
	pk.pack_float(h);
	pk.pack_array(sizeof(test_content) / sizeof(char*));
	for (auto it : test_content)
	{
		pk.pack_array(2);
		int sl=strlen(it);
		pk.pack_bin(sl);
		pk.pack_bin_body(it, sl);
		pk.pack_array(sizeof(child_content) / sizeof(char*));
		for (auto iit : child_content)
		{
			pk.pack_array(5);
			int csl = strlen(iit);
			pk.pack_str(csl);
			pk.pack_str_body(iit, csl);
			float x0 = 1.f, x1 = 2.f, x2 = 3.f, x3 = 4.f;
			pk.pack_float(x0);
			pk.pack_float(x1);
			pk.pack_float(x2);
			pk.pack_float(x3);
		}
	}
	/*auto oh = msgpack::unpack(sbuf.str().data(), sbuf.str().size());
	std::cout << oh.get() << std::endl;*/

	msgpack::object_handle oh;
	msgpack::unpacker unpac;
	unpac.reserve_buffer(sbuf.str().size());
	memcpy(unpac.buffer(), sbuf.str().data(), sbuf.str().size());
	unpac.buffer_consumed(sbuf.str().size());
	unpac.next(oh);
	auto obj = oh.get();
	float uw= obj.via.array.ptr[0].as<float>();
	float uh= obj.via.array.ptr[1].as<float>();
	auto& chd_obj = obj.via.array.ptr[2];

	std::cout << chd_obj.type << std::endl;//6
	auto sz = chd_obj.via.array.size;
	for (size_t i = 0; i < sz; i++)
	{
		auto obj_bin=chd_obj.via.array.ptr[i];
		std::cout << obj_bin.type << std::endl;//6
		std::cout << obj_bin.via.array.size << std::endl;
		auto obj_str = obj_bin.via.array.ptr[0];
		std::cout << obj_str.type << std::endl;//BIN(8)
		auto obj_str_sz = obj_str.via.bin.size;
		std::cout << obj_str_sz << std::endl;//
		char*pstr = new char[obj_str_sz];
		memcpy(pstr, obj_str.via.bin.ptr, obj_str_sz);
		std::cout << pstr << std::endl;

		auto a_array = obj_bin.via.array.ptr[1];
		std::cout << a_array.type << std::endl;//6
		auto asz=a_array.via.array.size;//
		auto achd = a_array.via.array.ptr[0];
		std::cout << achd.type << std::endl;//ARRAY(6)
		auto achd_sz = achd.via.array.size;//5
		auto cld_obj = achd.via.array.ptr[0];
		std::cout << cld_obj.type << std::endl;//STR(5)
		auto chld_obj_sz = cld_obj.via.str.size;
		char* pchd_str = new char[chld_obj_sz];
		memcpy(pchd_str, cld_obj.via.str.ptr, chld_obj_sz);
		std::cout << pchd_str << std::endl;//
		delete[] pstr;
		delete[] pchd_str;
		std::cout << achd.via.array.ptr[1].as<float>() << std::endl;
		std::cout << achd.via.array.ptr[2].as<float>() << std::endl;
		std::cout << achd.via.array.ptr[3].as<float>() << std::endl;
		std::cout << achd.via.array.ptr[4].as<float>() << std::endl;

	}
	return 0;
}

