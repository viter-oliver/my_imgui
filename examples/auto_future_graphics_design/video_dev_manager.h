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
#include <wmcodecdsp.h>

#include "ft_video.h"
#define CHECK_HR(hr, msg) if (hr != S_OK) { printf(msg); printf(" Error: %.2X.\n", hr); throw ""; }

using namespace std;
using namespace chrono;

using mp_video_ctl = map<ft_video*, steady_clock::time_point>;

class video_dev_unit;
using sdev_unit = shared_ptr<video_dev_unit>;
using mp_dev_unit = map<string, sdev_unit>;



class video_dev_unit
{
     mutex _mutex;
     wstring _link_str;
     UINT32 _width, _height;
     atomic<bool> _frame_valid { false };
     condition_variable _cond;
     IMFSourceReader* _pReader = NULL;
     IMFMediaType* _videoSourceOutputType = NULL;
     BYTE* pVideoframe;
     DWORD dwCurrentLenth = 0, dwMaxlenth = 0;
     bool _be_pulling_frame = false;
     mp_video_ctl _video_ctl_list;
     bool _initialized = false;
public:
     video_dev_unit( wchar_t* plink, IMFSourceReader* pSource ) :_link_str( plink ), _pReader( pSource )
     {
          try
          {
               CHECK_HR( _pReader->GetCurrentMediaType( (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &_videoSourceOutputType ),
                         "Error retrieving current media type from first video stream." );
               CHECK_HR( MFGetAttributeSize(_videoSourceOutputType, MF_MT_FRAME_SIZE, &_width, &_height ), "faile to get video frame size." );
               CHECK_HR( _pReader->SetStreamSelection( (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE ),
                         "Failed to set the first video stream on the source reader." );

               _initialized = true;
          }
          catch( ... )
          {
               printf( "fail to initialize video unit\n" );
          }
          

         // _pSource->AddRef();
     }
     ~video_dev_unit();
     bool initialized()
     {
          return _initialized;
     }
     void delink_cltrl_list()
     {
          for( auto&ictlv : _video_ctl_list )
          {
               ictlv.first->delink();
          }
     }
     bool unlink_self( wchar_t* unlink_dev_name )
     {
          if( wcsncmp( _link_str.c_str(), unlink_dev_name, 25 ) == 0 )
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
                    ictl.first->update_pixels( pVideoframe, dwCurrentLenth );
               }
               _frame_valid = false;
               _cond.notify_one();
          }
     }
     void start_pulling_data();
     void start_pulling_data_with_mtf();
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
			if (ImGui::Button("Start pulling video frame"))
			{
				//start_pulling_data();
                    start_pulling_data_with_mtf();
			}
		}
     }
};
extern mp_dev_unit g_map_dev_units;

extern HRESULT EnumDeviceVideoDevices();