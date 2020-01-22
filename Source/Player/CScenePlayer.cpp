//----------------------------------------------------------------------------------------------------------------------
//	CScenePlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CScenePlayer.h"

#include "CScene.h"
#include "CLogServices.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	const	Float32	kHSwipeDragMinX = 12.0f;
static	const	Float32	kHSwipeDragMaxY = 4.0f;

static	const	Float32	kVSwipeDragMaxX = 4.0f;
static	const	Float32	kVSwipeDragMinY = 12.0f;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

//static	CImageX				sSceneItemPlayerGetCurrentViewportImage(const CSceneItemPlayer& sceneItemPlayer,
//										bool performRedraw, void* userData);

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
													mSceneItemPlayerProcsInfo(sceneItemPlayerActionArrayPerformProc,
//															sceneItemPlayerCreateSceneItemPlayerForSceneItemProc,
//															sceneItemPlayerAddSceneItemPlayerProc,
//															sceneItemPlayerGetPeerForSceneItemName,
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

		static	void					sceneItemPlayerActionArrayPerformProc(const CActionArray& actionArray,
												const S2DPoint32& point, void* userData)
											{
												// Setup
												CScenePlayerInternals*	internals = (CScenePlayerInternals*) userData;

												// Call proc
												internals->mScenePlayerProcsInfo.mActionArrayPerformProc(actionArray,
														point, internals->mScenePlayerProcsInfo.mUserData);
											}
//		static	OV<CSceneItemPlayer*>	sceneItemPlayerCreateSceneItemPlayerForSceneItemProc(
//												const CSceneItem& sceneItem, bool autoLoad, void* userData)
//											{
//												// Setup
//												CScenePlayerInternals*	internals = (CScenePlayerInternals*) userData;
//
//												// Create scene item player
//												OV<CSceneItemPlayer*>	sceneItemPlayer =
//																				internals->mScenePlayerProcsInfo.
//																						mCreateSceneItemPlayerProc(
//																						sceneItem,
//																						internals->
//																								mSceneAppResourceManagementInfo,
//																						internals->
//																								mSceneItemPlayerProcsInfo,
//																						internals->
//																								mScenePlayerProcsInfo.mUserData);
//												if (sceneItemPlayer.hasValue()) {
//													// Add to array
//													internals->mSceneItemPlayersArray += *sceneItemPlayer;
//
//													// Check if need to load
//													if (autoLoad && internals->mIsLoaded)
//														// Load
//														(*sceneItemPlayer)->load();
//												}
//
//												return sceneItemPlayer;
//											}
//
//		static	void 					sceneItemPlayerAddSceneItemPlayerProc(CSceneItemPlayer& sceneItemPlayer,
//												bool autoLoad, void* userData)
//											{
//												// Setup
//												CScenePlayerInternals*	internals = (CScenePlayerInternals*) userData;
//
//												// Add to array
//												internals->mSceneItemPlayersArray += &sceneItemPlayer;
//
//												// Check if need to load
//												if (autoLoad && internals->mIsLoaded)
//													// Load
//													sceneItemPlayer.load();
//											}
//
//		static	OR<CSceneItemPlayer>	sceneItemPlayerGetPeerForSceneItemName(CSceneItemPlayer& sceneItemPlayer,
//												const CString& sceneItemName, void* userData)
//											{
//												// Setup
//												CScenePlayerInternals*	internals = (CScenePlayerInternals*) userData;
//
//												return internals->getSceneItemPlayerForSceneItemWithName(sceneItemName);
//											}

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
CActionArray CScenePlayer::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CActionArray	allActionArray;

	// Iterate all scene item players
	for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
			iterator.advance())
		// Add actions
		allActionArray += iterator.getValue()->getAllActions();

	return allActionArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::load()
//----------------------------------------------------------------------------------------------------------------------
{
	// Ensure is loaded
	if (mInternals->mIsLoaded)
		return;
	// Iterate all scene items
	for (UInt32 i = 0; i < mInternals->mScene.getSceneItemsCount(); i++) {
		// Get scene
		CSceneItem&	sceneItem = mInternals->mScene.getSceneItemAtIndex(i);

		// Try to create scene item player
		OV<CSceneItemPlayer*>	sceneItemPlayer =
										mInternals->mScenePlayerProcsInfo.mCreateSceneItemPlayerProc(sceneItem,
												mInternals->mSceneAppResourceManagementInfo,
												mInternals->mSceneItemPlayerProcsInfo,
												mInternals->mScenePlayerProcsInfo.mUserData);
		if (sceneItemPlayer.hasValue()) {
			// Yes
			mInternals->mSceneItemPlayersArray += *sceneItemPlayer;

			// Check if need to load
			if (!(sceneItem.getOptions() & kSceneItemOptionsDontLoadWithScene))
				// Load
				(*sceneItemPlayer)->load();
		}
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
	CLogServices::logMessage(CString("Unloading ") + CString(this));

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
CLogServices::logMessage(CString("Resetting ") + CString(this));
	// Check if running
	if (mInternals->mIsRunning) {
		// No longer running
		mInternals->mIsRunning = false;

		// Iterate all scene item players
		for (TIteratorS<CSceneItemPlayer*> iterator = mInternals->mSceneItemPlayersArray.getIterator(); iterator.hasValue();
				iterator.advance())
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
		if ((mInternals->mIsRunning || (sceneItemPlayer->getStartTimeInterval() == kSceneItemStartTimeStartAtLoad)) &&
				sceneItemPlayer->getIsVisible())
			// Update
			sceneItemPlayer->update(deltaTimeInterval, mInternals->mIsRunning);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::render(CGPU& gpu, const S2DPoint32& offset)
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

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* CScenePlayer::getSceneItemPlayerOrNilForItemName(const CString& itemName) const
////----------------------------------------------------------------------------------------------------------------------
//{
//	for (CArrayItemIndex i = 0; i < mInternals->mSceneItemPlayersArray.getCount(); i++) {
//		CSceneItemPlayer*	sceneItemPlayer = mInternals->mSceneItemPlayersArray[i];
//		CSceneItem*			sceneItem = sceneItemPlayer->getSceneItemOrNil();
//		if ((sceneItem != nil) && (sceneItem->getName() == itemName))
//			return sceneItemPlayer;
//	}
//
//	return nil;
//}

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

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* CScenePlayer::getSceneItemPlayerOrNilForTouchOrMouseAtPoint(const S2DPoint32& point) const
////----------------------------------------------------------------------------------------------------------------------
//{
//	S2DPoint32	offsetPoint(point.mX - mInternals->mCurrentOffsetPoint.mX,
//						point.mY - mInternals->mCurrentOffsetPoint.mY);
//
//	// Search players from front to back
//	for (CArrayItemIndex i = mInternals->mSceneItemPlayersArray.getCount(); i > 0; i--) {
//		CSceneItemPlayer*	sceneItemPlayer = mInternals->mSceneItemPlayersArray[i - 1];
//		if (!sceneItemPlayer->getIsVisible())
//			continue;
//
//		S2DPoint32	testPoint =
//							((sceneItemPlayer->getSceneItemOrNil() != nil) &&
//									(sceneItemPlayer->getSceneItemOrNil()->getOptions() &
//											kSceneItemOptionsAnchorToScreen)) ?
//							point : offsetPoint;
//		if (sceneItemPlayer->handlesTouchOrMouseAtPoint(testPoint))
//			// Found it
//			return sceneItemPlayer;
//	}
//
//	return nil;
//}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mInitialTouchPoint = point;

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
		S2DSize32	viewportSize =
							mInternals->mScenePlayerProcsInfo.mGetViewportSizeProc(
									mInternals->mScenePlayerProcsInfo.mUserData);
		Float32	minX = viewportSize.mWidth - mInternals->mScene.getBoundsRect().getMaxX();
		Float32	maxX = mInternals->mScene.getBoundsRect().mOrigin.mX;
		if (mInternals->mCurrentOffsetPoint.mX < minX)
			mInternals->mCurrentOffsetPoint.mX = minX;
		else if (mInternals->mCurrentOffsetPoint.mX > maxX)
			mInternals->mCurrentOffsetPoint.mX = maxX;

		Float32	minY = viewportSize.mHeight - mInternals->mScene.getBoundsRect().getMaxY();
		Float32	maxY = mInternals->mScene.getBoundsRect().mOrigin.mY;
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

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* CScenePlayer::createAndAddSceneItemPlayerForSceneItem(const CSceneItem& sceneItem, bool makeCopy,
//		bool autoLoad)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return sSceneItemPlayerCreateSceneItemPlayerForSceneItemProc(sceneItem, makeCopy, autoLoad, mInternals);
//}

////----------------------------------------------------------------------------------------------------------------------
//void CScenePlayer::addSceneItemPlayer(CSceneItemPlayer& sceneItemPlayer, bool autoLoad)
////----------------------------------------------------------------------------------------------------------------------
//{
//	sSceneItemPlayerAddSceneItemPlayerProc(sceneItemPlayer, autoLoad, mInternals);
//}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

////----------------------------------------------------------------------------------------------------------------------
//CImageX sSceneItemPlayerGetCurrentViewportImage(const CSceneItemPlayer& sceneItemPlayer, bool performRedraw,
//		void* userData)
////----------------------------------------------------------------------------------------------------------------------
//{
//	CScenePlayerInternals*	internals = (CScenePlayerInternals*) userData;
//
//	return internals->mScenePlayerProcsInfo->mGetCurrentViewportImageProc(
//			internals->mScenePlayer, performRedraw, internals->mScenePlayerProcsInfo->mUserData);
//}
