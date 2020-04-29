//----------------------------------------------------------------------------------------------------------------------
//	CSceneAppMoviePlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "C2DGeometry.h"
#include "CEvent.h"
#include "CURLX.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneAppMoviePlayerProcsInfo

class CSceneAppMoviePlayer;
typedef	void	(*CSceneAppMoviePlayerViewAddedToHierarchyProc)(CSceneAppMoviePlayer& sceneAppMoviePlayer,
						void* userData);
typedef	void	(*CSceneAppMoviePlayerTouchBeganOrMouseDownAtPointProc)(CSceneAppMoviePlayer& sceneAppMoviePlayer,
						const S2DPoint32& point, void* userData);

struct SSceneAppMoviePlayerProcsInfo {
	CSceneAppMoviePlayerViewAddedToHierarchyProc			mViewAddedToHierarchyProc;
	CSceneAppMoviePlayerTouchBeganOrMouseDownAtPointProc	mTouchBeganOrMouseDownAtPointProc;
	void*													mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Notifications

const	OSType	kSceneAppMoviePlayerMovieWillStartEventKind	= MAKE_OSTYPE('M', 'P', 'l', '1');
/*
	Sent when a movie will start playing.
*/

const	OSType	kSceneAppMoviePlayerMovieDidFinishEventKind	= MAKE_OSTYPE('M', 'P', 'l', '2');
/*
	Sent when a movie has finished playing.
*/

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Control Mode

enum ESceneAppMoviePlayerControlMode {
	kSceneAppMoviePlayerControlModeHidden,
	kSceneAppMoviePlayerControlModeAllControls,

	kSceneAppMoviePlayerControlModeDefault = kSceneAppMoviePlayerControlModeHidden,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneAppMoviePlayer

class CSceneAppMoviePlayerInternals;
class CSceneAppMoviePlayer : public CEventHandler {
	// Methods
	public:
						// Lifecycle methods
						CSceneAppMoviePlayer(const CURLX& URL);
		virtual			~CSceneAppMoviePlayer();

						// CEventHandler methods
		virtual	UError	handleEvent(CEvent& event);

						// Instance methods
				void	setControlMode(ESceneAppMoviePlayerControlMode controlMode);
				void	setBackgroundInImageURL(const CURLX& inImageURL);
				void	setScreenRect(const S2DRect32& screenRect);
				
				void	play();
				void	stop();

				void	setSceneAppMoviePlayerProcsInfo(
								const SSceneAppMoviePlayerProcsInfo& sceneAppMoviePlayerProcsInfo);

	// Properties
	private:
		CSceneAppMoviePlayerInternals*	mInternals;
};
