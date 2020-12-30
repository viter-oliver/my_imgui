// test_miniz.cpp : Defines the entry point for the console application.
//

#include "json.h"
#ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR
#include "json_batchallocator.h"
#endif // #ifndef JSON_USE_SIMPLE_INTERNAL_ALLOCATOR
#include <fstream>
#include <string>
using namespace std;
using namespace Json;
void afg_txt_2_vi2( Value& jobj )
{
     Value::Members memb=jobj.getMemberNames();
     for( auto& itemm:memb )
     {
          if (itemm.size()<5)
          {
               continue;
          }
          string rg = itemm.substr( itemm.length() - 4, 4 );
          Value& jtar = jobj[ itemm ];
          if (rg=="_txt")
          {
               auto ivalue = jtar.asInt();
               Value jv2( objectValue );
               jv2[ "x" ] = 0;
               jv2[ "y" ] = ivalue;
               jtar = jv2;
          }
          if( jtar.isObject() )
          {
               afg_txt_2_vi2( jtar );
          }
          if (jtar.isArray())
          {
               int idx = 0;
               int isize = jtar.size();
               for( ; idx < isize;++idx )
               {
                    Value& jchild = jtar[ idx ];
                    if (jchild.isObject())
                    {
                         afg_txt_2_vi2( jchild );
                    }
               }
          }
     }
}
int main(int argc, char* argv[])
{
	if (argc<2)
	{
		printf("invalid number of parameters\n");
		return 0;
	}

	string file_path = argv[1];
     string file_out = file_path.substr( 0, file_path.size()-4);
     file_out += "_o.afg";
	ifstream fin;
     fin.open( file_path );
     if (!fin.is_open())
     {
          printf( "fail to open %s\n", file_path.c_str() );
          return 0;
     }
     Reader reader;
     Value jroot;
     if( !reader.parse( fin, jroot, false ) )
     {
          printf( "fail to parse %s\n", file_path.c_str() );
          return 0;
     }
     fin.close();
     afg_txt_2_vi2( jroot );
     ofstream fout;
     fout.open( file_out );
     StyledStreamWriter jwr;
     jwr.write( fout, jroot );
     fout.close();
	return 0;
}

