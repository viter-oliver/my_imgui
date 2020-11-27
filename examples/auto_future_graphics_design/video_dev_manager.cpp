#include "video_dev_manager.h"
#include "common_functions.h"

#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"mf.lib")
#pragma comment(lib,"mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

template <class T> void SafeRelease( T **ppT )
{
     if( *ppT )
     {
          ( *ppT )->Release();
          *ppT = NULL;
     }
}
template <class T> inline void SafeRelease( T*& pT )
{
     if( pT != NULL )
     {
          pT->Release();
          pT = NULL;
     }
}
/**
* Copies a media type attribute from an input media type to an output media type. Useful when setting
* up the video sink and where a number of the video sink input attributes need to be duplicated on the
* video writer attributes.
* @param[in] pSrc: the media attribute the copy of the key is being made from.
* @param[in] pDest: the media attribute the copy of the key is being made to.
* @param[in] key: the media attribute key to copy.
*/
HRESULT CopyAttribute( IMFAttributes* pSrc, IMFAttributes* pDest, const GUID& key )
{
     PROPVARIANT var;
     PropVariantInit( &var );

     HRESULT hr = S_OK;

     hr = pSrc->GetItem( key, &var );
     if( SUCCEEDED( hr ) )
     {
          hr = pDest->SetItem( key, var );
     }

     PropVariantClear( &var );
     return hr;
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
     if( _pReader )
     {
          //_pSource->Shutdown();
     }
     SafeRelease( _pReader );
     SafeRelease(_videoSourceOutputType );
}

void video_dev_unit::start_pulling_data()
{
     assert( _pReader != NULL );
     thread td_pulling_data( [&]
     {
          HRESULT hr;
          IMFMediaType *pConfigureType = NULL;
          dwCurrentLenth = _width*_height * 4;
          pVideoframe = new BYTE[ dwCurrentLenth ];

          do
          {
               DWORD dwStream = MF_SOURCE_READER_FIRST_VIDEO_STREAM;
               for( DWORD dwMediaTypeIndex = 0;; dwMediaTypeIndex++ )
               {
                    IMFMediaType *pType = NULL;
                    hr = _pReader->GetNativeMediaType( dwStream, dwMediaTypeIndex, &pType );
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
               IMFSample *videoSample = NULL;
               while( _be_pulling_frame )
               {
                    DWORD streamIndex, flags;
                    LONGLONG llTimeStamp;
                    hr = _pReader->ReadSample(
                         dwStream,    // Stream index.
                         0,                              // Flags.
                         &streamIndex,                   // Receives the actual stream index. 
                         &flags,                         // Receives status flags.
                         &llTimeStamp,                   // Receives the time stamp.
                         &videoSample                        // Receives the sample or NULL.
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
                         hr = videoSample->GetBufferCount( &dwBuffCount );

                         if( dwBuffCount>0 )
                         {
                              LONGLONG phduration = 0;
                              hr = videoSample->GetSampleDuration( &phduration );
                              if( SUCCEEDED( hr ) )
                              {
                              }
                              IMFMediaBuffer* pMediaBuffer;
                              hr = videoSample->ConvertToContiguousBuffer( &pMediaBuffer );

                              //hr = pMediaBuffer->GetCurrentLength(&dwBufferSize);
                              BYTE* pData = NULL;
                              DWORD dtmpLenth = 0;
                              hr = pMediaBuffer->Lock( &pData, &dwMaxlenth, &dtmpLenth );
                              FromYUY2ToBGR32( pVideoframe, pData, _width, _height );

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

                    SafeRelease( &videoSample );
               }

          }
          while( false );

          delete pVideoframe;
          SafeRelease( &pConfigureType );
     } );
     td_pulling_data.detach();
}
struct init_mtf_env
{
     IUnknown* colorConvTransformUnk = NULL;
     IMFTransform* pColorConvTransform = NULL; // This is colour converter MFT is used to convert between the webcam pixel format and RGB32.
 
     init_mtf_env()
     {
          try
          {
               CHECK_HR( CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE ),
                         "COM initialisation failed." );
               CHECK_HR( MFTRegisterLocalByCLSID(
                    __uuidof( CColorConvertDMO ),
                    MFT_CATEGORY_VIDEO_PROCESSOR,
                    L"",
                    MFT_ENUM_FLAG_SYNCMFT,
                    0,
                    NULL,
                    0,
                    NULL ),
                    "Error registering colour converter DSP." );
               CHECK_HR( CoCreateInstance( CLSID_CColorConvertDMO, NULL, CLSCTX_INPROC_SERVER,
                    IID_IUnknown, (void**)&colorConvTransformUnk ),
                    "Failed to create colour converter MFT." );
               CHECK_HR( colorConvTransformUnk->QueryInterface( IID_PPV_ARGS( &pColorConvTransform ) ),
                         "Failed to get IMFTransform interface from colour converter MFT object." );
               CHECK_HR( pColorConvTransform->ProcessMessage( MFT_MESSAGE_COMMAND_FLUSH, NULL ), "Failed to process FLUSH command on colour converter MFT." );
               CHECK_HR( pColorConvTransform->ProcessMessage( MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, NULL ), "Failed to process BEGIN_STREAMING command on colour converter MFT." );
               CHECK_HR( pColorConvTransform->ProcessMessage( MFT_MESSAGE_NOTIFY_START_OF_STREAM, NULL ), "Failed to process START_OF_STREAM command on colour converter MFT." );

          }
          catch( ... )
          {
               printf( "fail to initialize mtf env\n" );
          }

     }
} mtf_env;
const int VIDEO_FRAME_RATE = 30;
void video_dev_unit::start_pulling_data_with_mtf()
{
     assert( _pReader != NULL );
     thread td_pulling_data( [&]
     {
          HRESULT hr;
          IMFSample *videoSample = NULL;
          IMFMediaType* pDecInputMediaType = NULL, *pDecOutputMediaType=NULL;
          IMFMediaType* pWebcamSourceType = NULL, *pImfEvrSinkType = NULL;
          IMFTransform*& pColorConvTransform = mtf_env.pColorConvTransform;
          CHECK_HR( MFCreateMediaType( &pImfEvrSinkType ), "Failed to create video output media type." );
          CHECK_HR( pImfEvrSinkType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Video ), "Failed to set video output media major type." );
          CHECK_HR( pImfEvrSinkType->SetGUID( MF_MT_SUBTYPE, MFVideoFormat_RGB32 ), "Failed to set video sub-type attribute on media type." );
          CHECK_HR( pImfEvrSinkType->SetUINT32( MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive ), "Failed to set interlace mode attribute on media type." );
          CHECK_HR( pImfEvrSinkType->SetUINT32( MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE ), "Failed to set independent samples attribute on media type." );
          CHECK_HR( MFSetAttributeRatio( pImfEvrSinkType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1 ), "Failed to set pixel aspect ratio attribute on media type." );
          CHECK_HR( MFSetAttributeSize( pImfEvrSinkType, MF_MT_FRAME_SIZE, _width, _height ), "Failed to set the frame size attribute on media type." );
          CHECK_HR( MFSetAttributeSize( pImfEvrSinkType, MF_MT_FRAME_RATE, VIDEO_FRAME_RATE, 1 ), "Failed to set the frame rate attribute on media type." );

          CHECK_HR( MFCreateMediaType( &pWebcamSourceType ), "Failed to create webcam output media type." );
          CHECK_HR( pImfEvrSinkType->CopyAllItems( pWebcamSourceType ), "Error copying media type attributes from EVR input to webcam output media type." );
          CHECK_HR( CopyAttribute( _videoSourceOutputType, pWebcamSourceType, MF_MT_SUBTYPE ), "Failed to set video sub-type attribute on webcam media type." );
          CHECK_HR( _pReader->SetCurrentMediaType( (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pWebcamSourceType ),
                    "Failed to set output media type on source reader." );

          MFCreateMediaType( &pDecInputMediaType );
          CHECK_HR( pWebcamSourceType->CopyAllItems( pDecInputMediaType ), "Error copying media type attributes to colour converter input media type." );
          CHECK_HR( pColorConvTransform->SetInputType( 0, pDecInputMediaType, 0 ), "Failed to set input media type on colour converter MFT." );
          
          MFCreateMediaType( &pDecOutputMediaType );
          CHECK_HR( pImfEvrSinkType->CopyAllItems( pDecOutputMediaType ), "Error copying media type attributes to colour converter output media type." );
          CHECK_HR( pColorConvTransform->SetOutputType( 0, pDecOutputMediaType, 0 ), "Failed to set output media type on colour converter MFT." );

          pVideoframe = new BYTE[ _width*_height * 4 ];
          try
          {
               _be_pulling_frame = true;
               DWORD  flags;
               while( _be_pulling_frame )
               {
                    DWORD streamIndex, flags;
                    LONGLONG llTimeStamp;
                    hr = _pReader->ReadSample(
                         MF_SOURCE_READER_FIRST_VIDEO_STREAM,    // Stream index.
                         0,                              // Flags.
                         &streamIndex,                   // Receives the actual stream index. 
                         &flags,                         // Receives status flags.
                         &llTimeStamp,                   // Receives the time stamp.
                         &videoSample                        // Receives the sample or NULL.
                         );

                    if( FAILED( hr ) )
                    {
                         break;
                    }
                    if (videoSample)
                    {
                         LONGLONG sampleDuration = 0;
                         DWORD mftOutFlags;

                         // ----- Video source sample. -----

                         CHECK_HR( videoSample->SetSampleTime( llTimeStamp ), "Error setting the video sample time." );
                         CHECK_HR( videoSample->GetSampleDuration( &sampleDuration ), "Failed to get video sample duration." );
                         CHECK_HR( pColorConvTransform->ProcessInput( 0, videoSample, NULL ), "The colour conversion decoder ProcessInput call failed." );
                         IMFSample* mftOutSample = NULL;
                         IMFMediaBuffer* mftOutBuffer = NULL;
                         MFT_OUTPUT_STREAM_INFO StreamInfo;
                         MFT_OUTPUT_DATA_BUFFER outputDataBuffer;
                         DWORD processOutputStatus = 0;

                         CHECK_HR( pColorConvTransform->GetOutputStreamInfo( 0, &StreamInfo ), "Failed to get output stream info from colour conversion MFT." );
                         CHECK_HR( MFCreateSample( &mftOutSample ), "Failed to create MF sample." );
                         CHECK_HR( MFCreateMemoryBuffer( StreamInfo.cbSize, &mftOutBuffer ), "Failed to create memory buffer." );
                         CHECK_HR( mftOutSample->AddBuffer( mftOutBuffer ), "Failed to add sample to buffer." );
                         outputDataBuffer.dwStreamID = 0;
                         outputDataBuffer.dwStatus = 0;
                         outputDataBuffer.pEvents = NULL;
                         outputDataBuffer.pSample = mftOutSample;

                         auto mftProcessOutput = pColorConvTransform->ProcessOutput( 0, 1, &outputDataBuffer, &processOutputStatus );

                         //printf("Colour conversion result %.2X, MFT status %.2X.\n", mftProcessOutput, processOutputStatus);

                         if( mftProcessOutput == S_OK )
                         {
                              // ----- Make Direct3D sample. -----
                              IMFMediaBuffer* buf = NULL;
                              DWORD bufLength = 0, pByteBufLength = 0;
                              BYTE* pByteBuf = NULL;

                              CHECK_HR( mftOutSample->ConvertToContiguousBuffer( &buf ), "ConvertToContiguousBuffer failed." );
                              CHECK_HR( buf->GetCurrentLength( &bufLength ), "Get buffer length failed." );
                              CHECK_HR( buf->Lock( &pByteBuf, NULL, &dwCurrentLenth ), "Failed to lock sample buffer." );
                              memcpy( pVideoframe, pByteBuf, dwCurrentLenth );
                              CHECK_HR( buf->Unlock(), "Failed to unlock source buffer." );

                              unique_lock<mutex> lck( _mutex );
                              _frame_valid = true;
                              _cond.wait( lck );

                              SafeRelease( buf );
                              SafeRelease( mftOutSample );
                              SafeRelease( mftOutBuffer );
                         }
                         else
                         {
                              printf( "Colour conversion failed with %.2X.\n", mftProcessOutput );
                              break;
                         }

                    }
                    if( flags & MF_SOURCE_READERF_ENDOFSTREAM )
                    {
                         printf( "\tEnd of stream\n" );
                         _be_pulling_frame = false;
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
                    SafeRelease( &videoSample );
               }

          }
          catch(...)
          {
               printf( "fail to pull data with the mtf\n " );
          }
         
          delete pVideoframe;

     } );
     td_pulling_data.detach();
}
mp_dev_unit g_map_dev_units;
HRESULT EnumDeviceVideoDevices()
{

     IMFMediaSource *pSource = NULL;
     IMFAttributes *pAttributes = NULL;
     IMFActivate **ppDevices = NULL;
     IMFSourceReader* pVideoReader;

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
               wchar_t* psystem_link = NULL;
               HRESULT hr = ppDevices[ id ]->GetAllocatedString( MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, \
                                                            &psystem_link, \
                                                          NULL );
               hr = MFCreateSourceReaderFromMediaSource(
                    psource,
                    pAttributes,
                    &pVideoReader );

               auto devUnit = make_shared<video_dev_unit>( psystem_link, pVideoReader );
               g_map_dev_units[ str_key ] = devUnit;
               if (!devUnit->initialized())
               {
                    auto& txt_width = devUnit->width();
                    auto& txt_height = devUnit->height();
                    GetVideoFrameSize( psource, txt_width, txt_height );
               }

          }
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