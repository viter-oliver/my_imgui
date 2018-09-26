
#include "idata_collect.h"
#include "imcu_service.h"


static RCF::RcfInitDeinit rcfInit;
static RCF::RcfServer* pserver = NULL;
idata_collect::idata_collect()
{
}


idata_collect::~idata_collect()
{
	pserver->unbind<I_DATACOLLECT>();
	delete pserver;
}

void idata_collect::link()
{
	assert(pserver == NULL);
	pserver = new RCF::RcfServer(RCF::TcpEndpoint(50001));
	pserver->bind<I_DATACOLLECT>(*this);
	pserver->start();

}

void idata_collect::send_vdata_2_host(std::vector<unsigned char>& vadata)
{
	if (_cmd_handle)
	{
		_cmd_handle(&*vadata.begin(), vadata.size());
	}
}
