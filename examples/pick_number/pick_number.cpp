#include <stdio.h>
#include <fstream>
#include <string>
using namespace std;
bool is_numb(char c)
{
	return c >= '0'&&c <= '9';
}
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("invalid number of parameters\n");
		return 0;
	}

	string file_path = argv[1];
	string file_name = file_path.substr(file_path.find_last_of('\\') + 1);
	string file_dir = file_path.substr(0, file_path.find_last_of('\\') + 1);
	ifstream fin;
	fin.open(file_path);

	if (!fin.is_open())
	{
		printf("invalid file:%s\n", argv[1]);
		return 0;
	}
	ofstream fout;
	string file_output = file_path + "_o";
	fout.open(file_output, ios::out | ios::trunc);
#define buff_len 0x100
	char cbuff[buff_len];
	while (fin.getline(cbuff,buff_len))
	{
		int clen = strlen(cbuff);
		for (int ix = 0; ix < clen;)
		{
			if (is_numb(cbuff[ix]))
			{
				int iy = ix + 1;
				int imx = ix + 30;
				int ib = ix + 10;
				for (; iy <imx ;iy++)
				{
					if (!is_numb(cbuff[iy]))
					{
						ix = iy + 1;
						break;
					}
				}
				if (iy > ib)
				{
					cbuff[iy] = '\n';
					if (cbuff[iy - 11]=='1')
					{
						fout.write(&cbuff[iy - 11], 12);
					}
					ix = iy;
				}
				
			}
			else
			{
				ix++;
			}
		}
	}
	fin.close();
	fout.close();
	
	return 0;
}

