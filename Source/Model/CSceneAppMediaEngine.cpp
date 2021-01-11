//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.cpp			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppMediaEngine.h"

#include "CAudioTrackReader.h"
#include "CDictionary.h"
#include "CFilesystemPath.h"
#include "CMPEG4MediaSource.h"
#include "CWAVEMediaSource.h"

#if TARGET_OS_IOS || TARGET_OS_MACOS || TARGET_OS_TVOS
	#include "CCoreAudioAudioConverter.h"
#elif TARGET_OS_WINDOWS
	#include "CSecretRabbitCodeAudioConverter.h"
#endif

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppMediaPlayer

class CSceneAppMediaPlayer : public CMediaPlayer {
	public:
				CSceneAppMediaPlayer(CSRSWMessageQueues& messageQueues, const CString& resourceFilename,
						TReferenceDictionary<CSceneAppMediaPlayer>& sceneAppMediaPlayerMap) :
					CMediaPlayer(messageQueues), mResourceFilename(resourceFilename),
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
								CSceneAppMediaPlayer& sceneAppMediaPlayer) :
							CMediaPlayer(messageQueues), mSceneAppMediaPlayer(sceneAppMediaPlayer)
							{ mSceneAppMediaPlayer.addReference(); }
						~CSceneAppMediaPlayerReference()
							{ mSceneAppMediaPlayer.removeReference(); }

		void			setupComplete()
							{ mSceneAppMediaPlayer.setupComplete(); }

		I<CAudioPlayer>	newAudioPlayer(const CString& identifier)
							{ return mSceneAppMediaPlayer.newAudioPlayer(identifier); }
		void			setAudioGain(Float32 audioGain)
							{ mSceneAppMediaPlayer.setAudioGain(audioGain); }

		void			setLoopCount(OV<UInt32> loopCount = OV<UInt32>())
							{ mSceneAppMediaPlayer.setLoopCount(loopCount); }

		void			play()
							{ mSceneAppMediaPlayer.play(); }
		void			pause()
							{ mSceneAppMediaPlayer.pause(); }
		bool			isPlaying() const
							{ return mSceneAppMediaPlayer.isPlaying(); }

		OI<SError>		reset()
							{ return mSceneAppMediaPlayer.reset(); }

		CSceneAppMediaPlayer&	mSceneAppMediaPlayer;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaEngineInternals

class CSceneAppMediaEngineInternals {
	public:
		CSceneAppMediaEngineInternals(CSRSWMessageQueues& messageQueues, const CSceneAppMediaEngine::Info& info) :
			mMessageQueues(messageQueues), mInfo(info)
			{}

		CSRSWMessageQueues&							mMessageQueues;
		CSceneAppMediaEngine::Info					mInfo;
		TReferenceDictionary<CSceneAppMediaPlayer>	mSceneAppMediaPlayerMap;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaEngine

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppMediaEngine::CSceneAppMediaEngine(CSRSWMessageQueues& messageQueues, const Info& info) : CMediaEngine()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneAppMediaEngineInternals(messageQueues, info);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneAppMediaEngine::~CSceneAppMediaEngine()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
OI<CMediaPlayer> CSceneAppMediaEngine::getMediaPlayer(const CAudioInfo& audioInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CString&			resourceFilename = audioInfo.getResourceFilename();
			CAudioInfo::Options	audioInfoOptions = audioInfo.getOptions();
if (audioInfoOptions != CAudioInfo::kOptionsNone)
	audioInfoOptions = audioInfoOptions;

	// Look for existing media player
	const	OR<CSceneAppMediaPlayer>	sceneAppMediaPlayerReference =
												mInternals->mSceneAppMediaPlayerMap[resourceFilename];
	if (sceneAppMediaPlayerReference.hasReference())
		// Have reference
		return OI<CMediaPlayer>(
				new CSceneAppMediaPlayerReference(mInternals->mMessageQueues, *sceneAppMediaPlayerReference));

	// Create media source
	CByteParceller		byteParceller = mInternals->mInfo.createByteParceller(resourceFilename);
	OI<CMediaSource>	mediaSource;
	CString				extension = CFilesystemPath(resourceFilename).getExtension();
	if (extension == CString(OSSTR("m4a")))
		// MPEG-4 Audio
		mediaSource = OI<CMediaSource>(new CMPEG4MediaSource(byteParceller));
	else if (extension == CString(OSSTR("wav")))
		// WAVE
		mediaSource = OI<CMediaSource>(new CWAVEMediaSource(byteParceller));
	else {
		// Unimplemented
		AssertFailUnimplemented();

		return OI<CMediaPlayer>();
	}

	// Load tracks
	OI<SError>			error = mediaSource->loadTracks();
	ReturnValueIfError(error, OI<CMediaPlayer>());

	// Retrieve audio tracks
	TArray<CAudioTrack>	audioTracks = mediaSource->getAudioTracks();

	// Setup Media Player
	CSceneAppMediaPlayer*	sceneAppMediaPlayer =
									new CSceneAppMediaPlayer(mInternals->mMessageQueues, resourceFilename,
											mInternals->mSceneAppMediaPlayerMap);
	for (CArray::ItemIndex i = 0; i < audioTracks.getCount(); i++) {
		// Setup
		I<CAudioTrackReader>	audioTrackReader(new CAudioTrackReader(audioTracks[i], byteParceller));
		I<CAudioPlayer>			audioPlayer = sceneAppMediaPlayer->newAudioPlayer(resourceFilename);

		// Connect
		ConnectResult	connectResult =
								connect((I<CAudioProcessor>&) audioTrackReader,
										(I<CAudioProcessor>&) audioPlayer,
										composeAudioProcessingFormat(*audioTrackReader, *audioPlayer));
		if (!connectResult.isSuccess()) {
			// Cleanup
			Delete(sceneAppMediaPlayer);

			return OI<CMediaPlayer>();
		}

		// Add to Media Player
		sceneAppMediaPlayer->add(audioPlayer, i);
	}

	// Finish setup
	sceneAppMediaPlayer->setAudioGain(audioInfo.getGain());
	sceneAppMediaPlayer->setLoopCount(audioInfo.getLoopCount());
	sceneAppMediaPlayer->setupComplete();

	// Success
	mInternals->mSceneAppMediaPlayerMap.set(resourceFilename, *sceneAppMediaPlayer);

	return OI<CMediaPlayer>(
			new CSceneAppMediaPlayerReference(mInternals->mMessageQueues, *sceneAppMediaPlayer));
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
