//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.cpp			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppMediaEngine.h"

#include "CAudioDecoder.h"
#include "CDictionary.h"
#include "CFilesystemPath.h"
#include "CMediaSourceRegistry.h"
#include "CVideoDecoder.h"

#if TARGET_OS_IOS || TARGET_OS_MACOS || TARGET_OS_TVOS
	#include "CCoreAudioAudioConverter.h"
#elif TARGET_OS_WINDOWS
	#include "CSecretRabbitCodeAudioConverter.h"
#endif

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppMediaPlayer

class CSceneAppMediaPlayer : public CMediaPlayer {
	public:
				CSceneAppMediaPlayer(CSRSWMessageQueues& messageQueues, const CMediaPlayer::Info& info,
						const CString& resourceFilename,
						TReferenceDictionary<CSceneAppMediaPlayer>& sceneAppMediaPlayerMap) :
					CMediaPlayer(messageQueues, info), mResourceFilename(resourceFilename),
							mSceneAppMediaPlayerMap(sceneAppMediaPlayerMap), mReferenceCount(0)
					{}

		void	addReference()
					{ mReferenceCount++; }
		void	removeReference()
					{
						// Check if last reference
						if (--mReferenceCount == 0) {
							// Going away now
							mSceneAppMediaPlayerMap.remove(mResourceFilename);

							CSceneAppMediaPlayer*	THIS = this;
							Delete(THIS);
						}
					}

		CString										mResourceFilename;
		TReferenceDictionary<CSceneAppMediaPlayer>&	mSceneAppMediaPlayerMap;
		UInt32										mReferenceCount;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaPlayerReference

class CSceneAppMediaPlayerReference : public CMediaPlayer {
	public:
							CSceneAppMediaPlayerReference(CSRSWMessageQueues& messageQueues,
									const CMediaPlayer::Info& info, CSceneAppMediaPlayer& sceneAppMediaPlayer) :
								CMediaPlayer(messageQueues, info), mSceneAppMediaPlayer(sceneAppMediaPlayer)
								{ mSceneAppMediaPlayer.addReference(); }
							~CSceneAppMediaPlayerReference()
								{ mSceneAppMediaPlayer.removeReference(); }

		void				setupComplete()
								{ mSceneAppMediaPlayer.setupComplete(); }

		I<CAudioPlayer>		newAudioPlayer(const CString& identifier, UInt32 trackIndex)
								{ return mSceneAppMediaPlayer.newAudioPlayer(identifier, trackIndex); }
		void				setAudioGain(Float32 audioGain)
								{ mSceneAppMediaPlayer.setAudioGain(audioGain); }

		I<CVideoFrameStore>	newVideoFrameStore(const CString& identifier, UInt32 trackIndex)
								{ return mSceneAppMediaPlayer.newVideoFrameStore(identifier, trackIndex); }

		void				setLoopCount(OV<UInt32> loopCount = OV<UInt32>())
								{ mSceneAppMediaPlayer.setLoopCount(loopCount); }

		void				play()
								{ mSceneAppMediaPlayer.play(); }
		void				pause()
								{ mSceneAppMediaPlayer.pause(); }
		bool				isPlaying() const
								{ return mSceneAppMediaPlayer.isPlaying(); }

		OI<SError>			reset()
								{ return mSceneAppMediaPlayer.reset(); }

		CSceneAppMediaPlayer&	mSceneAppMediaPlayer;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaEngineInternals

class CSceneAppMediaEngineInternals {
	public:
		CSceneAppMediaEngineInternals(CSRSWMessageQueues& messageQueues,
				const SSceneAppResourceLoading& sceneAppResourceLoading) :
			mMessageQueues(messageQueues), mSSceneAppResourceLoading(sceneAppResourceLoading)
			{}

		CSRSWMessageQueues&							mMessageQueues;
		SSceneAppResourceLoading					mSSceneAppResourceLoading;
		TReferenceDictionary<CSceneAppMediaPlayer>	mSceneAppMediaPlayerMap;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaEngine

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppMediaEngine::CSceneAppMediaEngine(CSRSWMessageQueues& messageQueues,
		const SSceneAppResourceLoading& sceneAppResourceLoading) : CMediaEngine()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneAppMediaEngineInternals(messageQueues, sceneAppResourceLoading);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppMediaEngine::~CSceneAppMediaEngine()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
OI<CMediaPlayer> CSceneAppMediaEngine::getMediaPlayer(const CAudioInfo& audioInfo, const CMediaPlayer::Info& info)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CString&			resourceFilename = audioInfo.getResourceFilename();
//			CAudioInfo::Options	audioInfoOptions = audioInfo.getOptions();

	// Look for existing media player
	const	OR<CSceneAppMediaPlayer>	sceneAppMediaPlayerReference =
												mInternals->mSceneAppMediaPlayerMap[resourceFilename];
	if (sceneAppMediaPlayerReference.hasReference())
		// Have reference
		return OI<CMediaPlayer>(
				new CSceneAppMediaPlayerReference(mInternals->mMessageQueues, info, *sceneAppMediaPlayerReference));

	// Query audio tracks
			I<CSeekableDataSource>							seekableDataSource =
																	mInternals->mSSceneAppResourceLoading
																			.createSeekableDataSource(resourceFilename);
			CString											extension =
																	CFilesystemPath(resourceFilename).getExtension();
			TIResult<CMediaSourceRegistry::IdentifyInfo>	identifyInfo =
																	CMediaSourceRegistry::mShared.identify(
																			seekableDataSource, extension);
	const	TArray<CAudioTrack>&							audioTracks =
																	identifyInfo.getValue().getMediaTracks()
																			.getAudioTracks();

	// Setup Media Player
	CSceneAppMediaPlayer*	sceneAppMediaPlayer =
									new CSceneAppMediaPlayer(mInternals->mMessageQueues, info, resourceFilename,
											mInternals->mSceneAppMediaPlayerMap);
	for (CArray::ItemIndex i = 0; i < audioTracks.getCount(); i++) {
		// Setup
		const	CAudioTrack&		audioTrack = audioTracks[i];
				I<CAudioDecoder>	audioDecoder(
											new CAudioDecoder(audioTrack.getAudioStorageFormat(),
													audioTrack.getDecodeInfo(), seekableDataSource));
				I<CAudioPlayer>		audioPlayer = sceneAppMediaPlayer->newAudioPlayer(resourceFilename, i);

		// Connect
		ConnectResult	connectResult =
								connect((I<CAudioProcessor>&) audioDecoder, (I<CAudioProcessor>&) audioPlayer,
										composeAudioProcessingFormat(*audioDecoder, *audioPlayer));
		if (!connectResult.isSuccess()) {
			// Cleanup
			Delete(sceneAppMediaPlayer);

			return OI<CMediaPlayer>();
		}
	}

	// Finish setup
	sceneAppMediaPlayer->setAudioGain(audioInfo.getGain());
	sceneAppMediaPlayer->setLoopCount(audioInfo.getLoopCount());
	sceneAppMediaPlayer->setupComplete();

	// Success
	mInternals->mSceneAppMediaPlayerMap.set(resourceFilename, *sceneAppMediaPlayer);

	return OI<CMediaPlayer>(new CSceneAppMediaPlayerReference(mInternals->mMessageQueues, info, *sceneAppMediaPlayer));
}

//----------------------------------------------------------------------------------------------------------------------
OI<CMediaPlayer> CSceneAppMediaEngine::getMediaPlayer(const VideoInfo& videoInfo,
		CVideoFrame::Compatibility compatibility, const CMediaPlayer::Info& info)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CString&	filename = videoInfo.mFilename;

	// Query tracks
			I<CSeekableDataSource>							seekableDataSource =
																	mInternals->mSSceneAppResourceLoading
																			.createSeekableDataSource(filename);
			CString											extension = CFilesystemPath(filename).getExtension();
			TIResult<CMediaSourceRegistry::IdentifyInfo>	identifyInfo =
																	CMediaSourceRegistry::mShared.identify(
																			seekableDataSource, extension);
	const	TArray<CAudioTrack>&							audioTracks =
																	identifyInfo.getValue().getMediaTracks()
																			.getAudioTracks();
	const	TArray<CVideoTrack>&							videoTracks =
																	identifyInfo.getValue().getMediaTracks()
																			.getVideoTracks();

	// Setup Media Player
	CSceneAppMediaPlayer*	sceneAppMediaPlayer =
									new CSceneAppMediaPlayer(mInternals->mMessageQueues, info, filename,
											mInternals->mSceneAppMediaPlayerMap);
	for (CArray::ItemIndex i = 0; i < audioTracks.getCount(); i++) {
		// Setup
		const	CAudioTrack&		audioTrack = audioTracks[i];
				I<CAudioDecoder>	audioDecoder(
											new CAudioDecoder(audioTrack.getAudioStorageFormat(),
													audioTrack.getDecodeInfo(), seekableDataSource));
				I<CAudioPlayer>		audioPlayer = sceneAppMediaPlayer->newAudioPlayer(filename, i);

		// Connect
		ConnectResult	connectResult =
								connect((I<CAudioProcessor>&) audioDecoder, (I<CAudioProcessor>&) audioPlayer,
										composeAudioProcessingFormat(*audioDecoder, *audioPlayer));
		if (!connectResult.isSuccess()) {
			// Cleanup
			Delete(sceneAppMediaPlayer);

			return OI<CMediaPlayer>();
		}
	}
	for (CArray::ItemIndex i = 0; i < videoTracks.getCount(); i++) {
		// Setup
		const	CVideoTrack&		videoTrack = videoTracks[i];
				I<CVideoDecoder>	videoDecoder(
											new CVideoDecoder(videoTrack.getVideoStorageFormat(),
													videoTrack.getDecodeInfo(), seekableDataSource, compatibility));
				I<CVideoFrameStore>	videoFrameStore = sceneAppMediaPlayer->newVideoFrameStore(filename, i);

		// Connect
		OI<SError>	error = videoFrameStore->connectInput((const I<CVideoProcessor>&) videoDecoder);
		if (error.hasInstance()) {
			// Cleanup
			Delete(sceneAppMediaPlayer);

			return OI<CMediaPlayer>();
		}
	}

	// Finish setup
	sceneAppMediaPlayer->setupComplete();

	// Success
	mInternals->mSceneAppMediaPlayerMap.set(filename, *sceneAppMediaPlayer);

	return OI<CMediaPlayer>(new CSceneAppMediaPlayerReference(mInternals->mMessageQueues, info, *sceneAppMediaPlayer));
}

// MARK: Subclass methods

//----------------------------------------------------------------------------------------------------------------------
I<CAudioConverter> CSceneAppMediaEngine::createAudioConverter() const
//----------------------------------------------------------------------------------------------------------------------
{
#if TARGET_OS_IOS || TARGET_OS_MACOS || TARGET_OS_TVOS
	// Apple
	return I<CAudioConverter>(new CCoreAudioAudioConverter());
#elif TARGET_OS_WINDOWS
	// Windows
	return I<CAudioConverter>(new CSecretRabbitCodeAudioConverter());
#endif
}
