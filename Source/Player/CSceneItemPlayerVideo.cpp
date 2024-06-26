//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerVideo.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerVideo.h"

#include "CGPURenderObject2D.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerVideo::Internals

class CSceneItemPlayerVideo::Internals {
	public:
						Internals(CSceneItemPlayerVideo& sceneItemPlayerVideo,
								const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo) :
							mSceneItemPlayerVideo(sceneItemPlayerVideo),
									mGPUTextureManager(sceneAppResourceManagementInfo.mGPUTextureManager),
									mSceneAppMediaEngine(sceneAppResourceManagementInfo.mSceneAppMediaEngine),
									mIsStarted(false), mCurrentTimeInterval(0.0)
							{}

				void	load(const CString& resourceFilename)
							{
								// Setup media player
								mMediaPlayer =
										mSceneAppMediaEngine.getMediaPlayer(
												CSceneAppMediaEngine::VideoInfo(resourceFilename),
												CMediaPlayer::Info(nil, nil,
														(CMediaPlayer::Info::VideoFrameUpdatedProc) videoFrameUpdated,
														nil, (CMediaPlayer::Info::FinishedProc) finished, this));
							}

		static	void	videoFrameUpdated(const CVideoFrame& videoFrame, Internals* internals)
							{
								// Collect texture references
								TArray<I<CGPUTexture> >			textures =
																		internals->mGPU->registerTextures(videoFrame);
								TNArray<CGPUTextureReference>	gpuTextureReferences;
								for (CArray::ItemIndex i = 0; i < textures.getCount(); i++)
									// Add texture reference
									gpuTextureReferences +=
											internals->mGPUTextureManager.gpuTextureReference(textures[i]);

								// Setup
								CGPUFragmentShader::Proc	fragmentShaderProc;
								switch (videoFrame.getDataFormat()) {
									case CVideoFrame::kDataFormatRGB:
										// RGB
										fragmentShaderProc = CGPUFragmentShader::getRGBAMultiTexture;
										break;

									case CVideoFrame::kDataFormatYCbCr:
										// YCbCr
										fragmentShaderProc =
												CGPUFragmentShader::getProc(videoFrame.getColorPrimaries(),
														videoFrame.getYCbCrConversionMatrix(),
														videoFrame.getColorTransferFunction());
										break;
#if defined(TARGET_OS_WINDOWS)
									default:
										// To cover the Windows compiler which complains about non-covered cases
										fragmentShaderProc = nil;
#endif
								}

								// Create Render Object 2D
								const	S2DRectF32&	screenRect =
															internals->mSceneItemPlayerVideo.getSceneItemVideo()
																	.getScreenRect();
								const	S2DRectU16&	viewRect = videoFrame.getViewRect();
										S2DRectF32	textureRect(S2DPointF32(viewRect.mOrigin.mX, viewRect.mOrigin.mY),
															S2DSizeF32(viewRect.mSize.mWidth, viewRect.mSize.mHeight));
								internals->mRenderObject2D =
										OI<CGPURenderObject2D>(
												CGPURenderObject2D(*internals->mGPU,
														CGPURenderObject2D::Item(textureRect.aspectFitIn(screenRect),
																textureRect),
														gpuTextureReferences, fragmentShaderProc));
							}
		static	void	finished(Internals* internals)
							{
								// Query finished actions
								const	OI<CActions>	actions =
																internals->mSceneItemPlayerVideo.getSceneItemVideo()
																		.getFinishedActions();
								if (actions.hasInstance())
									// Perform
									internals->mSceneItemPlayerVideo.perform(*actions);
							}

		CSceneItemPlayerVideo&	mSceneItemPlayerVideo;
		CGPUTextureManager&		mGPUTextureManager;
		CSceneAppMediaEngine&	mSceneAppMediaEngine;

		OI<CMediaPlayer>		mMediaPlayer;
		OR<CGPU>				mGPU;
		OI<CGPURenderObject2D>	mRenderObject2D;

		bool					mIsStarted;
		UniversalTimeInterval	mCurrentTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Register Scene Item Player

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerVideo

// MARK: Properties

CString	CSceneItemPlayerVideo::mFilenameProperty(OSSTR("filename"));

CString	CSceneItemPlayerVideo::mCommandNamePlay(OSSTR("play"));
CString	CSceneItemPlayerVideo::mCommandNameStop(OSSTR("stop"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerVideo::CSceneItemPlayerVideo(const CSceneItemVideo& sceneItemVideo,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const CSceneItemPlayer::Procs& sceneItemPlayerProcs) :
		CSceneItemPlayer(sceneItemVideo, sceneItemPlayerProcs)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(*this, sceneAppResourceManagementInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerVideo::~CSceneItemPlayerVideo()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerVideo::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	OI<CActions>	actions = getSceneItemVideo().getFinishedActions();

	return actions.hasInstance() ? *actions : CActions();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerVideo::load(CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	const	CSceneItemVideo&	sceneItemVideo = getSceneItemVideo();

	// Store
	mInternals->mGPU = OR<CGPU>(gpu);

	// Check if have resource filename
	const	OI<CString>&	resourceFilename = sceneItemVideo.getResourceFilename();
	if (resourceFilename.hasInstance())
		// Load
		mInternals->load(*resourceFilename);

	// Do super
	CSceneItemPlayer::load(gpu);

//	if (sceneItemVideo.getStartTimeInterval().hasValue() && (*sceneItemVideo.getStartTimeInterval() == 0.0)) {
//		// Start
////		mInternals->mIsStarted = true;
////
////		// Check for audio
//		if (mInternals->mMediaPlayer.hasInstance())
//			// Start
//			mInternals->mMediaPlayer->play();
////
////		// Check for started actions
////		const	OI<CActions>&	startedActions = sceneItemAnimation.getStartedActions();
////		if (startedActions.hasInstance())
////			// Perform
////			perform(*startedActions);
//	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerVideo::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	// Unload media player
	mInternals->mMediaPlayer = OI<CMediaPlayer>();
}

//----------------------------------------------------------------------------------------------------------------------
const OV<UniversalTimeInterval>& CSceneItemPlayerVideo::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemVideo().getStartTimeInterval();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerVideo::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	// Reset
	if (mInternals->mMediaPlayer.hasInstance())
		// Reset
		mInternals->mMediaPlayer->reset();

	mInternals->mIsStarted = false;
	mInternals->mCurrentTimeInterval = 0.0;

	// Super
	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerVideo::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	// Update
	mInternals->mCurrentTimeInterval += deltaTimeInterval;

	// Check if started
	if (!mInternals->mIsStarted && getSceneItemVideo().getStartTimeInterval().hasValue() &&
			(mInternals->mCurrentTimeInterval >= getSceneItemVideo().getStartTimeInterval().getValue(0.0))) {
		// Start
		if (mInternals->mMediaPlayer.hasInstance())
			// Start
			mInternals->mMediaPlayer->play();

		mInternals->mIsStarted = true;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerVideo::render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if can render
	if (mInternals->mRenderObject2D.hasInstance())
		// Render
		mInternals->mRenderObject2D->render(CGPURenderObject2D::Indexes::forIndex(0), renderInfo);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerVideo::handlesTouchOrMouseAtPoint(const S2DPointF32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerVideo::setProperty(const CString& property, const SValue& value)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check property name
	if (property == mFilenameProperty)
		// Filename
		mInternals->load(value.getString());
	else
		// Do super
		CSceneItemPlayer::setProperty(property, value);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerVideo::handleCommand(CGPU& gpu, const CString& command, const CDictionary& commandInfo,
		const S2DPointF32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check command
	if (command == mCommandNamePlay) {
		// Play
		if (mInternals->mMediaPlayer.hasInstance())
			// Play
			mInternals->mMediaPlayer->play();

		return true;
	} else if (command == mCommandNameStop) {
		// Stop
		if (mInternals->mMediaPlayer.hasInstance())
			// Reset
			mInternals->mMediaPlayer->reset();

		return true;
	} else
		// Unknown
		return false;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

////----------------------------------------------------------------------------------------------------------------------
//void sSceneAppMoviePlayerTouchBeganOrMouseDownAtPoint(CSceneAppMoviePlayer& sceneAppMoviePlayer,
//		const S2DPointF32& point, void* userData)
////----------------------------------------------------------------------------------------------------------------------
//{
//			CSceneItemPlayerMovieInternals*	internals = (CSceneItemPlayerMovieInternals*) userData;
//			CSceneItemPlayerMovie&			sceneItemPlayerMovie = internals->mSceneItemPlayerMovie;
//	const	TPtrArray<CSceneItemHotspot*>&		hotspotsArray = sceneItemPlayerMovie.getSceneItemMovie().getSceneHotspotsArray();
//	for (CArray::ItemIndex i = 0; i < hotspotsArray.getCount(); i++) {
//		CSceneItemHotspot*	hotspot = hotspotsArray[i];
//		if (hotspot->getScreenRect().contains(point)) {
//			sceneItemPlayerMovie.perform(*hotspot->getActionsOrNil(), point);
//
//			return;
//		}
//	}
//}
