//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.h			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAudioInfo.h"
#include "CMediaEngine.h"
#include "CMediaPlayer.h"
#include "CNotificationCenter.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppMediaEngine

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
		typedef	I<CDataSource>	(*CreateDataSourceProc)(const CString& resourceFilename, void* userData);

	// Structs
	public:
		struct Info {
							// Lifecycle methods
							Info(CreateDataSourceProc createDataSourceProc, void* userData) :
								mCreateDataSourceProc(createDataSourceProc), mUserData(userData)
								{}
							Info(const Info& other) :
								mCreateDataSourceProc(other.mCreateDataSourceProc), mUserData(other.mUserData)
								{}

							// Instance methods
			I<CDataSource>	createDataSource(const CString& resourceFilename) const
								{ return mCreateDataSourceProc(resourceFilename, mUserData); }

			// Properties
			private:
				CreateDataSourceProc	mCreateDataSourceProc;
				void*					mUserData;
		};

		struct VideoInfo {
			// Lifecycle methods
			VideoInfo(const CString& filename) : mFilename(filename) {}

			// Properties
			CString	mFilename;
		};

	// Methods
	public:
							// Lifecycle methods
							CSceneAppMediaEngine(CSRSWMessageQueues& messageQueues, const Info& info);
							~CSceneAppMediaEngine();

							// Instance methods
		OI<CMediaPlayer>	getMediaPlayer(const CAudioInfo& audioInfo,
									const CMediaPlayer::Info& info = CMediaPlayer::Info());
		OI<CMediaPlayer>	getMediaPlayer(const VideoInfo& videoInfo,
									CVideoCodec::DecodeFrameInfo::Compatibility compatibility,
									const CVideoDecoder::RenderInfo& renderInfo,
									const CMediaPlayer::Info& info = CMediaPlayer::Info());

							// Subclass methods
		I<CAudioConverter>	createAudioConverter() const;

	// Properties
	private:
		CSceneAppMediaEngineInternals*	mInternals;
};
