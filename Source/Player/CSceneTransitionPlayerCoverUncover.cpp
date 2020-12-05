//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayerCoverUncover.cpp			©2020 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneTransitionPlayerCoverUncover.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerCoverUncoverInternals

class CSceneTransitionPlayerCoverUncoverInternals {
	public:
		CSceneTransitionPlayerCoverUncoverInternals(bool isAuto, bool isCover, ESceneTransitionDirection direction,
				UniversalTimeInterval durationTimeInterval,
				const SSceneTransitionPlayerProcsInfo& sceneTransitionPlayerProcsInfo) :
			mIsAuto(isAuto), mIsCover(isCover), mDirection(direction), mCurrentOffset(0.0),
					mSceneTransitionPlayerProcsInfo(sceneTransitionPlayerProcsInfo),
					mDurationTimeInterval(durationTimeInterval)
			{}

				bool								mIsAuto;
				bool								mIsCover;
				ESceneTransitionDirection			mDirection;
				Float32								mCurrentOffset;
		const	SSceneTransitionPlayerProcsInfo&	mSceneTransitionPlayerProcsInfo;
				UniversalTimeInterval				mDurationTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneTransitionPlayerCoverUncover

// MARK: Properties

CString	CSceneTransitionPlayerCoverUncover::mInfoIsAutoKey(OSSTR("auto"));
CString	CSceneTransitionPlayerCoverUncover::mInfoDirectionKey(OSSTR("direction"));
CString	CSceneTransitionPlayerCoverUncover::mInfoDurationKey(OSSTR("duration"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneTransitionPlayerCoverUncover::CSceneTransitionPlayerCoverUncover(CScenePlayer& currentScenePlayer,
		CScenePlayer& nextScenePlayer, const CDictionary& info, const S2DPointF32& initialTouchOrMousePoint,
		bool isCover, const SSceneTransitionPlayerProcsInfo& sceneTransitionPlayerProcsInfo) :
		CSceneTransitionPlayer(currentScenePlayer, nextScenePlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new CSceneTransitionPlayerCoverUncoverInternals(info.contains(mInfoIsAutoKey), isCover,
					getSceneTransitionDirection(info.getString(mInfoDirectionKey)),
					info.getFloat32(mInfoDurationKey, 0.3f), sceneTransitionPlayerProcsInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneTransitionPlayerCoverUncover::~CSceneTransitionPlayerCoverUncover()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: CSceneTransitionPlayer methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayerCoverUncover::update(UniversalTimeInterval deltaTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	S2DSizeF32	viewportPixelSize = mInternals->mSceneTransitionPlayerProcsInfo.getViewportSize();

	// Check for auto
	if (mInternals->mIsAuto) {
		// No user interaction
		switch (mInternals->mDirection) {
			case kSceneTransitionDirectionUp:
			case kSceneTransitionDirectionDown:
				// Up or Down
				mInternals->mCurrentOffset +=
						(Float32) (deltaTimeInterval / mInternals->mDurationTimeInterval) * viewportPixelSize.mHeight;
				if (mInternals->mCurrentOffset > viewportPixelSize.mHeight) {
					// Beyond bounds, done
					mInternals->mCurrentOffset = viewportPixelSize.mHeight;
					setState(kSceneTransitionStateCompleted);
				}
				break;

			case kSceneTransitionDirectionLeft:
			case kSceneTransitionDirectionRight:
				// Left or Right
				mInternals->mCurrentOffset +=
						(Float32) (deltaTimeInterval / mInternals->mDurationTimeInterval) * viewportPixelSize.mWidth;
				if (mInternals->mCurrentOffset > viewportPixelSize.mWidth) {
					// Beyond bounds, done
					mInternals->mCurrentOffset = viewportPixelSize.mWidth;
					setState(kSceneTransitionStateCompleted);
				}
				break;
		}
	} else {
		// User driven
AssertFailUnimplemented();
//		if (mDeltaOffset != 0.0f) {
//			mCurrentOffset += mDeltaOffset;
//			if (mCurrentOffset <= -viewportPixelSize.mWidth) {
//				mCurrentOffset = -viewportPixelSize.mWidth;
//				mState = kTransitionCompleted;
//			} else if (mCurrentOffset >= 0.0f) {
//				mCurrentOffset = 0.0f;
//				mState = kTransitionReset;
//			}
//		}
	}

	// Do super
	CSceneTransitionPlayer::update(deltaTimeInterval);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayerCoverUncover::render(CGPU& gpu) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	S2DOffsetF32	offset;
	S2DSizeF32		viewportPixelSize = mInternals->mSceneTransitionPlayerProcsInfo.getViewportSize();

	// Check cover/uncover
	if (mInternals->mIsCover) {
		// Cover
		// Draw from scene
		getFromScenePlayer().render(gpu);

		// Draw to scene
		switch (mInternals->mDirection) {
			case kSceneTransitionDirectionUp:
				// Up
				offset.mDY = viewportPixelSize.mHeight - mInternals->mCurrentOffset;
				break;

			case kSceneTransitionDirectionDown:
				// Down
				offset.mDY = mInternals->mCurrentOffset - viewportPixelSize.mHeight;
				break;

			case kSceneTransitionDirectionLeft:
				// Left
				offset.mDX = viewportPixelSize.mWidth - mInternals->mCurrentOffset;
				break;

			case kSceneTransitionDirectionRight:
				// Right
				offset.mDX = mInternals->mCurrentOffset - viewportPixelSize.mWidth;
				break;
		}
		getToScenePlayer().render(gpu, CGPURenderObject::RenderInfo(offset));
	} else {
		// Uncover
		// Draw to scene
		getToScenePlayer().render(gpu);

		// Draw from scene
		switch (mInternals->mDirection) {
			case kSceneTransitionDirectionUp:
				// Up
				offset.mDY = -mInternals->mCurrentOffset;
				break;

			case kSceneTransitionDirectionDown:
				// Down
				offset.mDY = mInternals->mCurrentOffset;
				break;

			case kSceneTransitionDirectionLeft:
				// Left
				offset.mDX = -mInternals->mCurrentOffset;
				break;

			case kSceneTransitionDirectionRight:
				// Right
				offset.mDX = mInternals->mCurrentOffset;
				break;
		}
		getFromScenePlayer().render(gpu, CGPURenderObject::RenderInfo(offset));
	}
}
