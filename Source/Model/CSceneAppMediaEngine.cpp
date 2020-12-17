//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMediaEngine.cpp			©2020 Stevo Brock	All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneAppMediaEngine.h"

#include "CAudioTrackReader.h"
#include "CDictionary.h"
#include "CFilesystemPath.h"
#include "CMPEG4MediaSource.h"
#include "CWAVEMediaSource.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneAppMediaPlayer

class CSceneAppMediaPlayer : public CMediaPlayer {
	public:
				CSceneAppMediaPlayer(CSRSWMessageQueue& messageQueue, const CString& resourceFilename,
						TReferenceDictionary<CSceneAppMediaPlayer>& sceneAppMediaPlayerMap) :
					CMediaPlayer(messageQueue), mResourceFilename(resourceFilename),
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
						CSceneAppMediaPlayerReference(CSRSWMessageQueue& messageQueue,
								CSceneAppMediaPlayer& sceneAppMediaPlayer) :
							CMediaPlayer(messageQueue), mSceneAppMediaPlayer(sceneAppMediaPlayer)
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
		CSceneAppMediaEngineInternals(CSRSWMessageQueue& messageQueue, const CSceneAppMediaEngine::Info& info) :
			mMessageQueue(messageQueue), mInfo(info)
			{}

		CSRSWMessageQueue&							mMessageQueue;
		CSceneAppMediaEngine::Info					mInfo;
		TReferenceDictionary<CSceneAppMediaPlayer>	mSceneAppMediaPlayerMap;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMediaEngine

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppMediaEngine::CSceneAppMediaEngine(CSRSWMessageQueue& messageQueue, const Info& info) : CMediaEngine()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneAppMediaEngineInternals(messageQueue, info);
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

	CString	extension = CFilesystemPath(resourceFilename).getExtension();
	if (extension == CString(OSSTR("m4v"))) {
		// MPEG4 Audio/Video
		return OI<CMediaPlayer>();
	} else if (extension == CString(OSSTR("wav"))) {
		// WAVE Audio
		const	OR<CSceneAppMediaPlayer>	sceneAppMediaPlayerReference =
													mInternals->mSceneAppMediaPlayerMap[resourceFilename];
		if (sceneAppMediaPlayerReference.hasReference())
			// Have reference
			return OI<CMediaPlayer>(
					new CSceneAppMediaPlayerReference(mInternals->mMessageQueue, *sceneAppMediaPlayerReference));
		else {
			// Create new
			CByteParceller		byteParceller = mInternals->mInfo.createByteParceller(resourceFilename);
			CWAVEMediaSource	mediaSource;
			OI<SError>			error = mediaSource.loadTracks(byteParceller);
			if (error.hasInstance())
				// Error
				return OI<CMediaPlayer>();

			// Setup Media Player
			TArray<CAudioTrack>		audioTracks = mediaSource.getAudioTracks();
			CSceneAppMediaPlayer*	sceneAppMediaPlayer =
											new CSceneAppMediaPlayer(mInternals->mMessageQueue, resourceFilename,
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
					new CSceneAppMediaPlayerReference(mInternals->mMessageQueue, *sceneAppMediaPlayer));
		}
	} else {
		// Unimplemented
		AssertFailUnimplemented();

		return OI<CMediaPlayer>();
	}
}
