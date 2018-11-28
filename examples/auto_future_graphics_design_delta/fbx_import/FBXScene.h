#pragma once

#include "FBXImporterCommon.h"
#include "FBXGlobalSettings.h"
#include "FBXModel.h"

namespace FBXImporterUnmanaged
{
	class FBXScene;
	typedef RCPtr<FBXScene> FBXScenePtr;

	class FBXScene : public RCObject
	{
		FBXModelPtr model;
		FBXGlobalSettingsPtr globalSettings;

	public:
		static FBXScenePtr Create();
		FBXScene();
		virtual ~FBXScene();

		inline void SetModel( FBXModelPtr value ) { model = value; }
		inline void SetGlobalSettings( FBXGlobalSettingsPtr value ) { globalSettings = value; }

		inline FBXModelPtr GetModel() const { return model; }
		inline FBXGlobalSettingsPtr GetGlobalSettings() const { return globalSettings; }
	};
}