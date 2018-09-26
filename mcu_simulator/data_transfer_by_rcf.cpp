#include "stdafx.h"
#include "data_transfer_by_rcf.h"

#include <iostream>
#include "imcu_service.h"

static RCF::RcfInitDeinit _rcfInit;
static RcfClient<I_DATACOLLECT>* _pclient=NULL;

static RCF::RcfServer* pPubServer = NULL;
static RCF::SubscriptionPtr subscriptionPtr;


class data_dispatch
{
public:
	data_dispatch(){}
	~data_dispatch(){}
	void dispatch_vdata_to_client(vector<unsigned char>& vdata)
	{
		std::cout << "got datas:";
		for (auto itt : vdata)
		{
			printf("%x,", itt);
		}
		cout << endl;
	}
};
data_dispatch _dd;
RCF::SubscriptionParms subParms;
RCF::SubscriptionPtr _psub;
data_transfer_by_rcf::data_transfer_by_rcf()
{
	assert(_pclient == NULL);
	try
	{
		_pclient = new RcfClient<I_DATACOLLECT>(RCF::TcpEndpoint(50001));
		_pclient->getClientStub().setConnectTimeoutMs(20 * 1000);
		_pclient->getClientStub().setRemoteCallTimeoutMs(600 * 1000);
	    
		pPubServer = new RCF::RcfServer(RCF::TcpEndpoint(-1));
		pPubServer->start();
		subParms.setPublisherEndpoint(RCF::TcpEndpoint("127.0.0.2",50002));
		_psub=pPubServer->createSubscription<I_DATADISPATCH>(*this, subParms);
		//pPubServer->createSubscription<I_DATADISPATCH>(*this, subParms);
		//subscriptionPtr =pPubServer->createPublisher<I_DATADISPATCH>(*this, subParms);
	}
	catch (const RCF::Exception & e)
	{
		std::cout << "Error: " << e.getErrorString() << std::endl;
	}
}


data_transfer_by_rcf::~data_transfer_by_rcf()
{
	delete _pclient;
}

bool data_transfer_by_rcf::send_data_to_host(std::vector<unsigned char>&vdata)
{
	try
	{
		_pclient->send_vdata_2_host(vdata);
	}
	catch (const RCF::Exception & e)
	{
		std::cout << "Error: " << e.getErrorString() << std::endl;
		return false;
	}
	return true;
}
void data_transfer_by_rcf::dispatch_vdata_to_client(std::vector<unsigned char>&vdata)
{
	printf("got data from host\n");

	if (_handle_vadata)
	{
		_handle_vadata(vdata);
	}
}
