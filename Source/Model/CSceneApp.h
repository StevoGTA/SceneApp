//----------------------------------------------------------------------------------------------------------------------
//	CScene.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CGPUTextureManager.h"
#include "CSceneAppMediaEngine.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneAppResourceManagementInfo

struct SSceneAppResourceManagementInfo {
	// Procs
	public:
		typedef	CByteParceller	(*CreateByteParcellerProc)(const CString& resourceFilename, void* userData);

	// Methods
	public:
					// Lifecycle methods
					SSceneAppResourceManagementInfo(CreateByteParcellerProc createByteParcellerProc,
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
		CGPUTextureManager&		mGPUTextureManager;
		CSceneAppMediaEngine&	mSceneAppMediaEngine;

	private:
		CreateByteParcellerProc	mCreateByteParcellerProc;
		void*					mUserData;
};
