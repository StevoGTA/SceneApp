//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerMovie.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerMovie.h"

#include "CSceneAppX.h"

#include "CNotificationCenterX.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

CString	CSceneItemPlayerMovie::mStopCommand(OSSTR("stop"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerMovieInternals

class CSceneItemPlayerMovieInternals {
	public:
		CSceneItemPlayerMovieInternals(CSceneItemPlayerMovie& sceneItemPlayerMovie);
		~CSceneItemPlayerMovieInternals();

		bool							mIsStarted;
		bool							mIsFinished;
		CSceneItemPlayerMovie&			mSceneItemPlayerMovie;
		CSceneAppMoviePlayer*			mSceneAppMoviePlayer;
		SSceneAppMoviePlayerProcsInfo	mSceneAppMoviePlayerProcsInfo;
		UniversalTimeInterval			mCurrentTimeInterval;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

static	CSceneItemPlayer*	sCreateSceneItemPlayer(const CSceneItem& sceneItem,
									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy);
static	void				sLoadAndPlayMovie(CSceneItemPlayerMovieInternals& internals,
									CSceneItemMovie& sceneItemMovie);
static	void				sSceneAppMoviePlayerTouchBeganOrMouseDownAtPoint(CSceneAppMoviePlayer& sceneAppMoviePlayer,
									const S2DPoint32& point, void* userData);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Register Scene Item Player

REGISTER_SCENE_ITEM_PLAYER(CSceneItemPlayerMovie, sCreateSceneItemPlayer, CSceneItemMovie::mType);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerMovieInternals

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerMovieInternals::CSceneItemPlayerMovieInternals(CSceneItemPlayerMovie& sceneItemPlayerMovie) :
		mSceneItemPlayerMovie(sceneItemPlayerMovie)
//----------------------------------------------------------------------------------------------------------------------
{
	mIsStarted = false;
	mIsFinished = false;
	mSceneAppMoviePlayer = nil;
	mCurrentTimeInterval = 0.0;

	mSceneAppMoviePlayerProcsInfo.mViewAddedToHierarchyProc = nil;
	mSceneAppMoviePlayerProcsInfo.mTouchBeganOrMouseDownAtPointProc = nil;
	mSceneAppMoviePlayerProcsInfo.mUserData = this;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerMovieInternals::~CSceneItemPlayerMovieInternals()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mSceneAppMoviePlayer);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerMovie

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerMovie::CSceneItemPlayerMovie(CSceneItemMovie& sceneItemMovie,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy) :
		CSceneItemPlayer(sceneItemMovie, sceneAppResourceManagementInfo, sceneItemPlayerProcsInfo, makeCopy)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemPlayerMovieInternals(*this);

	// Register for notifications
	CNotificationCenterX::registerHandler(MAKE_CUSTOM_EVENT_KIND(kSceneAppMoviePlayerMovieDidFinishEventKind), *this);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerMovie::~CSceneItemPlayerMovie()
//----------------------------------------------------------------------------------------------------------------------
{
	CNotificationCenterX::unregisterHandler(*this);

	Delete(mInternals);
}

// MARK: CEventHandler methods

//----------------------------------------------------------------------------------------------------------------------
UError CSceneItemPlayerMovie::handleEvent(CEvent& event)
//----------------------------------------------------------------------------------------------------------------------
{
	switch (event.getClassAndKind()) {
		case MAKE_CUSTOM_EVENT_KIND(kSceneAppMoviePlayerMovieDidFinishEventKind):
			// Movie finished
			mInternals->mIsFinished = true;

			if (getSceneItemMovie().getActionsOrNil() != nil)
				perform(*getSceneItemMovie().getActionsOrNil());

			return kNoError;
	}
	
	return kEventEventNotHandled;
}

// MARK: CSceneItemPlayer methods

////----------------------------------------------------------------------------------------------------------------------
//S2DRect32 CSceneItemPlayerMovie::getCurrentScreenRect() const
////----------------------------------------------------------------------------------------------------------------------
//{
//	return getSceneItemMovie().getScreenRect();
//}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerMovie::load()
//----------------------------------------------------------------------------------------------------------------------
{
	if (getSceneItemMovie().getStartTimeInterval() == kSceneItemStartTimeStartAtLoad)
		// Load and play movie
		sLoadAndPlayMovie(*mInternals, getSceneItemMovie());
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemPlayerMovie::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return getSceneItemMovie().getStartTimeInterval();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerMovie::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsStarted = false;
	mInternals->mIsFinished = false;
	mInternals->mCurrentTimeInterval = 0.0;

	// Super
	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerMovie::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	if (!mInternals->mIsStarted) {
		mInternals->mCurrentTimeInterval += deltaTimeInterval;
		if (mInternals->mCurrentTimeInterval >= getSceneItemMovie().getStartTimeInterval())
			mInternals->mIsStarted = true;
	}
	
	if (mInternals->mIsStarted && !mInternals->mIsFinished) {
		if (mInternals->mSceneAppMoviePlayer == nil)
			// Load and play movie
			sLoadAndPlayMovie(*mInternals, getSceneItemMovie());
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerMovie::handleCommand(const CString& command, const CDictionary& commandInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	if (command == mStopCommand)
		// Stop movie
		mInternals->mSceneAppMoviePlayer->stop();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CSceneAppMoviePlayer* CSceneItemPlayerMovie::getSceneAppMoviePlayerOrNil() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneAppMoviePlayer;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer* sCreateSceneItemPlayer(const CSceneItem& sceneItem,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy)
//----------------------------------------------------------------------------------------------------------------------
{
	return new CSceneItemPlayerMovie(*((CSceneItemMovie*) &sceneItem), sceneAppResourceManagementInfo,
			sceneItemPlayerProcsInfo, makeCopy);
}

//----------------------------------------------------------------------------------------------------------------------
void sLoadAndPlayMovie(CSceneItemPlayerMovieInternals& internals, CSceneItemMovie& sceneItemMovie)
//----------------------------------------------------------------------------------------------------------------------
{
	// Load
	internals.mSceneAppMoviePlayer =
			new CSceneAppMoviePlayer(eGetURLForResourceFilename(sceneItemMovie.getResourceFilename()));

	internals.mSceneAppMoviePlayer->setControlMode(sceneItemMovie.getControlMode());

	if (sceneItemMovie.getSceneHotspotsArray().getCount() > 0)
		internals.mSceneAppMoviePlayerProcsInfo.mTouchBeganOrMouseDownAtPointProc =
				sSceneAppMoviePlayerTouchBeganOrMouseDownAtPoint;

	if (sceneItemMovie.getInImageResourceFilename().getLength() > 0)
		internals.mSceneAppMoviePlayer->setBackgroundInImageURL(
				eGetURLForResourceFilename(sceneItemMovie.getInImageResourceFilename()));

	if (!sceneItemMovie.getScreenRect().isEmpty())
		internals.mSceneAppMoviePlayer->setScreenRect(sceneItemMovie.getScreenRect());

	// Play!
	internals.mSceneAppMoviePlayer->setSceneAppMoviePlayerProcsInfo(internals.mSceneAppMoviePlayerProcsInfo);
	internals.mSceneAppMoviePlayer->play();
}

//----------------------------------------------------------------------------------------------------------------------
void sSceneAppMoviePlayerTouchBeganOrMouseDownAtPoint(CSceneAppMoviePlayer& sceneAppMoviePlayer,
		const S2DPoint32& point, void* userData)
//----------------------------------------------------------------------------------------------------------------------
{
			CSceneItemPlayerMovieInternals*	internals = (CSceneItemPlayerMovieInternals*) userData;
			CSceneItemPlayerMovie&			sceneItemPlayerMovie = internals->mSceneItemPlayerMovie;
	const	TPtrArray<CSceneItemHotspot*>&		hotspotsArray = sceneItemPlayerMovie.getSceneItemMovie().getSceneHotspotsArray();
	for (CArrayItemIndex i = 0; i < hotspotsArray.getCount(); i++) {
		CSceneItemHotspot*	hotspot = hotspotsArray[i];
		if (hotspot->getScreenRect().contains(point)) {
			sceneItemPlayerMovie.perform(*hotspot->getActionsOrNil(), point);

			return;
		}
	}
}
