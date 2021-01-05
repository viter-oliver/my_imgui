#include "file_outputor.h"
void point_to_underline(string& target)
{
	for (auto& str_u:target)
	{
		if (str_u == '.' || str_u == ' ' || str_u == '-')
		{
			str_u = '_';
		}
	}
}
file_outputor::file_outputor( string output_path ) 
{
     _fout.open( output_path );
}
file_outputor::~file_outputor()
{
     _fout.close();
}
void file_outputor::begin_enum(string enum_file)
{
	//point_to_underline(enum_file);
	_fout << "enum " << enum_file << "{" << endl;
}

void file_outputor::push_enum_name(string enum_name)
{
	point_to_underline(enum_name);
	_fout << "en_" << enum_name <<","<< endl;
}

void file_outputor::end_enum()
{
	_fout << "};" << endl;
}
