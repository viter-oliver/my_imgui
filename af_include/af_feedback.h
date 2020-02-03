#pragma once
#include "af_material.h"
#include "af_primitive_object.h"
class af_feedback
{
     ps_mtl _pmtl;
     ps_primrive_object _pprm;
     GLuint _gpuOutputBuffer;
public:
	af_feedback(ps_mtl& pmtl, ps_primrive_object& pprm);
     ~af_feedback();
     bool try_bind_outputbuff();
     void draw();
};

