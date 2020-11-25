#pragma once
#include <memory>
#include<mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm>
#include<map>
#include <comdef.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include <mfreadwrite.h>
#include "ft_video.h"
using namespace std;
using namespace chrono;

using mp_video_ctl = map<ft_video*, steady_clock::time_point>;

class video_dev_unit;
using sdev_unit = shared_ptr<video_dev_unit>;
using mp_dev_unit = map<string, sdev_unit>;



class video_dev_unit
{
     mutex _mutex;
     UINT32 _width, _height;
     atomic<bool> _frame_valid { false };
     condition_variable _cond;
     IMFActivate* _pActivate = NULL;
     IMFMediaSource* _pSource = NULL;
     BYTE* pVideoframe;
     DWORD dwCurrentLenth = 0, dwMaxlenth = 0;
     bool _be_pulling_frame = false;
     mp_video_ctl _video_ctl_list;
public:
     video_dev_unit( IMFActivate* pActivate, IMFMediaSource* pSource ) :_pActivate( pActivate ), _pSource( pSource )
     {
          _pActivate->AddRef();
         // _pSource->AddRef();
     }
     ~video_dev_unit();
     void delink_cltrl_list()
     {
          for( auto&ictlv : _video_ctl_list )
          {
               ictlv.first->delink();
          }
     }
     bool unlink_self( wchar_t* unlink_dev_name )
     {
          wchar_t* psystem_link = NULL;
          HRESULT hr = _pActivate->GetAllocatedString( MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, \
                                                       &psystem_link, \
                                                       NULL );
          if( wcsncmp( psystem_link, unlink_dev_name, 25 ) == 0 )
          {
               return true;
          }
          return false;
     }
     UINT32& width()
     {
          return _width;
     }
     UINT32& height()
     {
          return _height;
     }
     void pick_frame_data()
     {
          if( _frame_valid )
          {
               //processing frame
               for( auto& ictl : _video_ctl_list )
               {
                    ictl.first->update_pixels( pVideoframe, dwCurrentLenth * 2 );
               }
               _frame_valid = false;
               _cond.notify_one();
          }
     }
     void start_pulling_data();
     bool add_video_ctrl( ft_video* pfv, int& vwidth, int& vheight )
     {
          auto& dic_f_video =_video_ctl_list;
          auto& ifv = dic_f_video.find( pfv );
          if( ifv == dic_f_video.end() )
          {
               dic_f_video[ pfv ] = steady_clock::now();
               vwidth = _width;
               vheight = _height;
               return true;
          }
          return false;
     }
     void view_edit()
     {
          ImGui::Text( "Frame width:%d",_width );
          ImGui::Text( "Frame height:%d",_height );
          ImGui::Text( "video control list:" );

          auto& ctl_list = _video_ctl_list;
		for (auto& ictl:ctl_list)
		{
			auto& pctl = ictl.first;
			string& cname= pctl->get_name();
			ImGui::Text(cname.c_str());
		}
		if (_be_pulling_frame)
		{
			if (ImGui::Button("Stop pulling video frame"))
			{
				_be_pulling_frame = false;
			}
		}
		else
		{
			if (ImGui::Button("Start pulling video frame") && ctl_list.size()>0)
			{
				start_pulling_data();
			}
		}
     }
};
extern mp_dev_unit g_map_dev_units;

extern HRESULT EnumDeviceVideoDevices();