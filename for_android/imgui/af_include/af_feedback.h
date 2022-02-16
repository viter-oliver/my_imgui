#pragma once
#include "af_material.h"
#include "af_primitive_object.h"
struct af_feedback
{
     ps_mtl _pmtl;
     ps_primrive_object _pprm;
     GLuint _gpuOutputBuffer;
#if !defined(DISABLE_DEMO)
	bool _sel{ false };
#endif
	af_feedback(ps_mtl& pmtl, ps_primrive_object& pprm);
     ~af_feedback();
     bool get_output_vertex(vector<float>& overtex);
     void draw();
};
struct feedback_key
{
	string _mtl_key;
	string _prm_key;
	bool operator <(const feedback_key& tkey) const
	{
		if (_mtl_key!=tkey._mtl_key)
		{
			return _mtl_key < tkey._mtl_key;
		}
		else
		{
			return _prm_key < tkey._prm_key;
		}
	}
};
using psfeed_back = shared_ptr<af_feedback>;
using mp_feed_back = map<feedback_key, psfeed_back>;
extern mp_feed_back g_feedback_list;
