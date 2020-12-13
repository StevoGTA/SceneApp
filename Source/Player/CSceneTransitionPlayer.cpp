//----------------------------------------------------------------------------------------------------------------------
//	CSceneTransitionPlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneTransitionPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneTransitionPlayerInternals

class CSceneTransitionPlayerInternals {
	public:
		CSceneTransitionPlayerInternals(CScenePlayer& fromScenePlayer, CScenePlayer& toScenePlayer) :
				mFromScenePlayer(fromScenePlayer), mToScenePlayer(toScenePlayer),
				mState(CSceneTransitionPlayer::kStateActive)
			{}

		CScenePlayer&					mFromScenePlayer;
		CScenePlayer&					mToScenePlayer;
		
		CSceneTransitionPlayer::State	mState;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneTransitionPlayer

// MARK: Properties

CString	CSceneTransitionPlayer::mInfoDirectionUpValue(OSSTR("up"));
CString	CSceneTransitionPlayer::mInfoDirectionDownValue(OSSTR("down"));
CString	CSceneTransitionPlayer::mInfoDirectionLeftValue(OSSTR("left"));
CString	CSceneTransitionPlayer::mInfoDirectionRightValue(OSSTR("right"));

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
	Delete(mInternals);
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
CSceneTransitionPlayer::State CSceneTransitionPlayer::getState() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mState;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneTransitionPlayer::setState(State state)
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

// MARK: Subclass methods

//----------------------------------------------------------------------------------------------------------------------
CSceneTransitionPlayer::Direction CSceneTransitionPlayer::getDirection(const CString& string)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check string
	if (string == mInfoDirectionUpValue)
		// Up
		return kDirectionUp;
	else if (string == mInfoDirectionDownValue)
		// Down
		return kDirectionDown;
	else if (string == mInfoDirectionLeftValue)
		// Left
		return kDirectionLeft;
	else
		// Right
		return kDirectionRight;
}
