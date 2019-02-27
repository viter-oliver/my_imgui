#include <algorithm>
#include <GLES2/gl2.h>
#include <memory>
#include <map>
#include <iostream>
#include "property_utilities.h"
#define GLFW_INCLUDE_ES2
#define IMGUI_DISABLE_DEMO_WINDOWS 
#define _LIBCPP_VERSION
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

using namespace std;
const unsigned char name_len = 40;
class test_pp
{
protected:
	vp_prop_ele _vprop_eles;
	DEF_STRUCT_WITH_INIT(base_prop, _in_p,
			(float, _posx, {0.f}),
			(float, _posy, {0.f}),
			(bool, _visible, {true}),
			(char, _name[name_len]))
public:
      test_pp()
      {
		    cout<<"size of base_prop:"<<sizeof(base_prop)<<endl;
			for(auto prop_ele:_vprop_eles)
			{
					auto& prop_page = prop_ele->_pro_page;
					for (auto& memb:prop_page)
				{
					auto mtype = memb->_type;
					auto mname = memb->_name;
					cout<<mname<<mtype<<endl;
				}
				cout<<"size of base_prop_bk"<<prop_ele->_pro_sz<<endl;
				
			}
			
	  }
	
};
struct vtype_size
{
	int _utsize,_cnt;
	//vtype_size(int us,int ct) :_utsize(us),_cnt(ct){}
	//vtype_size()=default;
	//~vtype_size()=default;
};
static map<int,char> test_map={{1,'1'},{2,'2'},{3,'3'},{4,'4'},{5,'5'}};
static map < GLenum, vtype_size> shader_variable_type_size =
{
	{ GL_FLOAT, { sizeof(float), 1 } },
	{ GL_FLOAT_VEC2, { sizeof(float), 2 } }, 
	{ GL_FLOAT_VEC3, { sizeof(float), 3 } }, 
	{ GL_FLOAT_VEC3, { sizeof(float), 3 } }, 
	{ GL_FLOAT_VEC4, { sizeof(float), 4 } }, 
	{ GL_BOOL, { sizeof(bool), 1 }}, 
	{ GL_BOOL_VEC2, { sizeof(bool), 2 } }, 
	{ GL_BOOL_VEC3, { sizeof(bool), 3 } }, 
	{ GL_BOOL_VEC4, { sizeof(bool), 4 } }, 
	{ GL_FLOAT_MAT2, {sizeof(float), 2 * 2 } }, 
	{ GL_FLOAT_MAT3, { sizeof(float), 3 * 3 } }, 
	{ GL_FLOAT_MAT4, { sizeof(float), 4 * 4 } }, 
	{ GL_INT, { sizeof(int), 1 }}, 
	{ GL_INT_VEC2, { sizeof(int), 2 } }, 
	{ GL_INT_VEC3, { sizeof(int), 3 } }, 
	{ GL_INT_VEC3, { sizeof(int), 3 } }, 
	{ GL_INT_VEC4, { sizeof(int), 4 } }, 
	{ GL_UNSIGNED_INT, { sizeof(unsigned int), 1 } }, 
	
};

int main(int argc, char *argv[])
{
	int fd=open(argv[1],O_RDONLY);
	test_pp tpp;
    return 0;
}
