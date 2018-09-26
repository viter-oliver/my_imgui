#include "file_outputor.h"
void point_to_underline(string& target)
{
	for (auto& str_u:target)
	{
		if (str_u=='.')
		{
			str_u = '_';
		}
	}
}
void file_outputor::begin_enum_file(string enum_file)
{
	string file_name = _output_path + enum_file;
	_fout.open(file_name);
	point_to_underline(enum_file);
	_fout << "enum " << enum_file << "{" << endl;
}

void file_outputor::push_enum_name(string enum_name)
{
	point_to_underline(enum_name);
	_fout << "en_" << enum_name <<","<< endl;
}

void file_outputor::end_enum_file()
{
	_fout << "};" << endl;
	_fout.close();
}
