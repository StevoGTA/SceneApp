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
		typedef	I<CDataSource>	(*CreateDataSourceProc)(const CString& resourceFilename, void* userData);

	// Methods
	public:
					// Lifecycle methods
					SSceneAppResourceManagementInfo(CreateDataSourceProc createDataSourceProc,
							CGPUTextureManager& gpuTextureManager, CSceneAppMediaEngine& sceneAppMediaEngine,
							void* userData) :
						mCreateDataSourceProc(createDataSourceProc), mGPUTextureManager(gpuTextureManager),
								mSceneAppMediaEngine(sceneAppMediaEngine), mUserData(userData)
						{}

					// Instance methods
	I<CDataSource>	createDataSource(const CString& resourceFilename) const
						{ return mCreateDataSourceProc(resourceFilename, mUserData); }

	// Properties
	public:
		CGPUTextureManager&		mGPUTextureManager;
		CSceneAppMediaEngine&	mSceneAppMediaEngine;

	private:
		CreateDataSourceProc	mCreateDataSourceProc;
		void*					mUserData;
};
