//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayerPush.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneTransitionPlayerPush.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerPushInternals

class CSceneTransitionPlayerPushInternals {
	public:
		CSceneTransitionPlayerPushInternals(bool isAuto, ESceneTransitionDirection direction,
				UniversalTimeInterval durationTimeInterval,
				const SSceneTransitionPlayerProcsInfo& sceneTransitionPlayerProcsInfo) :
			mIsAuto(isAuto), mDirection(direction), mCurrentOffset(0.0),
					mSceneTransitionPlayerProcsInfo(sceneTransitionPlayerProcsInfo),
					mDurationTimeInterval(durationTimeInterval)
			{}

				bool								mIsAuto;
				ESceneTransitionDirection			mDirection;
				Float32								mCurrentOffset;
		const	SSceneTransitionPlayerProcsInfo&	mSceneTransitionPlayerProcsInfo;
				UniversalTimeInterval				mDurationTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneTransitionPlayerPush

// MARK: Properties

CString	CSceneTransitionPlayerPush::mInfoIsAutoKey(OSSTR("auto"));
CString	CSceneTransitionPlayerPush::mInfoDirectionKey(OSSTR("direction"));
CString	CSceneTransitionPlayerPush::mInfoDurationKey(OSSTR("duration"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneTransitionPlayerPush::CSceneTransitionPlayerPush(CScenePlayer& currentScenePlayer, CScenePlayer& nextScenePlayer,
		const CDictionary& info, const S2DPointF32& initialTouchOrMousePoint,
		const SSceneTransitionPlayerProcsInfo& sceneTransitionPlayerProcsInfo) :
		CSceneTransitionPlayer(currentScenePlayer, nextScenePlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new CSceneTransitionPlayerPushInternals(info.contains(mInfoIsAutoKey),
					getSceneTransitionDirection(info.getString(mInfoDirectionKey)),
					info.getFloat32(mInfoDurationKey, 0.3f), sceneTransitionPlayerProcsInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneTransitionPlayerPush::~CSceneTransitionPlayerPush()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: CSceneTransitionPlayer methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayerPush::update(UniversalTimeInterval deltaTimeInterval)
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
void CSceneTransitionPlayerPush::render(CGPU& gpu) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	S2DSizeF32	viewportPixelSize = mInternals->mSceneTransitionPlayerProcsInfo.getViewportSize();

	// Check direction
	switch (mInternals->mDirection) {
		case kSceneTransitionDirectionUp:
			// Up
			AssertFailUnimplemented();
			break;
		
		case kSceneTransitionDirectionDown:
			// Down
			AssertFailUnimplemented();
			break;
		
		case kSceneTransitionDirectionLeft:
			// Left
			getFromScenePlayer().render(gpu,
					CGPURenderObject::RenderInfo(S2DOffsetF32(-mInternals->mCurrentOffset, 0.0)));
			getToScenePlayer().render(gpu,
					CGPURenderObject::RenderInfo(
							S2DOffsetF32(-mInternals->mCurrentOffset + viewportPixelSize.mWidth, 0.0)));
			break;
		
		case kSceneTransitionDirectionRight:
			// Right
			getFromScenePlayer().render(gpu, CGPURenderObject::RenderInfo(S2DOffsetF32(mInternals->mCurrentOffset, 0.0)));
			getToScenePlayer().render(gpu,
					CGPURenderObject::RenderInfo(
							S2DOffsetF32(mInternals->mCurrentOffset - viewportPixelSize.mWidth, 0.0)));
			break;
	}
}
