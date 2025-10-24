//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayerPush.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneTransitionPlayerPush.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerPush::Internals

class CSceneTransitionPlayerPush::Internals {
	public:
		Internals(bool isAuto, CSceneTransitionPlayer::Direction direction, UniversalTimeInterval durationTimeInterval,
				const CSceneTransitionPlayer::Procs& procs) :
			mIsAuto(isAuto), mDirection(direction), mCurrentOffset(0.0), mProcs(procs),
					mDurationTimeInterval(durationTimeInterval)
			{}

				bool								mIsAuto;
				CSceneTransitionPlayer::Direction	mDirection;
				Float32								mCurrentOffset;
		const	CSceneTransitionPlayer::Procs&		mProcs;
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
		const CDictionary& info, const S2DPointF32& initialTouchOrMousePoint, const Procs& procs) :
		CSceneTransitionPlayer(currentScenePlayer, nextScenePlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new Internals(info.contains(mInfoIsAutoKey), getDirection(info.getString(mInfoDirectionKey)),
					info.getFloat32(mInfoDurationKey, 0.3f), procs);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneTransitionPlayerPush::~CSceneTransitionPlayerPush()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: CSceneTransitionPlayer methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayerPush::update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	S2DSizeF32	viewportPixelSize = mInternals->mProcs.getViewportSize();

	// Check for auto
	if (mInternals->mIsAuto) {
		// No user interaction
		switch (mInternals->mDirection) {
			case kDirectionUp:
			case kDirectionDown:
				// Up or Down
				mInternals->mCurrentOffset +=
						(Float32) (deltaTimeInterval / mInternals->mDurationTimeInterval) * viewportPixelSize.mHeight;
				if (mInternals->mCurrentOffset > viewportPixelSize.mHeight) {
					// Beyond bounds, done
					mInternals->mCurrentOffset = viewportPixelSize.mHeight;
					setState(kStateCompleted);
				}
				break;

			case kDirectionLeft:
			case kDirectionRight:
				// Left or Right
				mInternals->mCurrentOffset +=
						(Float32) (deltaTimeInterval / mInternals->mDurationTimeInterval) * viewportPixelSize.mWidth;
				if (mInternals->mCurrentOffset > viewportPixelSize.mWidth) {
					// Beyond bounds, done
					mInternals->mCurrentOffset = viewportPixelSize.mWidth;
					setState(kStateCompleted);
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
	CSceneTransitionPlayer::update(gpu, deltaTimeInterval);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayerPush::render(CGPU& gpu) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	S2DSizeF32	viewportPixelSize = mInternals->mProcs.getViewportSize();

	// Check direction
	switch (mInternals->mDirection) {
		case kDirectionUp:
			// Up
			AssertFailUnimplemented();
			break;
		
		case kDirectionDown:
			// Down
			AssertFailUnimplemented();
			break;
		
		case kDirectionLeft:
			// Left
			getFromScenePlayer().render(gpu,
					CGPURenderObject::RenderInfo(S2DOffsetF32(-mInternals->mCurrentOffset, 0.0)));
			getToScenePlayer().render(gpu,
					CGPURenderObject::RenderInfo(
							S2DOffsetF32(-mInternals->mCurrentOffset + viewportPixelSize.mWidth, 0.0)));
			break;
		
		case kDirectionRight:
			// Right
			getFromScenePlayer().render(gpu, CGPURenderObject::RenderInfo(S2DOffsetF32(mInternals->mCurrentOffset, 0.0)));
			getToScenePlayer().render(gpu,
					CGPURenderObject::RenderInfo(
							S2DOffsetF32(mInternals->mCurrentOffset - viewportPixelSize.mWidth, 0.0)));
			break;
	}
}
