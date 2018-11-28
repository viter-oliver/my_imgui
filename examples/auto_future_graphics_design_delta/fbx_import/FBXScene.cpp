#include "FBXScene.h"

namespace FBXImporterUnmanaged
{
	FBXScenePtr FBXScene::Create()
	{
		FBXScene* newFBXScene = new FBXScene();
		assert( newFBXScene );
		return FBXScenePtr( newFBXScene );
	}

	FBXScene::FBXScene()
	{
	}

	FBXScene::~FBXScene()
	{
	}
}