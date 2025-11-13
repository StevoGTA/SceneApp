//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.cpp			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppMediaEngine.h"

#include "CAudioDecoder.h"
#include "CDictionary.h"
#include "CFilesystemPath.h"
#include "CMediaSourceRegistry.h"
#include "CVideoDecoder.h"

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_MACOS) || defined(TARGET_OS_TVOS)
	#include "CCoreAudioAudioConverter.h"
#elif defined(TARGET_OS_WINDOWS)
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

		void				setupComplete() const
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

		void				reset()
								{ mSceneAppMediaPlayer.reset(); }

		CSceneAppMediaPlayer&	mSceneAppMediaPlayer;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaEngine::Internals

class CSceneAppMediaEngine::Internals {
	public:
		Internals(CSRSWMessageQueues& messageQueues,
				const CSceneAppResourceLoading& sceneAppResourceLoading) :
			mMessageQueues(messageQueues), mSceneAppResourceLoading(sceneAppResourceLoading)
			{}

				CSRSWMessageQueues&							mMessageQueues;
		const	CSceneAppResourceLoading&					mSceneAppResourceLoading;
				TReferenceDictionary<CSceneAppMediaPlayer>	mSceneAppMediaPlayerMap;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaEngine

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppMediaEngine::CSceneAppMediaEngine(CSRSWMessageQueues& messageQueues,
		const CSceneAppResourceLoading& sceneAppResourceLoading) : CMediaEngine()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(messageQueues, sceneAppResourceLoading);
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
			I<CRandomAccessDataSource>		randomAccessDataSource =
													mInternals->mSceneAppResourceLoading
															.createRandomAccessDataSource(resourceFilename);
			CString							extension = *CFilesystemPath(resourceFilename).getExtension();
			I<SMediaSource::ImportResult>	mediaSourceImportResult =
													CMediaSourceRegistry::mShared.import(
															SMediaSource::ImportSetup(randomAccessDataSource,
																	SMediaSource::kOptionsCreateDecoders),
															extension);

	// Setup Media Player
	CSceneAppMediaPlayer*	sceneAppMediaPlayer =
									new CSceneAppMediaPlayer(mInternals->mMessageQueues, info, resourceFilename,
											mInternals->mSceneAppMediaPlayerMap);
	for (TIteratorD<SMediaSource::Tracks::AudioTrack> iterator =
					mediaSourceImportResult->getTracks().getAudioTracks().getIterator();
			iterator.hasValue(); iterator.advance()) {
		// Setup
		I<CAudioSource>	audioSource = getAudioSource(*iterator, resourceFilename);
		I<CAudioPlayer>	audioPlayer = sceneAppMediaPlayer->newAudioPlayer(resourceFilename, iterator.getIndex());

		// Connect
		TVResult<SAudio::ProcessingFormat>	audioProcessingFormat =
													connect((I<CAudioProcessor>&) audioSource,
															(I<CAudioProcessor>&) audioPlayer,
															composeAudioProcessingFormat(*audioSource, *audioPlayer));
		if (audioProcessingFormat.hasValue())
			// Success
			sceneAppMediaPlayer->add((const I<CAudioDestination>&) audioPlayer, iterator.getIndex());
		else {
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
OI<CMediaPlayer> CSceneAppMediaEngine::getMediaPlayer(const VideoInfo& videoInfo, const CMediaPlayer::Info& info)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CString&	filename = videoInfo.getFilename();

	// Query tracks
			I<CRandomAccessDataSource>						randomAccessDataSource =
																	mInternals->mSceneAppResourceLoading
																			.createRandomAccessDataSource(filename);
			CString											extension = *CFilesystemPath(filename).getExtension();
			I<SMediaSource::ImportResult>					mediaSourceImportResult =
																	CMediaSourceRegistry::mShared.import(
																			SMediaSource::ImportSetup(
																					randomAccessDataSource,
																					SMediaSource::
																							kOptionsCreateDecoders),
																			extension);

	// Setup Media Player
	CSceneAppMediaPlayer*	sceneAppMediaPlayer =
									new CSceneAppMediaPlayer(mInternals->mMessageQueues, info, filename,
											mInternals->mSceneAppMediaPlayerMap);
	for (TIteratorD<SMediaSource::Tracks::AudioTrack> iterator =
					mediaSourceImportResult->getTracks().getAudioTracks().getIterator();
			iterator.hasValue(); iterator.advance()) {
		// Setup
		I<CAudioSource>	audioSource = getAudioSource(*iterator, filename);
		I<CAudioPlayer>	audioPlayer = sceneAppMediaPlayer->newAudioPlayer(filename, iterator.getIndex());

		// Connect
		TVResult<SAudio::ProcessingFormat>	audioProcessingFormat =
													connect((I<CAudioProcessor>&) audioSource,
															(I<CAudioProcessor>&) audioPlayer,
															composeAudioProcessingFormat(*audioSource, *audioPlayer));
		if (audioProcessingFormat.hasValue())
			// Success
			sceneAppMediaPlayer->add((const I<CAudioDestination>&) audioPlayer, iterator.getIndex());
		else {
			// Cleanup
			Delete(sceneAppMediaPlayer);

			return OI<CMediaPlayer>();
		}
	}
	for (TIteratorD<SMediaSource::Tracks::VideoTrack> iterator =
					mediaSourceImportResult->getTracks().getVideoTracks().getIterator();
			iterator.hasValue(); iterator.advance()) {
		// Setup
		I<CVideoSource>		videoSource = getVideoSource(*iterator, filename);
		I<CVideoFrameStore>	videoFrameStore = sceneAppMediaPlayer->newVideoFrameStore(filename, iterator.getIndex());

		// Connect
		OV<SError>	error = videoFrameStore->connectInput((const I<CVideoProcessor>&) videoSource);
		if (!error.hasValue())
			// Success
			sceneAppMediaPlayer->add((const I<CVideoDestination>&) videoFrameStore, iterator.getIndex());
		else {
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
#if defined(TARGET_OS_IOS) || defined(TARGET_OS_MACOS) || defined(TARGET_OS_TVOS)
	// Apple
	return I<CAudioConverter>(new CCoreAudioAudioConverter());
#elif defined(TARGET_OS_WINDOWS)
	// Windows
	return I<CAudioConverter>(new CSecretRabbitCodeAudioConverter());
#endif
}
