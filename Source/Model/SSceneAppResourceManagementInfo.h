//----------------------------------------------------------------------------------------------------------------------
//	SSceneAppResourceManagementInfo.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CGPUTextureManager.h"
#include "CSceneAppMediaEngine.h"
#include "SSceneAppResourceLoading.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneAppResourceManagementInfo

struct SSceneAppResourceManagementInfo {
	// Methods
	public:
								// Lifecycle methods
								SSceneAppResourceManagementInfo(CGPUTextureManager& gpuTextureManager,
										CSceneAppMediaEngine& sceneAppMediaEngine,
										const SSceneAppResourceLoading& sceneAppResourceLoading) :
									mGPUTextureManager(gpuTextureManager), mSceneAppMediaEngine(sceneAppMediaEngine),
											mSceneAppResourceLoading(sceneAppResourceLoading)
									{}
								SSceneAppResourceManagementInfo(const SSceneAppResourceManagementInfo& other) :
									mGPUTextureManager(other.mGPUTextureManager),
											mSceneAppMediaEngine(other.mSceneAppMediaEngine),
											mSceneAppResourceLoading(other.mSceneAppResourceLoading)
									{}

								// Instance methods
	I<CDataSource>				createDataSource(const CString& resourceFilename) const
									{ return mSceneAppResourceLoading.createDataSource(resourceFilename); }
	I<CRandomAccessDataSource>	createRandomAccessDataSource(const CString& resourceFilename) const
									{ return mSceneAppResourceLoading.createRandomAccessDataSource(resourceFilename); }

	// Properties
	public:
				CGPUTextureManager&			mGPUTextureManager;
				CSceneAppMediaEngine&		mSceneAppMediaEngine;
		const	SSceneAppResourceLoading&	mSceneAppResourceLoading;
};
