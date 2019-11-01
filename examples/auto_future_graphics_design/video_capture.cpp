#include "video_capture.h"
#include <stdio.h>
#include <comdef.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>
#include<map>
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"mf.lib")
#pragma comment(lib,"mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")
template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
using namespace std;
typedef HRESULT(*COPY_TRANSFORM_FN)(BYTE*, LONG, BYTE*, LONG, UINT, UINT);

struct ConversionFunction {
	GUID guidSubType;
	LPCWSTR pwszGuidSubType;
	COPY_TRANSFORM_FN FnCopyTransform;
};
typedef map<GUID, ConversionFunction> mpConversionFunc;
ConversionFunction FormatConversions[] = {
	/* Uncompressed RGB formats */
	{ MFVideoFormat_RGB8,  L"MFVideoFormat_RGB8", NULL  },
	{ MFVideoFormat_RGB555, L"MFVideoFormat_RGB555", NULL },
	{ MFVideoFormat_RGB565, L"MFVideoFormat_RGB565", NULL },
	{ MFVideoFormat_RGB32, L"MFVideoFormat_RGB32", NULL },
	{ MFVideoFormat_RGB24, L"MFVideoFormat_RGB24", NULL },
	{ MFVideoFormat_ARGB32, L"MFVideoFormat_ARGB32", NULL },
	/* YUV Formats: 8-Bit and Palettized */
	{ MFVideoFormat_AI44, L"MFVideoFormat_AI44", NULL },
	{ MFVideoFormat_AYUV, L"MFVideoFormat_AYUV", NULL },
	{ MFVideoFormat_I420, L"MFVideoFormat_I420", NULL },
	{ MFVideoFormat_IYUV, L"MFVideoFormat_IYUV", NULL },
	{ MFVideoFormat_NV11, L"MFVideoFormat_NV11", NULL },
	{ MFVideoFormat_NV12, L"MFVideoFormat_NV12", NULL },
	{ MFVideoFormat_UYVY, L"MFVideoFormat_UYVY", NULL },
	{ MFVideoFormat_Y41P, L"MFVideoFormat_Y41P", NULL },
	{ MFVideoFormat_Y41T, L"MFVideoFormat_Y41T", NULL },
	{ MFVideoFormat_Y42T, L"MFVideoFormat_Y42T", NULL },
	{ MFVideoFormat_YUY2, L"MFVideoFormat_YUY2", NULL /*FnCopyYUY2ToRGB32*/ },
	{ MFVideoFormat_YV12, L"MFVideoFormat_YV12", NULL },
	/* YUV Formats: 10-Bit and 16-Bit */
	{ MFVideoFormat_P010, L"MFVideoFormat_P010", NULL },
	{ MFVideoFormat_P016, L"MFVideoFormat_P016", NULL },
	{ MFVideoFormat_P210, L"MFVideoFormat_P210", NULL },
	{ MFVideoFormat_P216, L"MFVideoFormat_P216", NULL },
	{ MFVideoFormat_v210, L"MFVideoFormat_v210", NULL },
	{ MFVideoFormat_v216, L"MFVideoFormat_v216", NULL },
	{ MFVideoFormat_v410, L"MFVideoFormat_v410", NULL },
	{ MFVideoFormat_Y210, L"MFVideoFormat_Y210", NULL },
	{ MFVideoFormat_Y216, L"MFVideoFormat_Y216", NULL },
	{ MFVideoFormat_Y410, L"MFVideoFormat_Y410", NULL },
	{ MFVideoFormat_Y416, L"MFVideoFormat_Y416", NULL },
	/* Encoded Video Types */
	{ MFVideoFormat_DV25, L"MFVideoFormat_DV25", NULL },
	{ MFVideoFormat_DV50, L"MFVideoFormat_DV50", NULL },
	{ MFVideoFormat_DVC, L"MFVideoFormat_DVC", NULL },
	{ MFVideoFormat_DVH1, L"MFVideoFormat_DVH1", NULL },
	{ MFVideoFormat_DVHD, L"MFVideoFormat_DVHD", NULL },
	{ MFVideoFormat_DVSD, L"MFVideoFormat_DVSD", NULL },
	{ MFVideoFormat_DVSL, L"MFVideoFormat_DVSL", NULL },
	{ MFVideoFormat_H264, L"MFVideoFormat_H264", NULL },
	{ MFVideoFormat_M4S2, L"MFVideoFormat_M4S2", NULL },
	{ MFVideoFormat_MJPG, L"MFVideoFormat_MJPG", NULL },
	{ MFVideoFormat_MP43, L"MFVideoFormat_MP43", NULL },
	{ MFVideoFormat_MP4S, L"MFVideoFormat_MP4S", NULL },
	{ MFVideoFormat_MP4V, L"MFVideoFormat_MP4V", NULL },
	{ MFVideoFormat_MPEG2, L"MFVideoFormat_MPEG2", NULL },
	{ MFVideoFormat_MPG1, L"MFVideoFormat_MPG1", NULL },
	{ MFVideoFormat_MSS1, L"MFVideoFormat_MSS1", NULL },
	{ MFVideoFormat_MSS2, L"MFVideoFormat_MSS2", NULL },
	{ MFVideoFormat_WMV1, L"MFVideoFormat_WMV1", NULL },
	{ MFVideoFormat_WMV2, L"MFVideoFormat_WMV2", NULL },
	{ MFVideoFormat_WMV3, L"MFVideoFormat_WMV3", NULL },
	{ MFVideoFormat_WVC1, L"MFVideoFormat_WVC1", NULL },
};

IMFMediaSource* g_pSource;
HRESULT CreateVideoDeviceSource(IMFMediaSource **ppSource)
{
	*ppSource = NULL;

	IMFMediaSource *pSource = NULL;
	IMFAttributes *pAttributes = NULL;
	IMFActivate **ppDevices = NULL;

	// Create an attribute store to specify the enumeration parameters.
	HRESULT hr = MFCreateAttributes(&pAttributes, 1);
	if (FAILED(hr))
	{
		goto done;
	}

	// Source type: video capture devices
	hr = pAttributes->SetGUID(
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
		MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
		);
	if (FAILED(hr))
	{
		goto done;
	}

	// Enumerate devices.
	UINT32 count;
	hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
	if (FAILED(hr))
	{
		goto done;
	}

	if (count == 0)
	{
		hr = E_FAIL;
		goto done;
	}

	// Create the media source object.
	hr = ppDevices[0]->ActivateObject(IID_PPV_ARGS(&pSource));
	if (FAILED(hr))
	{
		goto done;
	}

	*ppSource = pSource;
	(*ppSource)->AddRef();

done:
	SafeRelease(&pAttributes);

	for (DWORD i = 0; i < count; i++)
	{
		SafeRelease(&ppDevices[i]);
	}
	CoTaskMemFree(ppDevices);
	SafeRelease(&pSource);
	return hr;
}

BOOL IsTargetMediaType(IMFMediaType *pType)
{
	GUID subType = { 0 };
	HRESULT hr = pType->GetGUID(MF_MT_SUBTYPE, &subType);
	//StringFromCLSID
	return TRUE;
}
HRESULT EnumerateCaptureFormats(IMFMediaSource *pSource)
{
	IMFPresentationDescriptor *pPD = NULL;
	IMFStreamDescriptor *pSD = NULL;
	IMFMediaTypeHandler *pHandler = NULL;
	IMFMediaType *pType = NULL;

	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
	if (FAILED(hr))
	{
		goto done;
	}

	BOOL fSelected;
	hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pSD->GetMediaTypeHandler(&pHandler);
	if (FAILED(hr))
	{
		goto done;
	}

	DWORD cTypes = 0;
	hr = pHandler->GetMediaTypeCount(&cTypes);
	if (FAILED(hr))
	{
		goto done;
	}

	for (DWORD i = 0; i < cTypes; i++)
	{
		hr = pHandler->GetMediaTypeByIndex(i, &pType);
		if (FAILED(hr))
		{
			goto done;
		}
		hr=pHandler->SetCurrentMediaType(pType);
		if (FAILED(hr))
		{
			goto done;
		}
		//LogMediaType(pType);
		//OutputDebugString(L"\n");

		SafeRelease(&pType);
	}

done:
	SafeRelease(&pPD);
	SafeRelease(&pSD);
	SafeRelease(&pHandler);
	SafeRelease(&pType);
	return hr;
}
HRESULT SetDeviceFormat(IMFMediaSource *pSource, DWORD dwFormatIndex)
{
	IMFPresentationDescriptor *pPD = NULL;
	IMFStreamDescriptor *pSD = NULL;
	IMFMediaTypeHandler *pHandler = NULL;
	IMFMediaType *pType = NULL;

	HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
	if (FAILED(hr))
	{
		goto done;
	}

	BOOL fSelected;
	hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pSD->GetMediaTypeHandler(&pHandler);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pHandler->GetMediaTypeByIndex(dwFormatIndex, &pType);
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pHandler->SetCurrentMediaType(pType);

done:
	SafeRelease(&pPD);
	SafeRelease(&pSD);
	SafeRelease(&pHandler);
	SafeRelease(&pType);
	return hr;
}
void usb_device_state(int state)
{
	printf("usb state=%d\n",state);
}