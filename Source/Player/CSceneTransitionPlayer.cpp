//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneTransitionPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerInternals

class CSceneTransitionPlayerInternals {
	public:
		CSceneTransitionPlayerInternals(CScenePlayer& fromScenePlayer, CScenePlayer& toScenePlayer) :
				mFromScenePlayer(fromScenePlayer), mToScenePlayer(toScenePlayer), mState(kSceneTransitionStateActive)
			{}
		~CSceneTransitionPlayerInternals() {}

		CScenePlayer&			mFromScenePlayer;
		CScenePlayer&			mToScenePlayer;
		
		ESceneTransitionState	mState;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneTransitionPlayer

// MARK: Properties

CString	CSceneTransitionPlayer::mInfoDirectionUpValue("up");
CString	CSceneTransitionPlayer::mInfoDirectionDownValue("down");
CString	CSceneTransitionPlayer::mInfoDirectionLeftValue("left");
CString	CSceneTransitionPlayer::mInfoDirectionRightValue("right");


// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneTransitionPlayer::CSceneTransitionPlayer(CScenePlayer& fromScenePlayer, CScenePlayer& toScenePlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneTransitionPlayerInternals(fromScenePlayer, toScenePlayer);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneTransitionPlayer::~CSceneTransitionPlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer& CSceneTransitionPlayer::getFromScenePlayer() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mFromScenePlayer;
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer& CSceneTransitionPlayer::getToScenePlayer() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mToScenePlayer;
}

//----------------------------------------------------------------------------------------------------------------------
ESceneTransitionState CSceneTransitionPlayer::getState() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mState;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayer::setState(ESceneTransitionState state)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mState = state;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayer::update(UniversalTimeInterval deltaTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mFromScenePlayer.update(deltaTimeInterval);
	mInternals->mToScenePlayer.update(deltaTimeInterval);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayer::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayer::touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayer::touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayer::touchesOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

// MARK: Subclass methods

//----------------------------------------------------------------------------------------------------------------------
ESceneTransitionDirection CSceneTransitionPlayer::getSceneTransitionDirection(const CString& string)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check string
	if (string == mInfoDirectionUpValue)
		// Up
		return kSceneTransitionDirectionUp;
	else if (string == mInfoDirectionDownValue)
		// Down
		return kSceneTransitionDirectionDown;
	else if (string == mInfoDirectionLeftValue)
		// Left
		return kSceneTransitionDirectionLeft;
	else
		// Right
		return kSceneTransitionDirectionRight;
}
