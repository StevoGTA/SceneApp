//----------------------------------------------------------------------------------------------------------------------
//	CScenePlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CScenePlayer.h"

#include "CLogServices.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	const	Float32	kHSwipeDragMinX = 12.0f;
static	const	Float32	kHSwipeDragMaxY = 4.0f;

static	const	Float32	kVSwipeDragMaxX = 4.0f;
static	const	Float32	kVSwipeDragMinY = 12.0f;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePlayerInternals

class CScenePlayerInternals {
	public:
										CScenePlayerInternals(const CScene& scene,
												const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
												const SScenePlayerProcsInfo& scenePlayerProcsInfo,
												CScenePlayer& scenePlayer) :
											mIsLoaded(false), mIsRunning(false), mScene(scene),
													mScenePlayer(scenePlayer),
													mSceneAppResourceManagementInfo(sceneAppResourceManagementInfo),
													mSceneItemPlayerProcsInfo(sceneItemPlayerActionsPerformProc,
															this),
													mScenePlayerProcsInfo(scenePlayerProcsInfo),
													mSceneItemPlayersArray(true)
											{}

				OR<CSceneItemPlayer>	getSceneItemPlayerForSceneItemWithName(const CString& itemName)
											{
												// Iterate all scene item players
												for (TIteratorS<CSceneItemPlayer*> iterator =
														mSceneItemPlayersArray.getIterator();
														iterator.hasValue(); iterator.advance()) {
													// Get info
													CSceneItemPlayer*	sceneItemPlayer = iterator.getValue();

													// Check if scene item matches
													if (sceneItemPlayer->getSceneItem().getName() == itemName)
														// Match
														return OR<CSceneItemPlayer>(*sceneItemPlayer);
												}

												return OR<CSceneItemPlayer>();
											}

		static	void					sceneItemPlayerActionsPerformProc(const CActions& actions,
												const S2DPoint32& point, void* userData)
											{
												// Setup
												CScenePlayerInternals*	internals = (CScenePlayerInternals*) userData;

												// Call proc
												internals->mScenePlayerProcsInfo.performActions(actions, point);
											}

				bool								mIsLoaded;
				bool								mIsRunning;
				TDictionary<CSceneItemPlayer*>		mTouchHandlingInfo;
		const	CScene&								mScene;
				CScenePlayer&						mScenePlayer;
				S2DPoint32							mInitialTouchPoint;
				S2DPoint32							mCurrentOffsetPoint;
		const	SSceneAppResourceManagementInfo&	mSceneAppResourceManagementInfo;
				SSceneItemPlayerProcsInfo			mSceneItemPlayerProcsInfo;
		const	SScenePlayerProcsInfo&				mScenePlayerProcsInfo;
				TPtrArray<CSceneItemPlayer*>		mSceneItemPlayersArray;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScenePlayer

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer::CScenePlayer(const CScene& scene, const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SScenePlayerProcsInfo& scenePlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CScenePlayerInternals(scene, sceneAppResourceManagementInfo, scenePlayerProcsInfo, *this);
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer::~CScenePlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const CScene& CScenePlayer::getScene() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScene;
}

//----------------------------------------------------------------------------------------------------------------------
CActions CScenePlayer::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	CActions	allActions;
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance())
		// Add actions
		allActions += iterator.getValue()->getAllActions();

	return allActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::load()
//----------------------------------------------------------------------------------------------------------------------
{
	// Ensure is loaded
	if (mInternals->mIsLoaded)
		return;

	// Iterate all scene items
	const	TArray<CSceneItem>&	sceneItems = mInternals->mScene.getSceneItems();
	for (UInt32 i = 0; i < sceneItems.getCount(); i++) {
		// Get scene
		CSceneItem&	sceneItem = sceneItems[i];

		// Create scene item player
		CSceneItemPlayer*	sceneItemPlayer =
									mInternals->mScenePlayerProcsInfo.createSceneItemPlayer(sceneItem,
											mInternals->mSceneAppResourceManagementInfo,
											mInternals->mSceneItemPlayerProcsInfo);
if (sceneItemPlayer == nil) {
CLogServices::logMessage(CString("No player created for ") + sceneItem.getType());
	continue;
}
		mInternals->mSceneItemPlayersArray += sceneItemPlayer;

		// Check if need to load
		if (!(sceneItem.getOptions() & kSceneItemOptionsDontLoadWithScene))
			// Load
			sceneItemPlayer->load();
	}

	// Iterate all created scene item players
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance())
		// Inform scene item player that all peers have loaded
		iterator.getValue()->allPeersHaveLoaded();

	// Update
	mInternals->mIsLoaded = true;
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	// Ensure is not loaded
	if (!mInternals->mIsLoaded)
		return;
	CLogServices::logMessage(CString(OSSTR("Unloading ")) + CString(this));

	// Iterate all scene item players
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance())
		// Unload
		iterator.getValue()->unload();

	// Remove all
	mInternals->mSceneItemPlayersArray.removeAll();

	// No longer loaded
	mInternals->mIsLoaded = false;
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::start()
//----------------------------------------------------------------------------------------------------------------------
{
	// Is running
	mInternals->mIsRunning = true;

	// Update all scene item players to get them into their initial state
	update(0.0);
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if running
	if (mInternals->mIsRunning) {
		// No longer running
		mInternals->mIsRunning = false;

		// Iterate all scene item players
		for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator();
				iterator.hasValue(); iterator.advance())
			// Reset
			iterator.getValue()->reset();
	}
	
	mInternals->mCurrentOffsetPoint = S2DPoint32();
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::update(UniversalTimeInterval deltaTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance()) {
		// Get scene item player
		CSceneItemPlayer*	sceneItemPlayer = iterator.getValue();

		// Check if need to update
		if ((mInternals->mIsRunning || !sceneItemPlayer->getStartTimeInterval().hasValue()) &&
				sceneItemPlayer->getIsVisible())
			// Update
			sceneItemPlayer->update(deltaTimeInterval, mInternals->mIsRunning);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::render(CGPU& gpu, const S2DPoint32& offset) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	S2DPoint32	totalOffsetPoint = mInternals->mCurrentOffsetPoint.offset(offset.mX, offset.mY);

	// Iterate all scene item players
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance()) {
		// Get scene item player
		CSceneItemPlayer*	sceneItemPlayer = iterator.getValue();

		// Check if is visible
		if (sceneItemPlayer->getIsVisible()) {
			// Check how this scene item is anchored
			if (sceneItemPlayer->getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen)
				// Draw anchored to screen
				sceneItemPlayer->render(gpu, offset);
			else
				// Draw anchored to bg
				sceneItemPlayer->render(gpu, totalOffsetPoint);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::setItemPlayerProperty(const CString& itemName, const CString& property,
		const SDictionaryValue& value) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve scene item player
	OR<CSceneItemPlayer>	sceneItemPlayer = mInternals->getSceneItemPlayerForSceneItemWithName(itemName);
	if (sceneItemPlayer.hasReference())
		// Make the call
		sceneItemPlayer->setProperty(property, value);
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::handleItemPlayerCommand(const CString& itemName, const CString& command,
		const CDictionary& commandInfo, const S2DPoint32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve scene item player
	OR<CSceneItemPlayer>	sceneItemPlayer = mInternals->getSceneItemPlayerForSceneItemWithName(itemName);
	if (sceneItemPlayer.hasReference())
		// Make the call
		sceneItemPlayer->handleCommand(command, commandInfo, point);
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mInitialTouchPoint = point;

	// Check for 2 taps/clicks and have double tap action
	if ((tapOrClickCount == 2) && mInternals->mScene.getDoubleTapActions().hasObject())
		// 2 taps or clicks and have double tap action
		mInternals->mScenePlayerProcsInfo.performActions(*mInternals->mScene.getDoubleTapActions());
	else {
		// Setup
		S2DPoint32	offsetPoint(point.mX - mInternals->mCurrentOffsetPoint.mX,
							point.mY - mInternals->mCurrentOffsetPoint.mY);

		// Search for target scene item player from front to back
		CSceneItemPlayer*	targetSceneItemPlayer = nil;
		for (CArrayItemIndex i = mInternals->mSceneItemPlayersArray.getCount(); i > 0; i--) {
			// Get next scene item player
			CSceneItemPlayer*	sceneItemPlayer = mInternals->mSceneItemPlayersArray[i - 1];

			// Check if visible
			if (!sceneItemPlayer->getIsVisible())
				// Not visible
				continue;

			// Compose test point
			S2DPoint32	testPoint =
								(sceneItemPlayer->getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen) ?
										point : offsetPoint;
			if (sceneItemPlayer->handlesTouchOrMouseAtPoint(testPoint)) {
				// Found it
				targetSceneItemPlayer = sceneItemPlayer;
				break;
			}
		}

		// Check if found scene item player
		if (targetSceneItemPlayer == nil)
			// Nothing found
			return;

		// Store
		mInternals->mTouchHandlingInfo.set(CString(reference), targetSceneItemPlayer);

		// Prepare to adjust point if needed
		S2DPoint32	adjustedPoint = point;
		if ((targetSceneItemPlayer->getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen) == 0) {
			adjustedPoint.mX -= mInternals->mCurrentOffsetPoint.mX;
			adjustedPoint.mY -= mInternals->mCurrentOffsetPoint.mY;
		}

		// Pass to scene item player
		targetSceneItemPlayer->touchBeganOrMouseDownAtPoint(adjustedPoint, tapOrClickCount, reference);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve target scene item player
	OV<CSceneItemPlayer*>	targetSceneItemPlayer = mInternals->mTouchHandlingInfo[CString(reference)];

	// Check if touch move has become a swipe
	if (targetSceneItemPlayer.hasValue() && !mInternals->mScene.getBoundsRect().isEmpty() &&
			(((fabsf(point2.mX - mInternals->mInitialTouchPoint.mX) >= kHSwipeDragMinX) &&
					(fabsf(point2.mY - mInternals->mInitialTouchPoint.mY) <= kHSwipeDragMaxY)) ||
			((fabsf(point2.mX - mInternals->mInitialTouchPoint.mX) <= kVSwipeDragMaxX) &&
					(fabsf(point2.mY - mInternals->mInitialTouchPoint.mY) >= kVSwipeDragMinY)))) {
		// Touch move has become a swipe
		(*targetSceneItemPlayer)->touchOrMouseCancelled(reference);
		targetSceneItemPlayer = OV<CSceneItemPlayer*>();
		mInternals->mTouchHandlingInfo.remove(CString(reference));

		// Update our current offset
		mInternals->mCurrentOffsetPoint.mX += point1.mX - mInternals->mInitialTouchPoint.mX;
		mInternals->mCurrentOffsetPoint.mY += point1.mY - mInternals->mInitialTouchPoint.mY;
	}

	// Handle
	if (targetSceneItemPlayer.hasValue()) {
		// Prepare to adjust point if needed
		S2DPoint32	adjustedPoint1 = point1;
		S2DPoint32	adjustedPoint2 = point2;
		if (((*targetSceneItemPlayer)->getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen) == 0) {
			// Adjust point
			adjustedPoint1.mX -= mInternals->mCurrentOffsetPoint.mX;
			adjustedPoint1.mY -= mInternals->mCurrentOffsetPoint.mY;
			adjustedPoint2.mX -= mInternals->mCurrentOffsetPoint.mX;
			adjustedPoint2.mY -= mInternals->mCurrentOffsetPoint.mY;
		}

		// Pass to scene item player
		(*targetSceneItemPlayer)->touchOrMouseMovedFromPoint(adjustedPoint1, adjustedPoint2, reference);
	} else if (!mInternals->mScene.getBoundsRect().isEmpty()) {
		// Adjust current offset point
		mInternals->mCurrentOffsetPoint.mX += point2.mX - point1.mX;
		mInternals->mCurrentOffsetPoint.mY += point2.mY - point1.mY;
		
		// Bound
		S2DSize32	viewportSize = mInternals->mScenePlayerProcsInfo.getViewportSize();
		Float32		minX = viewportSize.mWidth - mInternals->mScene.getBoundsRect().getMaxX();
		Float32		maxX = mInternals->mScene.getBoundsRect().mOrigin.mX;
		if (mInternals->mCurrentOffsetPoint.mX < minX)
			mInternals->mCurrentOffsetPoint.mX = minX;
		else if (mInternals->mCurrentOffsetPoint.mX > maxX)
			mInternals->mCurrentOffsetPoint.mX = maxX;

		Float32		minY = viewportSize.mHeight - mInternals->mScene.getBoundsRect().getMaxY();
		Float32		maxY = mInternals->mScene.getBoundsRect().mOrigin.mY;
		if (mInternals->mCurrentOffsetPoint.mY < minY)
			mInternals->mCurrentOffsetPoint.mY = minY;
		else if (mInternals->mCurrentOffsetPoint.mY > maxY)
			mInternals->mCurrentOffsetPoint.mY = maxY;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve target scene item player
	OV<CSceneItemPlayer*>	targetSceneItemPlayer = mInternals->mTouchHandlingInfo[CString(reference)];

	// Check if have scene item player
	if (targetSceneItemPlayer.hasValue()) {
		// Send to Scene Item Player
		S2DPoint32	adjustedPoint = point;
		if (((*targetSceneItemPlayer)->getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen) == 0) {
			adjustedPoint.mX -= mInternals->mCurrentOffsetPoint.mX;
			adjustedPoint.mY -= mInternals->mCurrentOffsetPoint.mY;
		}

		// Pass to scene item player
		(*targetSceneItemPlayer)->touchEndedOrMouseUpAtPoint(adjustedPoint, reference);

		// Cleanup
		mInternals->mTouchHandlingInfo.remove(CString(reference));
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve target scene item player
	OV<CSceneItemPlayer*>	targetSceneItemPlayer = mInternals->mTouchHandlingInfo[CString(reference)];

	// Check if have scene item player
	if (targetSceneItemPlayer.hasValue()) {
		// Pass to scene item player
		(*targetSceneItemPlayer)->touchOrMouseCancelled(reference);

		// Cleanup
		mInternals->mTouchHandlingInfo.remove(CString(reference));
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::shakeBegan()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance())
		// Pass to scene item player
		iterator.getValue()->shakeBegan();
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::shakeEnded()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance())
		// Pass to scene item player
		iterator.getValue()->shakeEnded();
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::shakeCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance())
		// Pass to scene item player
		iterator.getValue()->shakeCancelled();
}
