//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.h			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAudioInfo.h"
#include "CMediaEngine.h"
#include "CMediaPlayer.h"
#include "CNotificationCenter.h"
#include "SSceneAppResourceLoading.h"

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

	// Structs
	public:
		struct VideoInfo {
			// Lifecycle methods
			VideoInfo(const CString& filename) : mFilename(filename) {}

			// Properties
			CString	mFilename;
		};

	// Methods
	public:
							// Lifecycle methods
							CSceneAppMediaEngine(CSRSWMessageQueues& messageQueues,
									const SSceneAppResourceLoading& sceneAppResourceLoading);
							~CSceneAppMediaEngine();

							// Instance methods
		OI<CMediaPlayer>	getMediaPlayer(const CAudioInfo& audioInfo,
									const CMediaPlayer::Info& info = CMediaPlayer::Info());
		OI<CMediaPlayer>	getMediaPlayer(const VideoInfo& videoInfo, CVideoFrame::Compatibility compatibility,
									const CMediaPlayer::Info& info = CMediaPlayer::Info());

							// Subclass methods
		I<CAudioConverter>	createAudioConverter() const;

	// Properties
	private:
		CSceneAppMediaEngineInternals*	mInternals;
};
