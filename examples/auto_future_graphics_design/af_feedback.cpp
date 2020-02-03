#include "af_feedback.h"

af_feedback::af_feedback(ps_mtl& pmtl, ps_primrive_object& pprm)
	:_pmtl(pmtl), _pprm(pprm)
{
	glGenBuffers(1, &_gpuOutputBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _gpuOutputBuffer);
	//auto stride = pprm->get_stride();
	auto buff_sz = pprm->_vertex_buf_len;// stride;
	glBufferData(GL_ARRAY_BUFFER, buff_sz, nullptr, GL_STATIC_READ);

}

af_feedback::~af_feedback()
{
	_pmtl = nullptr;
	_pprm = nullptr;
}

bool af_feedback::try_bind_outputbuff()
{
	return true;
}

void af_feedback::draw()
{
	_pmtl->use();
	glEnable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _gpuOutputBuffer);
	_pprm->enableVertex();
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, _pprm->_vertex_buf_len);
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
}
