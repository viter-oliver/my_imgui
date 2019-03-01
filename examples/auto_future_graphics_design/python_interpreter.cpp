#include "python_interpreter.h"
const string python_fun_head = "def bind_fun(";
const string python_fun_name = "bind_fun";
python_interpreter g_python_intp;
python_interpreter::python_interpreter()
{
	Py_Initialize();
	_pGlobal = PyDict_New();
	_pModule = PyModule_New("_afg_mod_");
	PyModule_AddStringConstant(_pModule, "__file__", "");
	_pLocal = PyModule_GetDict(_pModule);

}


python_interpreter::~python_interpreter()
{
	Py_Finalize();
}

bool python_interpreter::call_python_fun(string& fun_exp, string fun_name, var_unit& fun_retn, variable_list& vlist)
{
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
		Py_XDECREF(pFunc);

	} while (0);

	if (PyErr_Occurred())
	{
		PyErr_Print();
		return false;
	}
	return true;
	
}
