//----------------------------------------------------------------------------------------------------------------------
//	CCelAnimationPlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CCelAnimationPlayer.h"

#include "CAnimationCel.h"
#include "CAnimationTexture.h"
#include "CSceneAppX.h"

#include "CGPUTextureManager.h"

#define LOG_TIMINGS	1

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

// Placeholders
typedef Float32 GLfloat;
enum DEF { kCGLTextureReferenceOptionsNone, kCGLTextureReferenceOptionWillChangeAlpha };
typedef UInt32 CGLTextureReferenceOptions;

enum ABC { GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_FLOAT, GL_BLEND, GL_TEXTURE_2D, GL_TRIANGLE_STRIP, };
static void glEnableClientState(ABC) {}
static void glVertexPointer(int, ABC, int, const GLfloat*) {}
static void glTexCoordPointer(int, ABC, int, const GLfloat*) {}
static void glEnable(ABC) {}
static void glColor4f(Float32, Float32, Float32, Float32) {}
static void glDrawArrays(ABC, int, int) {}
static void glDisableClientState(ABC) {}
static void glDisable(ABC) {}


static	UInt32							sFramesPerSecond = 60;
static	TPtrArray<CCelAnimationPlayer*>	sDeadPlayerArray;

static	bool							sRecordingTiming = false;
static	UniversalTimeInterval			sTotalTime = 0.0;
static	Float32							sIterations = 0.0;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CCelAnimationPlayerInternals

class CCelAnimationPlayerInternals {
	public:
		CCelAnimationPlayerInternals(CCelAnimationInfo& celAnimationInfo,
				const SCelAnimationPlayerProcsInfo& celAnimationPlayerProcsInfo, bool makeCopy,
				UniversalTimeInterval startTimeInterval) :
			mRenderMaterialImageReferences(true), mCelAnimationPlayerProcsInfo(celAnimationPlayerProcsInfo)
			{
				if (makeCopy) {
					mDisposeCelAnimationInfo = true;
					mCelAnimationInfo = celAnimationInfo.copy();
				} else {
					mDisposeCelAnimationInfo = false;
					mCelAnimationInfo = &celAnimationInfo;
				}

				mNeedToAddToDeadPlayerArray = false;
				mCurrentAnimationCel = nil;

				mIsStarted = false;
				mIsFinished = false;
				mCommonTexturesCelAnimationPlayer = nil;
				mCurrentTimeInterval = 0.0;
				mStartTimeInterval = startTimeInterval;
				mStartedTimeInterval = 0.0;
				mCurrentFrameIndex = 0;
				mCurrentLoopCount = 0;
			}
		~CCelAnimationPlayerInternals()
			{
				mRenderMaterialImageReferences.removeAll();

				if (mDisposeCelAnimationInfo) {
					// Dispose cel animation info
					Delete(mCelAnimationInfo);
				} else if (!mCelAnimationInfo->getKeepLoaded())
					// Unload cel animation info
					mCelAnimationInfo->unload();
			}

				bool								mDisposeCelAnimationInfo;
				CCelAnimationInfo*					mCelAnimationInfo;

				bool								mNeedToAddToDeadPlayerArray;
				TPtrArray<CGPUTextureReference*>	mRenderMaterialImageReferences;
				CAnimationCel*						mCurrentAnimationCel;

				bool								mIsStarted;
				bool								mIsFinished;
				CCelAnimationPlayer*				mCommonTexturesCelAnimationPlayer;
				UniversalTimeInterval				mCurrentTimeInterval;
				UniversalTimeInterval				mStartTimeInterval;
				UniversalTimeInterval				mStartedTimeInterval;
				UInt32								mCurrentFrameIndex;
				UInt32								mCurrentLoopCount;

		const	SCelAnimationPlayerProcsInfo&		mCelAnimationPlayerProcsInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CCelAnimationPlayer

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CCelAnimationPlayer::CCelAnimationPlayer(CCelAnimationInfo& celAnimationInfo,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SCelAnimationPlayerProcsInfo& celAnimationPlayerProcsInfo, bool makeCopy,
		UniversalTimeInterval startTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new CCelAnimationPlayerInternals(celAnimationInfo, celAnimationPlayerProcsInfo, makeCopy,
					startTimeInterval);
}

//----------------------------------------------------------------------------------------------------------------------
CCelAnimationPlayer::~CCelAnimationPlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mNeedToAddToDeadPlayerArray)
		sDeadPlayerArray += this;
		
	Delete(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CCelAnimationInfo& CCelAnimationPlayer::getCelAnimationInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return *mInternals->mCelAnimationInfo;
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::load(bool start)
//----------------------------------------------------------------------------------------------------------------------
{
//	if (!mInternals->mCelAnimationInfo->isLoaded())
//		mInternals->mCelAnimationInfo->loadFromURL(
//				eGetURLForResourceFilename(mInternals->mCelAnimationInfo->getResourceFilename()));
//	
//	CGLTextureReferenceOptions	textureOptions = kCGLTextureReferenceOptionsNone;
//	if (mInternals->mCelAnimationInfo->getOptions() & kCelAnimationInfoOptionsTextureWillChangeAlpha)
//		textureOptions |= kCGLTextureReferenceOptionWillChangeAlpha;

//	CAnimationTexture*	animationTexture;
//	if (mInternals->mCelAnimationInfo->getStorageOptions() & kCelAnimationInfoStorageOptionsCanBeStreamed) {
//		// Setup for streamed
//		// Load first cel
//		CAnimationCel*	animationCel = mInternals->mCelAnimationInfo->getAnimationCelAtFrameIndex(0);
//		(void) animationCel->getDrawOrigin();
//
//		// Load first texture
//		animationTexture = mInternals->mCelAnimationInfo->getAnimationTextureAtIndex(0);
//		mInternals->mRenderMaterialImageReferences +=
//				CRenderMaterialTextureManager::newRenderMaterialImageReference(animationTexture->getData(),
//				animationTexture->getSize(), animationTexture->getRenderMaterialImageDataFormat(),
//				(CGLTextureReferencePixelFormat) animationTexture->getPixelFormat(), textureOptions);
//	} else {
//		// Setup for loaded
//		for (UInt32 i = 0; i < mInternals->mCelAnimationInfo->getAnimationTextureCount(); i++) {
//			animationTexture = mInternals->mCelAnimationInfo->getAnimationTextureAtIndex(i);
//			mInternals->mRenderMaterialImageReferences +=
//					CRenderMaterialTextureManager::newRenderMaterialImageReference(animationTexture->getData(),
//							animationTexture->getSize(), animationTexture->getRenderMaterialImageDataFormat(),
//							(CGLTextureReferencePixelFormat) animationTexture->getPixelFormat(), textureOptions);
//		}
//	}

	reset(start);
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
//	for (CArrayItemIndex i = 0; i < mInternals->mRenderMaterialImageReferences.getCount(); i++)
//		mInternals->mRenderMaterialImageReferences[i]->finishLoading();
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	if (!mInternals->mCelAnimationInfo->getKeepLoaded())
		mInternals->mCelAnimationInfo->unload();
	mInternals->mRenderMaterialImageReferences.removeAll();
}

////----------------------------------------------------------------------------------------------------------------------
//bool CCelAnimationPlayer::getIsFullyLoaded() const
////----------------------------------------------------------------------------------------------------------------------
//{
////	if (mInternals->mRenderMaterialImageReferences.getCount() == 0)
//		return true;
////	else
////		return mInternals->mRenderMaterialImageReferences[mInternals->mRenderMaterialImageReferences.getCount() - 1]->isFullyLoaded();
//}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::reset(bool start)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsStarted = false;
	mInternals->mIsFinished = false;
	mInternals->mCurrentTimeInterval = 0.0;
	mInternals->mStartedTimeInterval = 0.0;
	mInternals->mCurrentFrameIndex = 0;
	mInternals->mCurrentLoopCount = 0;

	if (start)
		update(0.0, false);
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
//	mInternals->mCurrentTimeInterval += deltaTimeInterval;
//
//	if (!mInternals->mIsStarted && (mInternals->mCurrentTimeInterval >= mInternals->mStartTimeInterval)) {
//		mInternals->mIsStarted = true;
//		mInternals->mStartedTimeInterval = mInternals->mCurrentTimeInterval;
//	}
//
//	if (mInternals->mIsStarted && !mInternals->mIsFinished) {
//		// Next Frame
//				bool							foundFrame = false;
//				UInt32							animationFrameCount =
//														mInternals->mCelAnimationInfo->getAnimationFrameCount();
//				UInt32							startFrameIndex =
//														(mInternals->mCurrentTimeInterval -
//																mInternals->mStartedTimeInterval) *
//																(UniversalTimeInterval) sFramesPerSecond;
//				UInt32							loopCount = startFrameIndex / animationFrameCount;
//				UInt32							lastCurrentFrameIndex = mInternals->mCurrentFrameIndex;
//		const	SCelAnimationPlayerProcsInfo*	procsInfo = mInternals->mCelAnimationPlayerProcsInfo;
//
//		mInternals->mCurrentFrameIndex = startFrameIndex % animationFrameCount;
//		while (!foundFrame && !mInternals->mIsFinished) {
//			if (loopCount == mInternals->mCurrentLoopCount) {
//				// Get cel
//				mInternals->mCurrentAnimationCel =
//						mInternals->mCelAnimationInfo->getAnimationCelAtFrameIndex(mInternals->mCurrentFrameIndex);
//				foundFrame = true;
//			} else {
//				ECelAnimationPlayerFinishedAction	finishedAction;
//				if ((procsInfo != nil) && (procsInfo->mGetFinishedActionProc != nil))
//					finishedAction =
//							procsInfo->mGetFinishedActionProc(*this, mInternals->mCurrentLoopCount + 1,
//									procsInfo->mUserData);
//				else
//					finishedAction = kCelAnimationPlayerFinishedActionFinish;
//
//				switch (finishedAction) {
//					case kCelAnimationPlayerFinishedActionFinish:
//						// Finish
//						mInternals->mCurrentAnimationCel = nil;
//						mInternals->mIsFinished = true;
//
//						// Inform
//						if ((procsInfo != nil) && (procsInfo->mFinishedProc != nil)) {
//							// Do inform
//							mInternals->mNeedToAddToDeadPlayerArray = true;
//							procsInfo->mFinishedProc(*this, procsInfo->mUserData);
//
//							// Did we die?
//							if (sDeadPlayerArray.contains(this)) {
//								// Yes
//								sDeadPlayerArray -= this;
//
//								return;
//							}
//							mInternals->mNeedToAddToDeadPlayerArray = false;
//						}
//						break;
//
//					case kCelAnimationPlayerFinishedActionLoop:
//						// Loop
//						mInternals->mCurrentLoopCount++;
//
//						// Inform
//						if ((procsInfo != nil) && (procsInfo->mLoopingProc != nil)) {
//							// Do inform
//							mInternals->mNeedToAddToDeadPlayerArray = true;
//							procsInfo->mLoopingProc(*this, procsInfo->mUserData);
//
//							// Did we die?
//							if (sDeadPlayerArray.contains(this)) {
//								// Yes
//								sDeadPlayerArray -= this;
//
//								return;
//							}
//							mInternals->mNeedToAddToDeadPlayerArray = false;
//						}
//						break;
//
//					case kCelAnimationPlayerFinishedActionHoldLastFrame:
//						// Hold last frame
//						mInternals->mCurrentFrameIndex = animationFrameCount - 1;
//						mInternals->mCurrentAnimationCel =
//								mInternals->mCelAnimationInfo->getAnimationCelAtFrameIndex(
//										mInternals->mCurrentFrameIndex);
//						foundFrame = true;
//						break;
//				}
//			}
//		}
//
//		// Inform of frame change
//		if ((lastCurrentFrameIndex != mInternals->mCurrentFrameIndex) && (procsInfo != nil) &&
//				(procsInfo->mCurrentFrameIndexChangedProc != nil))
//			// Do inform
//			procsInfo->mCurrentFrameIndexChangedProc(*this, mInternals->mCurrentFrameIndex, procsInfo->mUserData);
//
//		if (mInternals->mCurrentAnimationCel != nil) {
//#if LOG_TIMINGS
//			UniversalTime	startTime = SUniversalTime::getCurrentUniversalTime();
//#endif
//
//			// Make sure cel is loaded
//			(void) mInternals->mCurrentAnimationCel->getDrawOrigin();
//
//			// We streaming?
//			if (mInternals->mCelAnimationInfo->getStorageOptions() & kCelAnimationInfoStorageOptionsCanBeStreamed) {
//				// Yes
//				// Dump current texture
//				mInternals->mRenderMaterialImageReferences.removeAll();
//
//				// Load next texture
//				CAnimationTexture*			animationTexture =
//													mInternals->mCelAnimationInfo->
//															getAnimationTextureAtIndex(mInternals->mCurrentFrameIndex);
//				CGLTextureReferenceOptions	textureOptions = kCGLTextureReferenceOptionsNone;
//				if (mInternals->mCelAnimationInfo->getOptions() & kCelAnimationInfoOptionsTextureWillChangeAlpha)
//					textureOptions |= kCGLTextureReferenceOptionWillChangeAlpha;
//
//				mInternals->mRenderMaterialImageReferences +=
//						CRenderMaterialTextureManager::newRenderMaterialImageReference(animationTexture->getData(),
//								animationTexture->getSize(), animationTexture->getRenderMaterialImageDataFormat(),
//								(CGLTextureReferencePixelFormat) animationTexture->getPixelFormat(),
//								textureOptions);
//
//#if LOG_TIMINGS
//				CLogServices::logMessage(CString(OSSTR("CCelAnimationPlayer load for ")) +
//						mInternals->mCelAnimationInfo->getResourceFilename() +
//						CString(OSSTR(", frame ")) + CString(mInternals->mCurrentFrameIndex) +
//						CString(OSSTR(", took ")) + CString(SUniversalTime::getCurrentUniversalTime() - startTime, 0, 3) +
//						CString(OSSTR("s")));
//#endif
//			}
//		}
//	}
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::render(CGPURenderEngine& gpuRenderEngine, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
////	#define	kArrayIndexCount	116000
//	#define	kArrayIndexCount	1116000
//
//	static	GLfloat	sVertices[kArrayIndexCount], sTextureUVs[kArrayIndexCount];
//	static	UInt32	sMaxArrayIndex = 0;
//
//	UniversalTime	startTime = 0.0;
//	if (sRecordingTiming)
//		startTime = SUniversalTime::getCurrentUniversalTime();
//
//	bool	glSetupDone = false;
//	if (mInternals->mIsStarted && !mInternals->mIsFinished && (mInternals->mCurrentAnimationCel != nil)) {
//		// General info
//		S2DPoint32	drawOrigin = mInternals->mCurrentAnimationCel->getDrawOrigin();
//		drawOrigin.mX += offset.mX;
//		drawOrigin.mY += offset.mY;
//
//		EAnimationCelRenderMethod	renderMethod = mInternals->mCurrentAnimationCel->getRenderMethod();
//
//		// Draw
//				CGLTextureReference*	textureReference;
//		const	UInt8*					dataPtr;
//		const	UInt8*					dataDonePtr;
//		if (renderMethod == kAnimationCelRenderMethodTextureBlocks) {
//			// Draw as Texture Blocks
//			// Get data
//			const	CData&	data = mInternals->mCurrentAnimationCel->getRenderTextureBlocksData();
//			dataPtr = (const UInt8*) data.getBytePtr();
//			dataDonePtr = dataPtr + data.getSize();
//
//			// Get Blocks Info
//			const	SAnimationCelRenderTextureBlocksInfo*	blocksInfo =
//																	(const SAnimationCelRenderTextureBlocksInfo*)
//																			dataPtr;
//			dataPtr += sizeof(SAnimationCelRenderTextureBlocksInfo);
//
//			// Get info
//			GLfloat	blockWidthPixels = blocksInfo->mBlockWidth;
//			GLfloat	blockHeightPixels = blocksInfo->mBlockHeight;
//
//			S2DPoint32	anchorPoint = mInternals->mCelAnimationInfo->getAnchorPoint();
//			S2DPoint32	screenPositionPoint = mInternals->mCelAnimationInfo->getScreenPositionPoint();
//			Float32		angle = mInternals->mCelAnimationInfo->getAngleDegrees();
//			Float32		alpha = mInternals->mCelAnimationInfo->getAlpha();
//			Float32		scale = mInternals->mCelAnimationInfo->getScale();
//
//			while (dataPtr < dataDonePtr) {
//				// Get texture info
//				const	SAnimationCelBlockTextureInfo*	textureInfo = (const SAnimationCelBlockTextureInfo*) dataPtr;
//				dataPtr += sizeof(SAnimationCelBlockTextureInfo);
//
//				// Does this texture have any blocks to draw?
//				if (textureInfo->mBlockCount == 0)
//					// No, skip
//					continue;
//
//				// Get texture
//				if (mInternals->mCelAnimationInfo->getStorageOptions() & kCelAnimationInfoStorageOptionsCanBeStreamed)
//					// Streaming, always first texture
//					textureReference = mInternals->mRenderMaterialImageReferences[0];
//				else if ((textureInfo->mTextureOptions & kAnimationCelTextureOptionsUseMask) ==
//						kAnimationCelTextureOptionsUseCommon)
//					// Loaded, get common texture
//					textureReference =
//							mInternals->mCommonTexturesCelAnimationPlayer->mInternals->mRenderMaterialImageReferences
//									.getAt(textureInfo->mTextureIndex);
//				else
//					// Loaded, get local texture
//					textureReference = mInternals->mRenderMaterialImageReferences[textureInfo->mTextureIndex];
//
//				// Make sure texture is loaded
//				textureReference->finishLoading();
//
//				// Let's go!
//				GLsizei	textureWidth, textureHeight;
//				textureReference->getTotalPixels(textureWidth, textureHeight);
//
//				UInt32	textureBlocksWide = (UInt32) textureWidth / blocksInfo->mBlockWidth;
//				GLfloat	uBlockWidth = (GLfloat) blocksInfo->mBlockWidth / textureWidth;
//				GLfloat	vBlockHeight = (GLfloat) blocksInfo->mBlockHeight / textureHeight;
//
//				UInt32	arrayIndex = 0, drawBlockIndex = 0;
//				for (UInt32 i = 0; i < textureInfo->mBlockCount; i++) {
//					// Get block info
//					const	SAnimationCelBlockInfo*	blockInfo = (const SAnimationCelBlockInfo*) dataPtr;
//					dataPtr += sizeof(SAnimationCelBlockInfo);
//
//					// What we doing?
//					if (blockInfo->mBlockOptions == kAnimationCelBlockOptionsSkipBlock)
//						// Skipping
//						drawBlockIndex += blockInfo->mBlockIndex;
//					else {
//						// Drawing
//						// Destination rect
//						GLfloat	drawX =
//										(drawBlockIndex % blocksInfo->mCelBlocksWide) * blockWidthPixels +
//												drawOrigin.mX;
//						GLfloat	drawY =
//										(drawBlockIndex / blocksInfo->mCelBlocksWide) * blockHeightPixels +
//												drawOrigin.mY;
//						drawBlockIndex++;
//
//						// Points are UL, UR, LL, LR
//						GLfloat	leftVertices[4], rightVertices[4];
//						if ((angle == 0.0) && (scale == 1.0)) {
//							// Draw straight
//							drawX += screenPositionPoint.mX - anchorPoint.mX;
//							drawY += screenPositionPoint.mY - anchorPoint.mY;
//							leftVertices[0] = drawX;
//							leftVertices[1] = drawY;
//
//							leftVertices[2] = drawX;
//							leftVertices[3] = drawY + blockHeightPixels;
//
//							rightVertices[0] = drawX + blockWidthPixels;
//							rightVertices[1] = drawY;
//
//							rightVertices[2] = drawX + blockWidthPixels;
//							rightVertices[3] = drawY + blockHeightPixels;
//						} else {
//							// Rotate around anchor point, then scale, then position on screen
//							Float32	a = -angle * kDegreesToRadiansConst;
//							Float32	cosA = cosf(a);
//							Float32	sinA = sinf(a);
//							Float32	dx, dy;
//
//							dx = drawX - anchorPoint.mX;
//							dy = drawY - anchorPoint.mY;
//							leftVertices[0] = (cosA * dx - sinA * dy) + screenPositionPoint.mX;
//							leftVertices[1] = (sinA * dx + cosA * dy) + screenPositionPoint.mY;
//
//							dx = drawX - anchorPoint.mX;
//							dy = drawY + blockHeightPixels - anchorPoint.mY;
//							leftVertices[2] = (cosA * dx - sinA * dy) + screenPositionPoint.mX;
//							leftVertices[3] = (sinA * dx + cosA * dy) + screenPositionPoint.mY;
//
//							dx = drawX + blockWidthPixels - anchorPoint.mX;
//							dy = drawY - anchorPoint.mY;
//							rightVertices[0] = (cosA * dx - sinA * dy) + screenPositionPoint.mX;
//							rightVertices[1] = (sinA * dx + cosA * dy) + screenPositionPoint.mY;
//
//							dx = drawX + blockWidthPixels - anchorPoint.mX;
//							dy = drawY + blockHeightPixels - anchorPoint.mY;
//							rightVertices[2] = (cosA * dx - sinA * dy) + screenPositionPoint.mX;
//							rightVertices[3] = (sinA * dx + cosA * dy) + screenPositionPoint.mY;
//						}
//
//						// Source UV rect
//						UInt32	textureBlockCol = blockInfo->mBlockIndex % textureBlocksWide;
//						UInt32	textureBlockRow = blockInfo->mBlockIndex / textureBlocksWide;
//
//						GLfloat	u = textureBlockCol * uBlockWidth;
//						GLfloat	v = textureBlockRow * vBlockHeight;
//
//						GLfloat	leftTextureUVs[4], rightTextureUVs[4];
//						switch (blockInfo->mBlockOptions & kAnimationCelBlockOptionsOrientMask) {
//							case kAnimationCelBlockOptionsOrientNormal:
//								//
//								//	1  3
//								//	2  4
//								leftTextureUVs[0] = u;
//								leftTextureUVs[1] = v;
//								leftTextureUVs[2] = u;
//								leftTextureUVs[3] = v + vBlockHeight;
//
//								rightTextureUVs[0] = u + uBlockWidth;
//								rightTextureUVs[1] = v;
//								rightTextureUVs[2] = u + uBlockWidth;
//								rightTextureUVs[3] = v + vBlockHeight;
//								break;
//
//							case kAnimationCelBlockOptionsOrientRotate90:
//								//
//								//	2  1
//								//	4  3
//								leftTextureUVs[0] = u;
//								leftTextureUVs[1] = v + vBlockHeight;
//								leftTextureUVs[2] = u + uBlockWidth;
//								leftTextureUVs[3] = v + vBlockHeight;
//
//								rightTextureUVs[0] = u;
//								rightTextureUVs[1] = v;
//								rightTextureUVs[2] = u + uBlockWidth;
//								rightTextureUVs[3] = v;
//								break;
//
//							case kAnimationCelBlockOptionsOrientRotate180:
//								//
//								//	4  2
//								//	3  1
//								leftTextureUVs[0] = u + uBlockWidth;
//								leftTextureUVs[1] = v + vBlockHeight;
//								leftTextureUVs[2] = u + uBlockWidth;
//								leftTextureUVs[3] = v;
//
//								rightTextureUVs[0] = u;
//								rightTextureUVs[1] = v + vBlockHeight;
//								rightTextureUVs[2] = u;
//								rightTextureUVs[3] = v;
//								break;
//
//							case kAnimationCelBlockOptionsOrientRotate270:
//								//
//								//	3  4
//								//	1  2
//								leftTextureUVs[0] = u + uBlockWidth;
//								leftTextureUVs[1] = v;
//								leftTextureUVs[2] = u;
//								leftTextureUVs[3] = v;
//
//								rightTextureUVs[0] = u + uBlockWidth;
//								rightTextureUVs[1] = v + vBlockHeight;
//								rightTextureUVs[2] = u;
//								rightTextureUVs[3] = v + vBlockHeight;
//								break;
//
//							case kAnimationCelBlockOptionsOrientNormalFlipLR:
//								//
//								//	3  1
//								//	4  2
//								leftTextureUVs[0] = u + uBlockWidth;
//								leftTextureUVs[1] = v;
//								leftTextureUVs[2] = u + uBlockWidth;
//								leftTextureUVs[3] = v + vBlockHeight;
//
//								rightTextureUVs[0] = u;
//								rightTextureUVs[1] = v;
//								rightTextureUVs[2] = u;
//								rightTextureUVs[3] = v + vBlockHeight;
//								break;
//
//							case kAnimationCelBlockOptionsOrientRotate90FlipLR:
//								//
//								//	1  2
//								//	3  4
//								leftTextureUVs[0] = u;
//								leftTextureUVs[1] = v;
//								leftTextureUVs[2] = u + uBlockWidth;
//								leftTextureUVs[3] = v;
//
//								rightTextureUVs[0] = u;
//								rightTextureUVs[1] = v + vBlockHeight;
//								rightTextureUVs[2] = u + uBlockWidth;
//								rightTextureUVs[3] = v + vBlockHeight;
//								break;
//
//							case kAnimationCelBlockOptionsOrientRotate180FlipLR:
//								//
//								//	2  4
//								//	1  3
//								leftTextureUVs[0] = u;
//								leftTextureUVs[1] = v + vBlockHeight;
//								leftTextureUVs[2] = u;
//								leftTextureUVs[3] = v;
//
//								rightTextureUVs[0] = u + uBlockWidth;
//								rightTextureUVs[1] = v + vBlockHeight;
//								rightTextureUVs[2] = u + uBlockWidth;
//								rightTextureUVs[3] = v;
//								break;
//
//							case kAnimationCelBlockOptionsOrientRotate270FlipLR:
//								//
//								// 4  3
//								// 2  1
//								leftTextureUVs[0] = u + uBlockWidth;
//								leftTextureUVs[1] = v + vBlockHeight;
//								leftTextureUVs[2] = u;
//								leftTextureUVs[3] = v + vBlockHeight;
//
//								rightTextureUVs[0] = u + uBlockWidth;
//								rightTextureUVs[1] = v;
//								rightTextureUVs[2] = u;
//								rightTextureUVs[3] = v;
//								break;
//						}
//
//						// Add vertices
//						size_t	size = sizeof(leftVertices);
//						if ((arrayIndex < 4) ||
//								memcmp(leftVertices, &sVertices[arrayIndex - 4], size) ||
//								memcmp(leftTextureUVs, &sTextureUVs[arrayIndex - 4], size)) {
//							if (arrayIndex > 0) {
//								// Add degenerate triangles
//								memcpy(&sVertices[arrayIndex], &sVertices[arrayIndex - 2], sizeof(GLfloat) * 2);
//								memcpy(&sTextureUVs[arrayIndex], &sTextureUVs[arrayIndex - 2], sizeof(GLfloat) * 2);
//								arrayIndex += 2;
//
//								memcpy(&sVertices[arrayIndex], leftVertices, sizeof(GLfloat) * 2);
//								memcpy(&sTextureUVs[arrayIndex], leftTextureUVs, sizeof(GLfloat) * 2);
//								arrayIndex += 2;
//							}
//
//							// Add new left side
//							memcpy(&sVertices[arrayIndex], leftVertices, sizeof(GLfloat) * 4);
//							memcpy(&sTextureUVs[arrayIndex], leftTextureUVs, sizeof(GLfloat) * 4);
//							arrayIndex += 4;
//						}
//
//						// Add right side
//						memcpy(&sVertices[arrayIndex], rightVertices, sizeof(GLfloat) * 4);
//						memcpy(&sTextureUVs[arrayIndex], rightTextureUVs, sizeof(GLfloat) * 4);
//						arrayIndex += 4;
//					}
//				}
//
//				// GL Setup
//				if (!glSetupDone) {
//					glEnableClientState(GL_VERTEX_ARRAY);
//					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//					glEnable(GL_BLEND);
//					glEnable(GL_TEXTURE_2D);
//					glSetupDone = true;
//				}
//
//				// Draw
//				textureReference->setupForRendering();
//				glVertexPointer(2, GL_FLOAT, 0, sVertices);
//				glTexCoordPointer(2, GL_FLOAT, 0, sTextureUVs);
//				glColor4f(1.0f, 1.0f, 1.0f, alpha);
//				glDrawArrays(GL_TRIANGLE_STRIP, 0, arrayIndex / 2);
//
//				if (arrayIndex > sMaxArrayIndex) {
//					sMaxArrayIndex = arrayIndex;
//					if (sMaxArrayIndex > kArrayIndexCount)
//						CLogServices::logMessage(
//								CString(OSSTR("******** UPDATE CODE ******* New max index: ")) +
//										CString(sMaxArrayIndex));
//					else
//						CLogServices::logMessage(CString(OSSTR("New max index: ")) + CString(sMaxArrayIndex));
//				}
//			}
//		} else {
//			// Draw as GL Data
//			// Get data
//			const	CData&	data = mInternals->mCurrentAnimationCel->getRenderTextureGLData();
//			dataPtr = (const UInt8*) data.getBytePtr();
//			dataDonePtr = dataPtr + data.getSize();
//
//			while (dataPtr < dataDonePtr) {
//				// Get texture info
//				const	SAnimationCelRenderGLDataInfo*	glDataInfo = (const SAnimationCelRenderGLDataInfo*) dataPtr;
//				dataPtr += sizeof(SAnimationCelRenderGLDataInfo);
//
//				// Get texture
//				if (mInternals->mCelAnimationInfo->getStorageOptions() & kCelAnimationInfoStorageOptionsCanBeStreamed)
//					// Streaming, always first texture
//					textureReference = mInternals->mRenderMaterialImageReferences[0];
//				else if ((glDataInfo->mTextureOptions & kAnimationCelTextureOptionsUseMask) ==
//						kAnimationCelTextureOptionsUseCommon)
//					// Loaded, get common texture
//					textureReference =
//							mInternals->mCommonTexturesCelAnimationPlayer->mInternals->mRenderMaterialImageReferences.
//									getAt(glDataInfo->mTextureIndex);
//				else
//					// Loaded, get local texture
//					textureReference = mInternals->mRenderMaterialImageReferences[glDataInfo->mTextureIndex];
//
//				// Make sure texture is loaded
//				textureReference->finishLoading();
//
//				// Get GL Data
//				CData	verticesData;
//				CData	textureUVsData;
//				if (renderMethod == kAnimationCelRenderMethodGLData) {
//					// Uncompressed GL Data
//					verticesData = CData(dataPtr, glDataInfo->mGLVerticesDataLength, false);
//					dataPtr += glDataInfo->mGLVerticesDataLength;
//
//					textureUVsData = CData(dataPtr, glDataInfo->mGLTextureUVsDataLength, false);
//					dataPtr += glDataInfo->mGLTextureUVsDataLength;
//				} else {
//					// ZIP Compressed GL Data
//					verticesData =
//							CData_ZIPExtensions::uncompressDataAsZIP(
//									CData(dataPtr, glDataInfo->mGLVerticesDataLength, false),
//											glDataInfo->mArrayIndexCount * sizeof(GLfloat));
//					dataPtr += glDataInfo->mGLVerticesDataLength;
//
//					textureUVsData =
//							CData_ZIPExtensions::uncompressDataAsZIP(
//									CData(dataPtr, glDataInfo->mGLTextureUVsDataLength, false),
//											glDataInfo->mArrayIndexCount * sizeof(GLfloat));
//					dataPtr += glDataInfo->mGLTextureUVsDataLength;
//				}
//
//				const	GLfloat*	vertices = (const GLfloat*) verticesData.getBytePtr();
//				const	GLfloat*	textureUVs = (const GLfloat*) textureUVsData.getBytePtr();
//
//				// GL Setup
//				if (!glSetupDone) {
//					glEnableClientState(GL_VERTEX_ARRAY);
//					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//					glEnable(GL_BLEND);
//					glEnable(GL_TEXTURE_2D);
//					glSetupDone = true;
//				}
//
//				// Draw
//				textureReference->setupForRendering();
//				glVertexPointer(2, GL_FLOAT, 0, vertices);
//				glTexCoordPointer(2, GL_FLOAT, 0, textureUVs);
//				glDrawArrays(GL_TRIANGLE_STRIP, 0, glDataInfo->mArrayIndexCount / 2);
//			}
//		}
//
//		mInternals->mCurrentAnimationCel->unload();
//	}
//
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisable(GL_BLEND);
//	glDisable(GL_TEXTURE_2D);
//
//	if (sRecordingTiming) {
//		UniversalTimeInterval	deltaTimeInterval = SUniversalTime::getCurrentUniversalTime() - startTime;
//		sTotalTime += deltaTimeInterval;
//		sIterations++;
//
//#if LOG_TIMINGS
//		CLogServices::logMessage(
//				CString(OSSTR("CCelAnimationPlayer - draw frame took ")) + CString(deltaTimeInterval, 0, 3) +
//						CString(OSSTR("s")));
//#endif
//	}
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::setCurrentFrameIndex(UInt32 currentFrameIndex)
//----------------------------------------------------------------------------------------------------------------------
{
//	mInternals->mIsStarted = true;
//	mInternals->mIsFinished = false;
//
//	// Inform of frame change
//	if ((currentFrameIndex != mInternals->mCurrentFrameIndex) &&
//			(mInternals->mCelAnimationPlayerProcsInfo != nil) &&
//			(mInternals->mCelAnimationPlayerProcsInfo->mCurrentFrameIndexChangedProc != nil))
//		mInternals->mCelAnimationPlayerProcsInfo->mCurrentFrameIndexChangedProc(*this, currentFrameIndex,
//				mInternals->mCelAnimationPlayerProcsInfo->mUserData);
//
//	mInternals->mCurrentFrameIndex = currentFrameIndex;
//	mInternals->mCurrentAnimationCel =
//			mInternals->mCelAnimationInfo->getAnimationCelAtFrameIndex(mInternals->mCurrentFrameIndex);
//#if LOG_TIMINGS
//	UniversalTime	startTime = SUniversalTime::getCurrentUniversalTime();
//#endif
//
//	// Make sure cel is loaded
//	(void) mInternals->mCurrentAnimationCel->getDrawOrigin();
//
//	// We streaming?
//	if (mInternals->mCelAnimationInfo->getStorageOptions() & kCelAnimationInfoStorageOptionsCanBeStreamed) {
//		// Yes
//		// Dump current texture
//		mInternals->mRenderMaterialImageReferences.removeAll();
//
//		// Load next texture
//		CAnimationTexture*			animationTexture =
//											mInternals->mCelAnimationInfo->getAnimationTextureAtIndex(
//													mInternals->mCurrentFrameIndex);
//		CGLTextureReferenceOptions	textureOptions = kCGLTextureReferenceOptionsNone;
//		if (mInternals->mCelAnimationInfo->getOptions() & kCelAnimationInfoOptionsTextureWillChangeAlpha)
//			textureOptions |= kCGLTextureReferenceOptionWillChangeAlpha;
//
//		mInternals->mRenderMaterialImageReferences +=
//				CRenderMaterialTextureManager::newRenderMaterialImageReference(animationTexture->getData(),
//				animationTexture->getTextureWidth(), animationTexture->getTextureHeight(),
//				animationTexture->getRenderMaterialImageDataFormat(),
//				(CGLTextureReferencePixelFormat) animationTexture->getPixelFormat(), textureOptions);
//
//#if LOG_TIMINGS
//		CLogServices::log(CString(OSSTR("CCelAnimationPlayer load for ")) +
//				mInternals->mCelAnimationInfo->getResourceFilename() +
//				CString(OSSTR(", frame ")) + CString(mInternals->mCurrentFrameIndex) +
//				CString(OSSTR(", took ")) + CString(SUniversalTime::getCurrentUniversalTime() - startTime, 0, 3) +
//				CString(OSSTR("s")));
//#endif
//	}
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::setCommonTexturesCelAnimationPlayer(CCelAnimationPlayer& commonTexturesCelAnimationPlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mCommonTexturesCelAnimationPlayer = &commonTexturesCelAnimationPlayer;
}

//----------------------------------------------------------------------------------------------------------------------
bool CCelAnimationPlayer::getIsFinished() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsFinished;
}

// MARK: Class methods

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::setFramesPerSecond(UInt32 framesPerSecond)
//----------------------------------------------------------------------------------------------------------------------
{
	sFramesPerSecond = framesPerSecond;
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::startTiming()
//----------------------------------------------------------------------------------------------------------------------
{
	sRecordingTiming = true;
	sTotalTime = 0.0;
	sIterations = 0.0;
}

//----------------------------------------------------------------------------------------------------------------------
void CCelAnimationPlayer::stopTimingAndLogResults()
//----------------------------------------------------------------------------------------------------------------------
{
	sRecordingTiming = false;

	CLogServices::logMessage(
			CString(OSSTR("CCelAnimationPlayer timing results - total time: ")) + CString(sTotalTime, 0, 2) +
					CString(OSSTR(", total iterations: ")) + CString((UInt32) sIterations) +
					CString(OSSTR(", average iteration time: ")) + CString(sTotalTime/ sIterations, 0, 2));
}
