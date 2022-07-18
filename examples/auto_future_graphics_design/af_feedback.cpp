#include "af_feedback.h"

af_feedback::af_feedback(ps_mtl& pmtl, ps_primrive_object& pprm)
	:_pmtl(pmtl), _pprm(pprm)
{
	glGenBuffers(1, &_gpuOutputBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _gpuOutputBuffer);
	//auto stride = pprm->get_stride();
	auto buff_sz = pprm->_vertex_buf_len;// stride;
	glBufferData(GL_ARRAY_BUFFER, buff_sz*sizeof(GLfloat), nullptr, GL_STATIC_READ);
	//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _gpuOutputBuffer);
}

af_feedback::~af_feedback()
{
	_pmtl = nullptr;
	_pprm = nullptr;
}

bool af_feedback::get_output_vertex(vector<float>& overtex)
{
	overtex.resize(_pprm->_vertex_buf_len);
	auto bfsz= _pprm->_vertex_buf_len*sizeof(GLfloat);
	#ifdef GLFW_INCLUDE_ES3
	float* pbuf=(float*)glMapBufferRange(GL_ARRAY_BUFFER,0,bfsz,GL_MAP_READ_BIT);
	memcpy(&overtex[0],pbuf,bfsz);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	#else
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, bfsz, &overtex[0]);
	#endif
	//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	return true;
}

void af_feedback::draw()
{
	_pmtl->use();
	GLboolean last_enable_discard = glIsEnabled(GL_RASTERIZER_DISCARD);
	glEnable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _gpuOutputBuffer);
	glBindVertexArray(_pprm->_vao);
	_pprm->enableVertex();
	glBeginTransformFeedback(GL_POINTS);
	auto stride = _pprm->get_stride();
	auto vcnt = _pprm->_vertex_buf_len / stride;
	glDrawArrays(GL_POINTS, 0, vcnt);
	//glDrawTransformFeedback(GL_POINTS,)
	glEndTransformFeedback();
	
	if (!last_enable_discard)
	{
		glDisable(GL_RASTERIZER_DISCARD);
	}
	glFlush();
	
}
mp_feed_back g_feedback_list;
