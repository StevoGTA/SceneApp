//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.h			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAudioInfo.h"
#include "CByteParceller.h"
#include "CMediaEngine.h"
#include "CMediaPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaEngine

class CSceneAppMediaEngineInternals;
class CSceneAppMediaEngine : public CMediaEngine {
	// Enums
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
	// Procs
	public:
		typedef	CByteParceller	(*CreateByteParcellerProc)(const CString& resourceFilename, void* userData);

	// Structs
	public:
		struct Info {
							// Lifecycle methods
							Info(CreateByteParcellerProc createByteParcellerProc, void* userData) :
								mCreateByteParcellerProc(createByteParcellerProc), mUserData(userData)
								{}
							Info(const Info& other) :
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
							CSceneAppMediaEngine(const Info& info);
							~CSceneAppMediaEngine();

							// Instance methods
		OI<CMediaPlayer>	getMediaPlayer(const CAudioInfo& audioInfo);

	// Properties
	private:
		CSceneAppMediaEngineInternals*	mInternals;
};
