#include "python_interpreter.h"
#include "af_type.h"
#ifdef with_python
#include <python2.7/Python.h>
static PyObject *_pModule, *_pGlobal, *_pLocal;
#endif
const string python_pre_define = R"(
class af_vec2:
  def __init__(self,x,y):
    self.x=x
    self.y=y
  def __add__(self,other):
    return af_vec2(self.x+other.x,self.y+other.y)
  def __sub__(self,other):
    return af_vec2(self.x-other.x,self.y-other.y)
  def __mul__(self,tm):
    return af_vec2(self.x*tm,self.y*tm)
  def norm(self):
    nvalue=self.x*self.x+self.y*self.y
    rtvalue=nvalue ** 0.5
    return rtvalue
class af_vec3:
  def __init__(self,x,y,z):
    self.x=x
    self.y=y
    self.z=z
  def __add__(self,other):
    return af_vec3(self.x+other.x,self.y+other.y,self.z+other.z)
  def __sub__(self,other):
    return af_vec3(self.x-other.x,self.y-other.y,self.z-other,z)
  def __mul__(self,tm):
    return af_vec3(self.x*tm,self.y*tm,self.z*tm)
  def norm(self):
    nvalue=self.x*self.x+self.y*self.y+self.z*self.z
    rtvalue=nvalue ** 0.5
    return rtvalue
class af_vec4:
  def __init__(self,x,y,z,w):
    self.x=x
    self.y=y
    self.z=z
  def __add__(self,other):
    return af_vec4(self.x+other.x,self.y+other.y,self.z+other.z,self.w+other.w)
  def __sub__(self,other):
    return af_vec4(self.x-other.x,self.y-other.y,self.z-other,z,self.w-other.w)
  def __mul__(self,tm):
    return af_vec4(self.x*tm,self.y*tm,self.z*tm,self.w*tm)
  def norm(self):
    nvalue=self.x*self.x+self.y*self.y+self.z*self.z+self.w*self.w
    rtvalue=nvalue ** 0.5
    return rtvalue
)";
const string python_fun_head = "def bind_fun(";
const string python_fun_name = "bind_fun";
using namespace zl_future;
python_interpreter g_python_intp;
python_interpreter::python_interpreter()
{
  #ifdef with_python
	Py_Initialize();
	_pGlobal = PyDict_New();
	_pModule = PyModule_New("_afg_mod_");
	PyModule_AddStringConstant(_pModule, "__file__", "");
	_pLocal = PyModule_GetDict(_pModule);
  #endif

}


python_interpreter::~python_interpreter()
{
  #ifdef with_python
	Py_Finalize();
  #endif
}

bool python_interpreter::call_python_fun(string& fun_exp, string fun_name, var_unit& fun_retn, variable_list& vlist)
{
  #ifdef with_python
	do 
	{
		PyObject *pArgs, *pValue,*pFunc;
		pValue = PyRun_String(fun_exp.c_str(), Py_file_input, _pGlobal, _pLocal);
		if (!pValue)
		{

			break;
		}
		Py_DECREF(pValue);
		pFunc = PyObject_GetAttrString(_pModule, fun_name.c_str());
		if (!pFunc || !PyCallable_Check(pFunc))
		{
			break;
		}
		auto vsz = vlist.size();
		pArgs = PyTuple_New(vsz);
		for (int ix = 0; ix < vsz;++ix)
		{
			auto& parm = vlist[ix];
			PyObject* pyPm = NULL;
			if (parm._type == "int" ){
				int* pivalue = (int*)parm._value_addr;
				PyTuple_SetItem(pArgs, ix, Py_BuildValue("i", *pivalue));
			}
			else if(parm._type == "float"){
				float* pfvalue = (float*)parm._value_addr;
				PyTuple_SetItem(pArgs, ix, Py_BuildValue("f", *pfvalue));
			}
			else if (parm._type == "double"){
				double* pdvalue = (double*)parm._value_addr;
				PyTuple_SetItem(pArgs, ix, Py_BuildValue("d", *pdvalue));
			}
			else if (parm._type=="bool")
			{
				bool* pbvalue = (bool*)parm._value_addr;
				PyTuple_SetItem(pArgs, ix, Py_BuildValue("b", *pbvalue));

			}
			else if(parm._type == "char"){
				char* pcvalue = (char*)parm._value_addr;
				string svalue = pcvalue;
				PyTuple_SetItem(pArgs, ix, Py_BuildValue("s", svalue));
			}
      else if (parm._type=="af_vi2")
      {
          af_vi2* pv2 = (af_vi2*)parm._value_addr;
          PyTuple_SetItem( pArgs, ix, Py_BuildValue( "[i,i]", pv2->x,pv2->y ) );
      }
      else if( parm._type == "af_vi3" )
      {
          af_vi3* pv3 = (af_vi3*)parm._value_addr;
          PyTuple_SetItem( pArgs, ix, Py_BuildValue( "[i,i,i]", pv3->x,pv3->y,pv3->z) );
      }               
      else if( parm._type == "af_vi4" )
      {
          af_vi4* pv4 = (af_vi4*)parm._value_addr;
          PyTuple_SetItem( pArgs, ix, Py_BuildValue( "[i,i,i,i]", pv4->x, pv4->y, pv4->z, pv4->w ) );
      }
      else if( parm._type == "af_vec2" )
      {
          af_vec2* pv2 = (af_vec2*)parm._value_addr;
          PyTuple_SetItem( pArgs, ix, Py_BuildValue( "[f,f]", pv2->x, pv2->y ) );
      }
      else if( parm._type == "af_vec3" )
      {
          af_vec3* pv3 = (af_vec3*)parm._value_addr;
          PyTuple_SetItem( pArgs, ix, Py_BuildValue( "[f,f,f]", pv3->x,pv3->y,pv3->z) );
      }
      else if( parm._type == "af_vec4" )
      {
          af_vec4* pv4 = (af_vec4*)parm._value_addr;
          PyTuple_SetItem( pArgs, ix, Py_BuildValue( "[f,f,f,f]", pv4->x,pv4->y,pv4->z,pv4->w) );
      }
		}
		PyObject* pRtn = PyObject_CallObject(pFunc, pArgs);
		if (fun_retn._type=="int")
		{		
			PyArg_Parse(pRtn, "i", (int*)fun_retn._value_addr);
		}
		else if (fun_retn._type == "float")
		{
			PyArg_Parse(pRtn, "f", (float*)fun_retn._value_addr);
		}
		else if (fun_retn._type == "double")
		{
			PyArg_Parse(pRtn, "d", (double*)fun_retn._value_addr);
		}
		else if (fun_retn._type == "bool")
		{
			PyArg_Parse(pRtn, "b", (bool*)fun_retn._value_addr);	 
		}
		else if (fun_retn._type == "char")
		{
			PyArg_Parse(pRtn, "s", (string*)fun_retn._value_addr);
		}
          else if(fun_retn._type=="af_vi2" )
          {
               af_vi2 tv2;
               PyArg_ParseTuple( pRtn, "ii", &tv2.x, &tv2.y );
               memcpy( fun_retn._value_addr, &tv2, sizeof( af_vi2 ) );
          }
          else if( fun_retn._type == "af_vi3" )
          {
              af_vi3 tv3;
              PyArg_ParseTuple( pRtn, "iii", &tv3.x, &tv3.y, &tv3.z );
              memcpy( fun_retn._value_addr, &tv3, sizeof( af_vi3 ) );

          }
          else if( fun_retn._type == "af_vi4" )
          {
               af_vi4 tv4;
               PyArg_ParseTuple( pRtn, "iiii", &tv4.x, &tv4.y, &tv4.z, &tv4.w );
               memcpy( fun_retn._value_addr, &tv4, sizeof( af_vi4 ) );
          }
          else if( fun_retn._type == "af_vec2" )
          {
               af_vec2 tv2;
               PyArg_ParseTuple( pRtn, "ff", &tv2.x, &tv2.y );
               memcpy( fun_retn._value_addr, &tv2, sizeof( af_vec2 ) );
          }
          else if( fun_retn._type == "af_vec3" )
          {
               af_vec3 tv3;
               PyArg_ParseTuple( pRtn, "fff", &tv3.x, &tv3.y, &tv3.z );
               memcpy( fun_retn._value_addr, &tv3, sizeof( af_vec3 ) );

          }
          else if( fun_retn._type == "af_vec4" )
          {
               af_vec4 tv4;
               PyArg_ParseTuple( pRtn, "ffff", &tv4.x, &tv4.y, &tv4.z, &tv4.w );
               memcpy( fun_retn._value_addr, &tv4, sizeof( af_vec4 ) );
          }
		Py_XDECREF(pFunc);
          Py_CLEAR( pArgs );

	} while (0);

	if (PyErr_Occurred())
	{
		PyErr_Print();
		return false;
	}
  #endif
	return true;
	
}
