#pragma once
#include <string>
#include <vector>
#include <memory>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

struct field_ele
{
	std::string _type;
	std::string _name;
	field_ele(std::string tp, std::string nm) :_type(tp), _name(nm){}
};
typedef std::shared_ptr<field_ele> sp_field_ele;
//
typedef std::vector<sp_field_ele> vt_field_ele;
struct  prop_ele
{
	void* _pro_address;
	vt_field_ele _pro_page;
	prop_ele(void* paddress) :_pro_address(paddress){}
};
typedef std::shared_ptr<prop_ele> sp_prop_ele;
typedef std::vector<sp_prop_ele> vp_prop_ele;
#define _PIKSTR(x) #x
#define PIKSTR(x) _PIKSTR(x)
#define EMP_IN_PPT(x)	ppt->_pro_page.emplace_back(make_shared<field_ele>(x));
#define SPLIT_ELE(ele) PIKSTR(BOOST_PP_TUPLE_ELEM(2,0,ele)),PIKSTR(BOOST_PP_TUPLE_ELEM(2,1,ele))
#define GET_ELE(r,data,ele) EMP_IN_PPT(SPLIT_ELE(ele))
#define GEN_V(r,data,elem) BOOST_PP_TUPLE_ELEM(2,0,elem) BOOST_PP_TUPLE_ELEM(2,1,elem);
#define DEF_STRUCT(stname,vname,...) struct stname {\
	BOOST_PP_SEQ_FOR_EACH(GEN_V,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	stname(vp_prop_ele& vprop_ele){sp_prop_ele ppt=make_shared<prop_ele>(this);\
	BOOST_PP_SEQ_FOR_EACH(GET_ELE,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	vprop_ele.emplace_back(ppt);\
    }\
};\
stname vname{_vprop_eles};
	