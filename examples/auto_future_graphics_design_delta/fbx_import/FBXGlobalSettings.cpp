#include "FBXGlobalSettings.h"

namespace FBXImporterUnmanaged
{
	FBXGlobalSettingsPtr FBXGlobalSettings::Create()
	{
		FBXGlobalSettings* p = new FBXGlobalSettings();
		assert( p );
		return FBXGlobalSettingsPtr( p );
	}

	FBXGlobalSettings::FBXGlobalSettings()
	{
	}

	FBXGlobalSettings::~FBXGlobalSettings()
	{
	}
}