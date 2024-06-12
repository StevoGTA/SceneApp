//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayerCoverUncover.cpp			©2020 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneTransitionPlayerCoverUncover.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerCoverUncover::Internals

class CSceneTransitionPlayerCoverUncover::Internals {
	public:
		Internals(bool isAuto, bool isCover, CSceneTransitionPlayer::Direction direction,
				UniversalTimeInterval durationTimeInterval, const CSceneTransitionPlayer::Procs& procs) :
			mIsAuto(isAuto), mIsCover(isCover), mDirection(direction), mCurrentOffset(0.0), mProcs(procs),
					mDurationTimeInterval(durationTimeInterval)
			{}

				bool								mIsAuto;
				bool								mIsCover;
				CSceneTransitionPlayer::Direction	mDirection;
				Float32								mCurrentOffset;
		const	CSceneTransitionPlayer::Procs&		mProcs;
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
		bool isCover, const Procs& procs) : CSceneTransitionPlayer(currentScenePlayer, nextScenePlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new Internals(info.contains(mInfoIsAutoKey), isCover, getDirection(info.getString(mInfoDirectionKey)),
					info.getFloat32(mInfoDurationKey, 0.3f), procs);
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
	CSceneTransitionPlayer::update(deltaTimeInterval);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayerCoverUncover::render(CGPU& gpu) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	S2DOffsetF32	offset;
	S2DSizeF32		viewportPixelSize = mInternals->mProcs.getViewportSize();

	// Check cover/uncover
	if (mInternals->mIsCover) {
		// Cover
		// Draw from scene
		getFromScenePlayer().render(gpu);

		// Draw to scene
		switch (mInternals->mDirection) {
			case kDirectionUp:
				// Up
				offset.mDY = viewportPixelSize.mHeight - mInternals->mCurrentOffset;
				break;

			case kDirectionDown:
				// Down
				offset.mDY = mInternals->mCurrentOffset - viewportPixelSize.mHeight;
				break;

			case kDirectionLeft:
				// Left
				offset.mDX = viewportPixelSize.mWidth - mInternals->mCurrentOffset;
				break;

			case kDirectionRight:
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
			case kDirectionUp:
				// Up
				offset.mDY = -mInternals->mCurrentOffset;
				break;

			case kDirectionDown:
				// Down
				offset.mDY = mInternals->mCurrentOffset;
				break;

			case kDirectionLeft:
				// Left
				offset.mDX = -mInternals->mCurrentOffset;
				break;

			case kDirectionRight:
				// Right
				offset.mDX = mInternals->mCurrentOffset;
				break;
		}
		getFromScenePlayer().render(gpu, CGPURenderObject::RenderInfo(offset));
	}
}
