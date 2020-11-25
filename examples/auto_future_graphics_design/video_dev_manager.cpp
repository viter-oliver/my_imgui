#include "video_dev_manager.h"
#include "common_functions.h"

#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"mf.lib")
#pragma comment(lib,"mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")
template <class T> void SafeRelease( T **ppT )
{
     if( *ppT )
     {
          ( *ppT )->Release();
          *ppT = NULL;
     }
}
typedef HRESULT( *COPY_TRANSFORM_FN )( BYTE*, LONG, BYTE*, LONG, UINT, UINT );

struct VideoFormatDescription
{
     GUID guidSubType;
     LPCSTR pwszGuidSubType;
     COPY_TRANSFORM_FN FnCopyTransform;
};
VideoFormatDescription video_format_default = { MFVideoFormat_Base, "", NULL };
VideoFormatDescription FormatConversions[] = {
     /* Uncompressed RGB formats */
     { MFVideoFormat_RGB8, "MFVideoFormat_RGB8", NULL },
     { MFVideoFormat_RGB555, "MFVideoFormat_RGB555", NULL },
     { MFVideoFormat_RGB565, "MFVideoFormat_RGB565", NULL },
     { MFVideoFormat_RGB32, "MFVideoFormat_RGB32", NULL },
     { MFVideoFormat_RGB24, "MFVideoFormat_RGB24", NULL },
     { MFVideoFormat_ARGB32, "MFVideoFormat_ARGB32", NULL },
     /* YUV Formats: 8-Bit and Palettized */
     { MFVideoFormat_AI44, "MFVideoFormat_AI44", NULL },
     { MFVideoFormat_AYUV, "MFVideoFormat_AYUV", NULL },
     { MFVideoFormat_I420, "MFVideoFormat_I420", NULL },
     { MFVideoFormat_IYUV, "MFVideoFormat_IYUV", NULL },
     { MFVideoFormat_NV11, "MFVideoFormat_NV11", NULL },
     { MFVideoFormat_NV12, "MFVideoFormat_NV12", NULL },
     { MFVideoFormat_UYVY, "MFVideoFormat_UYVY", NULL },
     { MFVideoFormat_Y41P, "MFVideoFormat_Y41P", NULL },
     { MFVideoFormat_Y41T, "MFVideoFormat_Y41T", NULL },
     { MFVideoFormat_Y42T, "MFVideoFormat_Y42T", NULL },
     { MFVideoFormat_YUY2, "MFVideoFormat_YUY2", NULL /*FnCopyYUY2ToRGB32*/ },
     { MFVideoFormat_YV12, "MFVideoFormat_YV12", NULL },
     /* YUV Formats: 10-Bit and 16-Bit */
     { MFVideoFormat_P010, "MFVideoFormat_P010", NULL },
     { MFVideoFormat_P016, "MFVideoFormat_P016", NULL },
     { MFVideoFormat_P210, "MFVideoFormat_P210", NULL },
     { MFVideoFormat_P216, "MFVideoFormat_P216", NULL },
     { MFVideoFormat_v210, "MFVideoFormat_v210", NULL },
     { MFVideoFormat_v216, "MFVideoFormat_v216", NULL },
     { MFVideoFormat_v410, "MFVideoFormat_v410", NULL },
     { MFVideoFormat_Y210, "MFVideoFormat_Y210", NULL },
     { MFVideoFormat_Y216, "MFVideoFormat_Y216", NULL },
     { MFVideoFormat_Y410, "MFVideoFormat_Y410", NULL },
     { MFVideoFormat_Y416, "MFVideoFormat_Y416", NULL },
     /* Encoded Video Types */
     { MFVideoFormat_DV25, "MFVideoFormat_DV25", NULL },
     { MFVideoFormat_DV50, "MFVideoFormat_DV50", NULL },
     { MFVideoFormat_DVC, "MFVideoFormat_DVC", NULL },
     { MFVideoFormat_DVH1, "MFVideoFormat_DVH1", NULL },
     { MFVideoFormat_DVHD, "MFVideoFormat_DVHD", NULL },
     { MFVideoFormat_DVSD, "MFVideoFormat_DVSD", NULL },
     { MFVideoFormat_DVSL, "MFVideoFormat_DVSL", NULL },
     { MFVideoFormat_H264, "MFVideoFormat_H264", NULL },
     { MFVideoFormat_M4S2, "MFVideoFormat_M4S2", NULL },
     { MFVideoFormat_MJPG, "MFVideoFormat_MJPG", NULL },
     { MFVideoFormat_MP43, "MFVideoFormat_MP43", NULL },
     { MFVideoFormat_MP4S, "MFVideoFormat_MP4S", NULL },
     { MFVideoFormat_MP4V, "MFVideoFormat_MP4V", NULL },
     { MFVideoFormat_MPEG2, "MFVideoFormat_MPEG2", NULL },
     { MFVideoFormat_MPG1, "MFVideoFormat_MPG1", NULL },
     { MFVideoFormat_MSS1, "MFVideoFormat_MSS1", NULL },
     { MFVideoFormat_MSS2, "MFVideoFormat_MSS2", NULL },
     { MFVideoFormat_WMV1, "MFVideoFormat_WMV1", NULL },
     { MFVideoFormat_WMV2, "MFVideoFormat_WMV2", NULL },
     { MFVideoFormat_WMV3, "MFVideoFormat_WMV3", NULL },
     { MFVideoFormat_WVC1, "MFVideoFormat_WVC1", NULL },

};
VideoFormatDescription& get_video_description( GUID& tar )
{
     auto fmt_cnt = sizeof FormatConversions / sizeof VideoFormatDescription;
     for( int id = 0; id < fmt_cnt; ++id )
     {
          if( tar == FormatConversions[ id ].guidSubType )
          {
               return FormatConversions[ id ];
          }
     }
     return video_format_default;
}


HRESULT GetVideoFrameSize( IMFMediaSource *pSource, UINT32& width, UINT32& height )
{
     IMFPresentationDescriptor *pPD = NULL;
     IMFStreamDescriptor *pSD = NULL;
     IMFMediaTypeHandler *pHandler = NULL;
     IMFMediaType *pType = NULL;

     HRESULT hr = pSource->CreatePresentationDescriptor( &pPD );
     if( FAILED( hr ) )
     {
          goto done;
     }

     BOOL fSelected;
     hr = pPD->GetStreamDescriptorByIndex( 0, &fSelected, &pSD );
     if( FAILED( hr ) )
     {
          goto done;
     }

     hr = pSD->GetMediaTypeHandler( &pHandler );
     if( FAILED( hr ) )
     {
          goto done;
     }

     DWORD cTypes = 0;
     hr = pHandler->GetMediaTypeCount( &cTypes );
     if( FAILED( hr ) )
     {
          goto done;
     }

     for( DWORD i = 0; i < cTypes; i++ )
     {
          hr = pHandler->GetMediaTypeByIndex( i, &pType );
          if( FAILED( hr ) )
          {
               goto done;
          }
          GUID majType = { 0 };
          hr = pType->GetGUID( MF_MT_MAJOR_TYPE, &majType );
          if( SUCCEEDED( hr ) && majType == MFMediaType_Video )
          {
               hr = MFGetAttributeSize( pType, MF_MT_FRAME_SIZE, &width, &height );
               break;
          }
          //LogMediaType(pType);
          //OutputDebugString(L"\n");

          SafeRelease( &pType );
     }

done:
     SafeRelease( &pPD );
     SafeRelease( &pSD );
     SafeRelease( &pHandler );
     SafeRelease( &pType );
     return hr;
}
video_dev_unit::~video_dev_unit()
{
 
     SafeRelease( &_pActivate );
     if( _pSource )
     {
          //_pSource->Shutdown();
     }
     SafeRelease( &_pSource );
}

void video_dev_unit::start_pulling_data()
{
     assert( _pSource != NULL );
     thread td_pulling_data( [&]
     {
          HRESULT hr;
          IMFAttributes *pAttributes = NULL;
          IMFSourceReader *pReader;
          IMFMediaType *pConfigureType = NULL;
          pVideoframe = new BYTE[ _width*_height * 4 ];
          do
          {
               hr = MFCreateAttributes( &pAttributes, 1 );
               if( FAILED( hr ) )
               {
                    printf( "fail to MFCreateAttributes!\n" );
                    break;
               }
               hr = pAttributes->SetUINT32( MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE );
               hr = MFCreateSourceReaderFromMediaSource( _pSource, pAttributes, &pReader );
               //hr = MFCreateSourceReaderFromMediaSource(_pSource, NULL, &pReader);

               if( FAILED( hr ) )
               {
                    printf( "fail to MFCreateSourceReaderFromMediaSource\n" );
                    break;
               }
               DWORD dwStream = MF_SOURCE_READER_FIRST_VIDEO_STREAM;
               for( DWORD dwMediaTypeIndex = 0;; dwMediaTypeIndex++ )
               {
                    IMFMediaType *pType = NULL;
                    hr = pReader->GetNativeMediaType( dwStream, dwMediaTypeIndex, &pType );
                    if( hr == MF_E_NO_MORE_TYPES )
                    {
                         break;
                    }
                    else
                         if( SUCCEEDED( hr ) )
                         {
                              GUID subType = { 0 };
                              hr = pType->GetGUID( MF_MT_SUBTYPE, &subType );
                              auto& video_des = get_video_description( subType );
                              printf( "video foramt:%s\n", video_des.pwszGuidSubType );
                              pType->Release();
                         }

               }

               _be_pulling_frame = true;
               IMFSample *pSample = NULL;
               while( _be_pulling_frame )
               {
                    DWORD streamIndex, flags;
                    LONGLONG llTimeStamp;
                    hr = pReader->ReadSample(
                         dwStream,    // Stream index.
                         0,                              // Flags.
                         &streamIndex,                   // Receives the actual stream index. 
                         &flags,                         // Receives status flags.
                         &llTimeStamp,                   // Receives the time stamp.
                         &pSample                        // Receives the sample or NULL.
                         );

                    if( FAILED( hr ) )
                    {
                         break;
                    }

                    if( flags & MF_SOURCE_READERF_ENDOFSTREAM )
                    {
                         printf( "\tEnd of stream\n" );
                         _be_pulling_frame = false;
                    }
                    if( flags & MF_SOURCE_READERF_NEWSTREAM || !flags )
                    {
                         //printf("\tNew stream\n");

                         //printf("Stream %d (%I64d)\n", streamIndex, llTimeStamp);
                         DWORD dwBuffCount = 0;
                         hr = pSample->GetBufferCount( &dwBuffCount );

                         if( dwBuffCount>0 )
                         {
                              LONGLONG phduration = 0;
                              hr = pSample->GetSampleDuration( &phduration );
                              if( SUCCEEDED( hr ) )
                              {
                              }
                              IMFMediaBuffer* pMediaBuffer;
                              hr = pSample->ConvertToContiguousBuffer( &pMediaBuffer );

                              //hr = pMediaBuffer->GetCurrentLength(&dwBufferSize);
                              BYTE* pData = NULL;
                              hr = pMediaBuffer->Lock( &pData, &dwMaxlenth, &dwCurrentLenth );
                              /*BYTE* paph = pVideoframe + 3;
                              auto pxcnt = dwCurrentLenth / 4;
                              for (int ix = 0; ix < pxcnt;ix++)
                              {
                              *paph = 0xff;
                              paph += 4;
                              }
                              */
                              FromYUY2ToRGB32( pVideoframe, pData, _width, _height );

                              pMediaBuffer->Unlock();
                              pMediaBuffer->Release();
                              unique_lock<mutex> lck( _mutex );
                              _frame_valid = true;
                              _cond.wait( lck );
                         }
                    }
                    if( flags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED )
                    {
                         printf( "\tNative type changed\n" );
                    }
                    if( flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED )
                    {
                         printf( "\tCurrent type changed\n" );
                    }
                    if( flags & MF_SOURCE_READERF_STREAMTICK )
                    {
                         printf( "\tStream tick\n" );
                    }

                    SafeRelease( &pSample );
               }

          }
          while( false );

          delete pVideoframe;
          SafeRelease( &pConfigureType );
          SafeRelease( &pAttributes );
          SafeRelease( &pReader );

     } );
     td_pulling_data.detach();
}
#if 0

BOOL IsTargetMediaType( IMFMediaType *pType )
{
     GUID subType = { 0 };
     HRESULT hr = pType->GetGUID( MF_MT_SUBTYPE, &subType );
     //StringFromCLSID
     return TRUE;
}
HRESULT EnumerateCaptureFormats( IMFMediaSource *pSource )
{
     IMFPresentationDescriptor *pPD = NULL;
     IMFStreamDescriptor *pSD = NULL;
     IMFMediaTypeHandler *pHandler = NULL;
     IMFMediaType *pType = NULL;

     HRESULT hr = pSource->CreatePresentationDescriptor( &pPD );
     if( FAILED( hr ) )
     {
          goto done;
     }

     BOOL fSelected;
     hr = pPD->GetStreamDescriptorByIndex( 0, &fSelected, &pSD );
     if( FAILED( hr ) )
     {
          goto done;
     }

     hr = pSD->GetMediaTypeHandler( &pHandler );
     if( FAILED( hr ) )
     {
          goto done;
     }

     DWORD cTypes = 0;
     hr = pHandler->GetMediaTypeCount( &cTypes );
     if( FAILED( hr ) )
     {
          goto done;
     }

     for( DWORD i = 0; i < cTypes; i++ )
     {
          hr = pHandler->GetMediaTypeByIndex( i, &pType );
          if( FAILED( hr ) )
          {
               goto done;
          }
          hr = pHandler->SetCurrentMediaType( pType );
          if( FAILED( hr ) )
          {
               goto done;
          }
          //LogMediaType(pType);
          //OutputDebugString(L"\n");

          SafeRelease( &pType );
     }

done:
     SafeRelease( &pPD );
     SafeRelease( &pSD );
     SafeRelease( &pHandler );
     SafeRelease( &pType );
     return hr;
}
HRESULT SetDeviceFormat( IMFMediaSource *pSource, DWORD dwFormatIndex )
{
     IMFPresentationDescriptor *pPD = NULL;
     IMFStreamDescriptor *pSD = NULL;
     IMFMediaTypeHandler *pHandler = NULL;
     IMFMediaType *pType = NULL;

     HRESULT hr = pSource->CreatePresentationDescriptor( &pPD );
     if( FAILED( hr ) )
     {
          goto done;
     }

     BOOL fSelected;
     hr = pPD->GetStreamDescriptorByIndex( 0, &fSelected, &pSD );
     if( FAILED( hr ) )
     {
          goto done;
     }

     hr = pSD->GetMediaTypeHandler( &pHandler );
     if( FAILED( hr ) )
     {
          goto done;
     }

     hr = pHandler->GetMediaTypeByIndex( dwFormatIndex, &pType );
     if( FAILED( hr ) )
     {
          goto done;
     }

     hr = pHandler->SetCurrentMediaType( pType );

done:
     SafeRelease( &pPD );
     SafeRelease( &pSD );
     SafeRelease( &pHandler );
     SafeRelease( &pType );
     return hr;
}

class SourceReaderCB : public IMFSourceReaderCallback
{
     long                m_nRefCount;        // Reference count.
     CRITICAL_SECTION    m_critsec;
     HANDLE              m_hEvent;
     BOOL                m_bEOS;
     HRESULT             m_hrStatus;
public:
     SourceReaderCB( HANDLE hEvent ) :
          m_nRefCount( 1 ), m_hEvent( hEvent ), m_bEOS( FALSE ), m_hrStatus( S_OK )
     {
          InitializeCriticalSection( &m_critsec );
     }

     // IUnknown methods
     STDMETHODIMP QueryInterface( REFIID iid, void** ppv )
     {
          static const QITAB qit[] =
          {
               QITABENT( SourceReaderCB, IMFSourceReaderCallback ),
               { 0 },
          };
          return QISearch( this, qit, iid, ppv );
     }
     STDMETHODIMP_( ULONG ) AddRef()
     {
          return InterlockedIncrement( &m_nRefCount );
     }
     STDMETHODIMP_( ULONG ) Release()
     {
          ULONG uCount = InterlockedDecrement( &m_nRefCount );
          if( uCount == 0 )
          {
               delete this;
          }
          return uCount;
     }

     // IMFSourceReaderCallback methods
     STDMETHODIMP OnReadSample( HRESULT hrStatus, DWORD dwStreamIndex,
                                DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample )
     {
          EnterCriticalSection( &m_critsec );

          if( SUCCEEDED( hrStatus ) )
          {
               if( pSample )
               {
                    // Do something with the sample.
                    wprintf( L"Frame @ %I64d\n", llTimestamp );
               }
          }
          else
          {
               // Streaming error.
               NotifyError( hrStatus );
          }

          if( MF_SOURCE_READERF_ENDOFSTREAM & dwStreamFlags )
          {
               // Reached the end of the stream.
               m_bEOS = TRUE;
          }
          m_hrStatus = hrStatus;

          LeaveCriticalSection( &m_critsec );
          SetEvent( m_hEvent );
          return S_OK;
     }

     STDMETHODIMP OnEvent( DWORD, IMFMediaEvent * )
     {
          return S_OK;
     }

     STDMETHODIMP OnFlush( DWORD )
     {
          return S_OK;
     }
     HRESULT Wait( DWORD dwMilliseconds, BOOL *pbEOS )
     {
          *pbEOS = FALSE;

          DWORD dwResult = WaitForSingleObject( m_hEvent, dwMilliseconds );
          if( dwResult == WAIT_TIMEOUT )
          {
               return E_PENDING;
          }
          else if( dwResult != WAIT_OBJECT_0 )
          {
               return HRESULT_FROM_WIN32( GetLastError() );
          }

          *pbEOS = m_bEOS;
          return m_hrStatus;
     }

private:

     // Destructor is private. Caller should call Release.
     virtual ~SourceReaderCB()
     {}

     void NotifyError( HRESULT hr )
     {
          wprintf( L"Source Reader error: 0x%X\n", hr );
     }
};

#endif
mp_dev_unit g_map_dev_units;
HRESULT EnumDeviceVideoDevices()
{

     IMFMediaSource *pSource = NULL;
     IMFAttributes *pAttributes = NULL;
     IMFActivate **ppDevices = NULL;
    
     // Create an attribute store to specify the enumeration parameters.
     HRESULT hr = MFCreateAttributes( &pAttributes, 1 );
     if( FAILED( hr ) )
     {
          goto done;
     }

     // Source type: video capture devices
     hr = pAttributes->SetGUID(
          MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
          MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
          );
     if( FAILED( hr ) )
     {
          goto done;
     }

     // Enumerate devices.
     UINT32 count = 0;
     for( int ix = 0; ix < 10; ix++ )
     {
          hr = MFEnumDeviceSources( pAttributes, &ppDevices, &count );
          if( FAILED( hr ) || count == 0 )
          {
               Sleep( 10 );
               hr = E_FAIL;
               continue;
          }
          break;
     }

     // Create the media source object.
     for( DWORD id = 0; id < count; id++ )
     {
          wchar_t* szFriendlyName = NULL;
          ppDevices[ id ]->GetAllocatedString(
               MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
               &szFriendlyName,
               NULL
               );
          string str_key = wstringToUtf8( wstring( szFriendlyName ) );
          printf( "cur dev_name=%s\n", str_key.c_str() );
          auto& it = g_map_dev_units.find( str_key );
          if( it == g_map_dev_units.end() )
          {
               IMFMediaSource* psource = NULL;
               hr = ppDevices[ id ]->ActivateObject( IID_PPV_ARGS( &psource ) );
               if( FAILED( hr ) )
               {
                    SafeRelease( &psource );
                    break;
               }
               auto devUnit = make_shared<video_dev_unit>( ppDevices[ id ],psource);
               g_map_dev_units[ str_key ] = devUnit;
     
               auto& txt_width = devUnit->width();
               auto& txt_height = devUnit->height();
               GetVideoFrameSize( psource, txt_width, txt_height );

          }
          //delete szFriendlyName;
     }
done:
     SafeRelease( &pAttributes );

     for( DWORD i = 0; i < count; i++ )
     {
          SafeRelease( &ppDevices[ i ] );
     }
     CoTaskMemFree( ppDevices );
     return hr;
}