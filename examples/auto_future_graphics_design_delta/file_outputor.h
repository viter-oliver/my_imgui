#pragma once
#include <string>
#include <fstream>
using namespace std;
class file_outputor
{
	string _output_path;
	ofstream _fout;
public:
	file_outputor(string output_path) :_output_path(output_path){}
	~file_outputor(){}
	void begin_enum_file(string enum_file);
	void push_enum_name(string enum_name);
	void end_enum_file();
};

