//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerMovie.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CSceneAppMoviePlayer.h"
#include "CSceneItemMovie.h"
#include "CSceneItemPlayer.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerMovie

class CSceneItemPlayerMovieInternals;
class CSceneItemPlayerMovie : public CSceneItemPlayer {
	// Methods
	public:
										// Lifecycle methods
										CSceneItemPlayerMovie(const CSceneItemMovie& sceneItemMovie,
												const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
												const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
										~CSceneItemPlayerMovie();

										// CEventHandler methods
				UError					handleEvent(CEvent& event);

										// CSceneItemPlayer methods
//				S2DRect32				getCurrentScreenRect() const;

				void					load();

				UniversalTimeInterval	getStartTimeInterval() const;

				void					reset();
				void					update(UniversalTimeInterval deltaTimeInterval, bool isRunning);

				void					handleCommand(const CString& command, const CDictionary& commandInfo);

										// Instance methods
		const	CSceneItemMovie&		getSceneItemMovie() const
											{ return (CSceneItemMovie&) getSceneItem(); }
				CSceneAppMoviePlayer*	getSceneAppMoviePlayerOrNil() const;

	// Properties
	public:
		static	CString							mStopCommand;

	private:
				CSceneItemPlayerMovieInternals*	mInternals;
};
