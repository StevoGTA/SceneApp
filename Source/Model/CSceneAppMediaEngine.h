//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.h			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAudioInfo.h"
#include "CByteParceller.h"
#include "CMediaEngine.h"
#include "CMediaPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK:  - CSceneAppMediaEngine

class CSceneAppMediaEngineInternals;
class CSceneAppMediaEngine : public CMediaEngine {
	// Types
	public:
//		// EAudioOptions
//		enum EAudioOptions {
//			kAudioOptionsNone						= 0,
//
//			kAudioOptionsCreateUniquePlayer			= 1 << 0,
//			kAudioOptionsUseDefinedLoopInResource	= 1 << 1,
//			kAudioOptionsLoadIntoMemory				= 1 << 2,
//		};
//
		typedef	CByteParceller	(*CreateByteParcellerProc)(const CString& resourceFilename, void* userData);

		struct SInfo {
							// Lifecycle methods
							SInfo(CreateByteParcellerProc createByteParcellerProc, void* userData) :
								mCreateByteParcellerProc(createByteParcellerProc), mUserData(userData)
								{}
							SInfo(const SInfo& other) :
								mCreateByteParcellerProc(other.mCreateByteParcellerProc), mUserData(other.mUserData)
								{}

							// Instance methods
			CByteParceller	createByteParceller(const CString& resourceFilename) const
								{ return mCreateByteParcellerProc(resourceFilename, mUserData); }

			// Properties
			private:
				CreateByteParcellerProc	mCreateByteParcellerProc;
				void*					mUserData;
		};

	// Methods
	public:
							// Lifecycle methods
							CSceneAppMediaEngine(const SInfo& info);
							~CSceneAppMediaEngine();

							// Instance methods
		OI<CMediaPlayer>	getMediaPlayer(const CAudioInfo& audioInfo);

	// Properties
	private:
		CSceneAppMediaEngineInternals*	mInternals;
};
