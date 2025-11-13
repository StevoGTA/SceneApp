//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.h			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAudioInfo.h"
#include "CMediaEngine.h"
#include "CMediaPlayer.h"
#include "CSceneAppResourceLoading.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppMediaEngine

class CSceneAppMediaEngine : public CMediaEngine {
	// AudioOptions
	public:
//		enum AudioOptions {
//			kAudioOptionsNone						= 0,
//
//			kAudioOptionsCreateUniquePlayer			= 1 << 0,
//			kAudioOptionsUseDefinedLoopInResource	= 1 << 1,
//			kAudioOptionsLoadIntoMemory				= 1 << 2,
//		};
//

	// VideoInfo
	public:
		struct VideoInfo {
			// Methods
			public:
									// Lifecycle methods
									VideoInfo(const CString& filename) : mFilename(filename) {}

									// Instance methods
				const	CString&	getFilename() const
										{ return mFilename; }

			// Properties
			private:
				CString	mFilename;
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
							// Lifecycle methods
							CSceneAppMediaEngine(CSRSWMessageQueues& messageQueues,
									const CSceneAppResourceLoading& sceneAppResourceLoading);
							~CSceneAppMediaEngine();

							// Instance methods
		OI<CMediaPlayer>	getMediaPlayer(const CAudioInfo& audioInfo,
									const CMediaPlayer::Info& info = CMediaPlayer::Info());
		OI<CMediaPlayer>	getMediaPlayer(const VideoInfo& videoInfo,
									const CMediaPlayer::Info& info = CMediaPlayer::Info());

	protected:
							// CMediaEngine methods
		I<CAudioConverter>	createAudioConverter() const;

	// Properties
	private:
		Internals*	mInternals;
};
