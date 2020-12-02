//----------------------------------------------------------------------------------------------------------------------
//	CScene.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CGPUTextureManager.h"
#include "CSceneAppMediaEngine.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: - SSceneAppResourceManagementInfo

typedef	CByteParceller	(*SSceneAppResourceManagementInfoCreateByteParcellerProc)(const CString& resourceFilename,
								void* userData);

struct SSceneAppResourceManagementInfo {
					// Lifecycle methods
					SSceneAppResourceManagementInfo(
							SSceneAppResourceManagementInfoCreateByteParcellerProc createByteParcellerProc,
							CGPUTextureManager& gpuTextureManager, CSceneAppMediaEngine& sceneAppMediaEngine,
							void* userData) :
						mCreateByteParcellerProc(createByteParcellerProc), mGPUTextureManager(gpuTextureManager),
								mSceneAppMediaEngine(sceneAppMediaEngine), mUserData(userData)
						{}

					// Instance methods
	CByteParceller	createByteParceller(const CString& resourceFilename) const
						{ return mCreateByteParcellerProc(resourceFilename, mUserData); }

	// Properties
	public:
		CGPUTextureManager&										mGPUTextureManager;
		CSceneAppMediaEngine&									mSceneAppMediaEngine;

	private:
		SSceneAppResourceManagementInfoCreateByteParcellerProc	mCreateByteParcellerProc;
		void*													mUserData;
};
