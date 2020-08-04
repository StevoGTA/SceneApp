//
//  CSceneApp.h
//  StevoBrock-Core
//
//  Created by Stevo on 9/27/19.
//

#pragma once

#include "C2DGeometry.h"
#include "CArray.h"
#include "CByteParceller.h"
#include "CDataSource.h"
#include "CGPUTextureManager.h"

//----------------------------------------------------------------------------------------------------------------------
/*
	Actions (*required options) (**alternate to required option)

	Open URL: "openURL"
		URL*: url to open

	Play Audio: "playAudio"
		audioFilename*: filename of audio to play

	Play Movie: "playMovie"
		movieFilename*: filename of movie to play

	Scene Cut: "sceneCut"
		sceneIndex*: index of next scene
			loadSceneIndexFrom**: load index of next scene from
			sceneName**: name of next scene
		dontPreload: don't preload the connected scene
		unloadCurent: unload the current scene before switching to the linked scene

	Scene Push: "scenePush"
		sceneIndex*: index of next scene
			loadSceneIndexFrom**: load index of next scene from
			sceneName**: name of next scene
		direction*: "up", "down", "left", or "right"
		auto: not user controller
			duration: duration in seconds
		dontPreload: don't preload the connected scene

	Scene Cover: "sceneCover"
		sceneIndex*: index of next scene
			loadSceneIndexFrom**: load index of next scene from
			sceneName**: name of next scene
		direction*: "up", "down", "left", or "right"
		auto: not user controller
			duration: duration in seconds
		dontPreload: don't preload the connected scene

	 Scene Uncover: "sceneUncover"
		 sceneIndex*: index of next scene
			 loadSceneIndexFrom**: load index of next scene from
			 sceneName**: name of next scene
		 direction*: "up", "down", "left", or "right"
		 auto: not user controller
			 duration: duration in seconds
		 dontPreload: don't preload the connected scene

	Set Item Name/Value: "setItemNameValue"
		itemName*: name of SceneItem
		propertyName*: name of property string
		propertyValue*: property value - either string or number

	Send Item Command: "sendItemCommand"
		itemName*: name of SceneItem
		command*: command string

	Send App Command: "sendAppCommand"
		command*: command string

*/

//----------------------------------------------------------------------------------------------------------------------
// MARK: - SSceneAppResourceManagementInfo

typedef	CByteParceller	(*SSceneAppResourceManagementInfoCreateByteParcellerProc)(const CString& resourceFilename,
								void* userData);
struct SSceneAppResourceManagementInfo {
					// Lifecycle methods
					SSceneAppResourceManagementInfo(
							SSceneAppResourceManagementInfoCreateByteParcellerProc createByteParcellerProc,
							CGPUTextureManager& gpuTextureManager, void* userData) :
						mCreateByteParcellerProc(createByteParcellerProc), mGPUTextureManager(gpuTextureManager),
								mUserData(userData)
						{}

					// Instance methods
	CByteParceller	createByteParceller(const CString& resourceFilename) const
						{ return mCreateByteParcellerProc(resourceFilename, mUserData); }

	// Properties
	public:
		CGPUTextureManager&										mGPUTextureManager;

	private:
		SSceneAppResourceManagementInfoCreateByteParcellerProc	mCreateByteParcellerProc;
		void*													mUserData;
};
