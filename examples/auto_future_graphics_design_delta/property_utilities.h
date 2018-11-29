#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stddef.h>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

struct field_ele
{
	std::string _type;
	std::string _name;
	int _tpsz;
	int _offset;
	field_ele(std::string tp, std::string nm, int tpsz, int offset)
		:_type(tp), _name(nm),_tpsz(tpsz),_offset(offset){}
};
typedef std::shared_ptr<field_ele> sp_field_ele;
//
typedef std::vector<sp_field_ele> vt_field_ele;
struct  prop_ele
{
	void* _pro_address;
	int _pro_sz;
	vt_field_ele _pro_page;
	prop_ele(void* paddress,int pro_sz) :_pro_address(paddress),_pro_sz(pro_sz){}
};
#ifdef _MSC_VER 
#define MSC_PACK_HEAD __pragma(pack(push, 1))
#define MSC_PACK_END __pragma(pack(pop))
#define GNU_DEF
#elif defined(__GNUC__)
#define MSC_PACK_HEAD 
#define MSC_PACK_END 
 #define GNU_DEF __attribute__((packed)) 
#endif
typedef std::shared_ptr<prop_ele> sp_prop_ele;
typedef std::vector<sp_prop_ele> vp_prop_ele;
#define GET_OFFSET(st,mb) offsetof(st,mb)
#define _PIKSTR(x) #x
#define PIKSTR(x) _PIKSTR(x)
#define EMP_IN_PPT(x)	ppt->_pro_page.emplace_back(make_shared<field_ele>(x));
#define SPLIT_ELE(ele) PIKSTR(BOOST_PP_TUPLE_ELEM(2,0,ele)),PIKSTR(BOOST_PP_TUPLE_ELEM(2,1,ele))
#define GET_CT_PM(ele,st) SPLIT_ELE(ele),sizeof(BOOST_PP_TUPLE_ELEM(2,0,ele)),GET_OFFSET(st,BOOST_PP_TUPLE_ELEM(2,1,ele))
#define GET_ELE(r,data,ele) EMP_IN_PPT(GET_CT_PM(ele,data))
#define GEN_V(r,data,elem) BOOST_PP_TUPLE_ELEM(2,0,elem) BOOST_PP_TUPLE_ELEM(2,1,elem);
#define DEF_STRUCT(stname,vname,...) MSC_PACK_HEAD \
 struct GNU_DEF stname {\
	BOOST_PP_SEQ_FOR_EACH(GEN_V,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	stname(){}\
	stname(vp_prop_ele& vprop_ele){sp_prop_ele ppt=make_shared<prop_ele>(this,sizeof(stname));\
	BOOST_PP_SEQ_FOR_EACH(GET_ELE,stname,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	vprop_ele.emplace_back(ppt);\
    }\
};\
stname vname{_vprop_eles}; MSC_PACK_END

#define GEN_V3(r,data,elem) BOOST_PP_TUPLE_ELEM(3,0,elem) BOOST_PP_TUPLE_ELEM(3,1,elem)BOOST_PP_TUPLE_ELEM(3,2,elem);
#define SPLIT_ELE3(ele) PIKSTR(BOOST_PP_TUPLE_ELEM(3,0,ele)),PIKSTR(BOOST_PP_TUPLE_ELEM(3,1,ele))
#define GET_CT_PM3(ele,st) SPLIT_ELE3(ele),sizeof(BOOST_PP_TUPLE_ELEM(3,0,ele)),GET_OFFSET(st,BOOST_PP_TUPLE_ELEM(3,1,ele))
#define GET_ELE3(r,data,ele) EMP_IN_PPT(GET_CT_PM3(ele,data))
#define DEF_STRUCT_WITH_INIT(stname,vname,...) MSC_PACK_HEAD \
 struct GNU_DEF stname {\
	BOOST_PP_SEQ_FOR_EACH(GEN_V3,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	stname(){}\
	stname(vp_prop_ele& vprop_ele){sp_prop_ele ppt=make_shared<prop_ele>(this,sizeof(stname));\
	BOOST_PP_SEQ_FOR_EACH(GET_ELE3,stname,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	vprop_ele.emplace_back(ppt);\
    }\
};\
stname vname{_vprop_eles}; MSC_PACK_END

/************************************************************************/
/* 
使用DEF_STRUCT来定义结构体注意事项：
1、数组成员不能直接初始化。例如：char a[20]{1,2,3}; //error C2536。
2、成员变量不支持直接初始化。例如 class1 my_class{p1,p2,p3}
3、基本数据类型的成员都有默认的属性操作，客户类型的成员属性操作,方法是注册回调函数，并提供类型名称。
4、用户可以针对特定的结构体变量提供属性操作，方法是注册回调函数，同时提供结构体变量的地址。
5、用户可以针对特定的结构体成员变量提供属性操作，方法是注册回调函数，同时提供结构体变量的地址和结构体成员变量在该结构中的位置索引。
*/
/************************************************************************/