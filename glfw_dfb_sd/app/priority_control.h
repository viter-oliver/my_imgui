#pragma once
#include <chrono>
#include <functional>
#include <vector>
#include <memory>
#include <deque>
namespace auto_future
{
	using namespace std;
	enum en_sg_statatus
	{
		en_sg_missing,
		en_sg_queuing,
		en_sg_on,
	};
	struct singal_priority_unit
	{
		bool _visible{false},be_preempted{false};
		virtual bool handle(bool,int)=0;//function<bool(bool,int)> handle;
		virtual en_sg_statatus sig_status(int)=0; //function<en_sg_statatus(int)> sig_status;
		virtual void queue(int){}
		virtual bool flash(bool,int)=0;
	};
	using sd_sg_un=shared_ptr<singal_priority_unit>;
	using vsg_pt_uint=vector<sd_sg_un>;
	class signal_priority_control
	{
		vsg_pt_uint&  _psignal_control_unit_list;
		public:
			signal_priority_control(vsg_pt_uint& su_list):_psignal_control_unit_list(su_list){}
		bool try_signal_on(int mid,bool be_visible,int sub_id=0);
		bool flash_signal(int mid,bool be_visible,int sub_id=0);
		en_sg_statatus signal_status(int mid,int sub_id=0);
	};

	struct signal_sub_unit
	{
		bool _on;
		function<void(bool)> _handle_sub_sig;
	};
	using vsub_signal_unit=vector<signal_sub_unit>;
    struct signal_unit_deque:public singal_priority_unit
    {
		vsub_signal_unit& _sub_sig_list;
		deque<int> _deque;
		signal_unit_deque(vsub_signal_unit& sub_sif_list):_sub_sig_list(sub_sif_list){}
		bool handle(bool on,int sid);
		en_sg_statatus sig_status(int sid);
		void queue(int sid);
		bool flash(bool visible,int sid);
    };
    struct signal_unit_coexist:public singal_priority_unit
    {
		vsub_signal_unit& _sub_sig_list;
		signal_unit_coexist(vsub_signal_unit& sub_sif_list):_sub_sig_list(sub_sif_list){}
		bool handle(bool on,int sid);
		en_sg_statatus sig_status(int sid);
	    bool flash(bool visible,int sid);
    };

}

