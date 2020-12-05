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

class CScenePlayerInternals : public TReferenceCountable<CScenePlayerInternals> {
	public:
										CScenePlayerInternals(const CScene& scene,
												const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
												const SScenePlayerProcsInfo& scenePlayerProcsInfo,
												CScenePlayer& scenePlayer) :
											TReferenceCountable(), mIsLoaded(false), mIsRunning(false), mScene(scene),
													mScenePlayer(scenePlayer),
													mSceneAppResourceManagementInfo(sceneAppResourceManagementInfo),
													mSceneItemPlayerProcsInfo(sceneItemPlayerActionsPerformProc,
															this),
													mScenePlayerProcsInfo(scenePlayerProcsInfo)
											{}

				OR<CSceneItemPlayer>	getSceneItemPlayerForSceneItemWithName(const CString& itemName)
											{
												// Iterate all scene item players
												for (TIteratorD<CSceneItemPlayer> iterator =
														mSceneItemPlayers.getIterator();
														iterator.hasValue(); iterator.advance()) {
													// Get info
													CSceneItemPlayer&	sceneItemPlayer = iterator.getValue();

													// Check if scene item matches
													if (sceneItemPlayer.getSceneItem().getName() == itemName)
														// Match
														return OR<CSceneItemPlayer>(sceneItemPlayer);
												}

												return OR<CSceneItemPlayer>();
											}

		static	void					sceneItemPlayerActionsPerformProc(const CActions& actions,
												const S2DPointF32& point, void* userData)
											{
												// Setup
												CScenePlayerInternals*	internals = (CScenePlayerInternals*) userData;

												// Call proc
												internals->mScenePlayerProcsInfo.performActions(actions, point);
											}

				bool									mIsLoaded;
				bool									mIsRunning;
				TReferenceDictionary<CSceneItemPlayer>	mTouchHandlingInfo;
		const	CScene&									mScene;
				CScenePlayer&							mScenePlayer;
				S2DPointF32								mInitialTouchPoint;
				S2DOffsetF32							mCurrentOffset;
		const	SSceneAppResourceManagementInfo&		mSceneAppResourceManagementInfo;
				SSceneItemPlayerProcsInfo				mSceneItemPlayerProcsInfo;
		const	SScenePlayerProcsInfo&					mScenePlayerProcsInfo;
				TIArray<CSceneItemPlayer>				mSceneItemPlayers;
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
CScenePlayer::CScenePlayer(const CScenePlayer& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CScenePlayer::~CScenePlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
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
	for (TIteratorD<CSceneItemPlayer> iterator = mInternals->mSceneItemPlayers.getIterator(); iterator.hasValue();
			iterator.advance())
		// Add actions
		allActions += iterator.getValue().getAllActions();

	return allActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::load(CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if already loaded
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
			// No SceneItemPlayer
#if !defined(DEBUG)
			AssertFailUnimplemented();
#else
			CLogServices::logMessage(CString("No player created for ") + sceneItem.getType());
			continue;
#endif
		}
		mInternals->mSceneItemPlayers += sceneItemPlayer;

		// Check if need to load
		if (!(sceneItem.getOptions() & kSceneItemOptionsDontLoadWithScene))
			// Load
			sceneItemPlayer->load(gpu);
	}

	// Iterate all created scene item players
	for (TIteratorD<CSceneItemPlayer> iterator = mInternals->mSceneItemPlayers.getIterator(); iterator.hasValue();
			iterator.advance())
		// Inform scene item player that all peers have loaded
		iterator.getValue().allPeersHaveLoaded();

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

	// Remove all scene item players
	mInternals->mSceneItemPlayers.removeAll();

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
		for (TIteratorD<CSceneItemPlayer> iterator = mInternals->mSceneItemPlayers.getIterator();
				iterator.hasValue(); iterator.advance())
			// Reset
			iterator.getValue().reset();
	}
	
	mInternals->mCurrentOffset = S2DOffsetF32();
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::update(UniversalTimeInterval deltaTimeInterval)
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	for (TIteratorD<CSceneItemPlayer> iterator = mInternals->mSceneItemPlayers.getIterator(); iterator.hasValue();
			iterator.advance()) {
		// Get scene item player
		CSceneItemPlayer&	sceneItemPlayer = iterator.getValue();

		// Check if need to update
		if ((mInternals->mIsRunning || !sceneItemPlayer.getStartTimeInterval().hasValue()) &&
				sceneItemPlayer.getIsVisible())
			// Update
			sceneItemPlayer.update(deltaTimeInterval, mInternals->mIsRunning);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CGPURenderObject::RenderInfo	offsetRenderInfo = renderInfo.offset(mInternals->mCurrentOffset);

	// Iterate all scene item players
	for (TIteratorD<CSceneItemPlayer> iterator = mInternals->mSceneItemPlayers.getIterator(); iterator.hasValue();
			iterator.advance()) {
		// Get scene item player
		CSceneItemPlayer&	sceneItemPlayer = iterator.getValue();

		// Check if is visible
		if (sceneItemPlayer.getIsVisible()) {
			// Check how this scene item is anchored
			if (sceneItemPlayer.getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen)
				// Draw anchored to screen
				sceneItemPlayer.render(gpu, renderInfo);
			else
				// Draw anchored to bg
				sceneItemPlayer.render(gpu, offsetRenderInfo);
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
void CScenePlayer::handleItemPlayerCommand(CGPU& gpu, const CString& itemName, const CString& command,
		const CDictionary& commandInfo, const S2DPointF32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve scene item player
	OR<CSceneItemPlayer>	sceneItemPlayer = mInternals->getSceneItemPlayerForSceneItemWithName(itemName);
	if (sceneItemPlayer.hasReference())
		// Make the call
		sceneItemPlayer->handleCommand(gpu, command, commandInfo, point);
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Store
	mInternals->mInitialTouchPoint = point;

	// Check for 2 taps/clicks and have double tap action
	if ((tapOrClickCount == 2) && mInternals->mScene.getDoubleTapActions().hasInstance())
		// 2 taps or clicks and have double tap action
		mInternals->mScenePlayerProcsInfo.performActions(*mInternals->mScene.getDoubleTapActions());
	else {
		// Setup
		S2DPointF32	scenePoint = point.offset(mInternals->mCurrentOffset.inverted());

		// Search for target scene item player from front to back
		CSceneItemPlayer*	targetSceneItemPlayer = nil;
		for (CArray::ItemIndex i = mInternals->mSceneItemPlayers.getCount(); i > 0; i--) {
			// Get next scene item player
			CSceneItemPlayer&	sceneItemPlayer = mInternals->mSceneItemPlayers[i - 1];

			// Check if visible
			if (!sceneItemPlayer.getIsVisible())
				// Not visible
				continue;

			// Compose scene item player point
			S2DPointF32	sceneItemPlayerPoint =
								(sceneItemPlayer.getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen) ?
										point : scenePoint;
			if (sceneItemPlayer.handlesTouchOrMouseAtPoint(sceneItemPlayerPoint)) {
				// Found it
				targetSceneItemPlayer = &sceneItemPlayer;
				break;
			}
		}

		// Check if found scene item player
		if (targetSceneItemPlayer == nil)
			// Nothing found
			return;

		// Store
		mInternals->mTouchHandlingInfo.set(CString(reference), *targetSceneItemPlayer);

		// Compose scene item player point
		S2DPointF32	sceneItemPlayerPoint =
							(targetSceneItemPlayer->getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen) ?
									point : scenePoint;

		// Pass to scene item player
		targetSceneItemPlayer->touchBeganOrMouseDownAtPoint(sceneItemPlayerPoint, tapOrClickCount, reference);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve target scene item player
	OR<CSceneItemPlayer>	targetSceneItemPlayer = mInternals->mTouchHandlingInfo[CString(reference)];

	// Check if touch move has become a swipe
	if (targetSceneItemPlayer.hasReference() && !mInternals->mScene.getBoundsRect().isEmpty() &&
			(((fabsf(point2.mX - mInternals->mInitialTouchPoint.mX) >= kHSwipeDragMinX) &&
					(fabsf(point2.mY - mInternals->mInitialTouchPoint.mY) <= kHSwipeDragMaxY)) ||
			((fabsf(point2.mX - mInternals->mInitialTouchPoint.mX) <= kVSwipeDragMaxX) &&
					(fabsf(point2.mY - mInternals->mInitialTouchPoint.mY) >= kVSwipeDragMinY)))) {
		// Touch move has become a swipe
		targetSceneItemPlayer->touchOrMouseCancelled(reference);
		targetSceneItemPlayer = OR<CSceneItemPlayer>();
		mInternals->mTouchHandlingInfo.remove(CString(reference));

		// Update our current offset
		mInternals->mCurrentOffset = mInternals->mCurrentOffset + S2DOffsetF32(mInternals->mInitialTouchPoint, point1);
	}

	// Handle
	if (targetSceneItemPlayer.hasReference()) {
		// Prepare to adjust point if needed
		S2DPointF32	adjustedPoint1 = point1;
		S2DPointF32	adjustedPoint2 = point2;
		if ((targetSceneItemPlayer->getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen) == 0) {
			// Adjust points
			adjustedPoint1 = adjustedPoint1.offset(mInternals->mCurrentOffset.inverted());
			adjustedPoint2 = adjustedPoint2.offset(mInternals->mCurrentOffset.inverted());
		}

		// Pass to scene item player
		targetSceneItemPlayer->touchOrMouseMovedFromPoint(adjustedPoint1, adjustedPoint2, reference);
	} else if (!mInternals->mScene.getBoundsRect().isEmpty()) {
		// Adjust current offset
		mInternals->mCurrentOffset = mInternals->mCurrentOffset + S2DOffsetF32(point1, point2);

		// Bound
				S2DSizeF32	viewportSize = mInternals->mScenePlayerProcsInfo.getViewportSize();
		const	S2DRectF32&	sceneBoundsRect = mInternals->mScene.getBoundsRect();
		mInternals->mCurrentOffset =
				mInternals->mCurrentOffset.bounded(
						viewportSize.mWidth - sceneBoundsRect.getMaxX(), sceneBoundsRect.mOrigin.mX,
						viewportSize.mHeight - sceneBoundsRect.getMaxY(), sceneBoundsRect.mOrigin.mY);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve target scene item player
	OR<CSceneItemPlayer>	targetSceneItemPlayer = mInternals->mTouchHandlingInfo[CString(reference)];

	// Check if have scene item player
	if (targetSceneItemPlayer.hasReference()) {
		// Send to Scene Item Player
		S2DPointF32	adjustedPoint = point;
		if ((targetSceneItemPlayer->getSceneItem().getOptions() & kSceneItemOptionsAnchorToScreen) == 0)
			// Offset
			adjustedPoint = adjustedPoint.offset(mInternals->mCurrentOffset.inverted());

		// Pass to scene item player
		targetSceneItemPlayer->touchEndedOrMouseUpAtPoint(adjustedPoint, reference);

		// Cleanup
		mInternals->mTouchHandlingInfo.remove(CString(reference));
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::touchOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Retrieve target scene item player
	OR<CSceneItemPlayer>	targetSceneItemPlayer = mInternals->mTouchHandlingInfo[CString(reference)];

	// Check if have scene item player
	if (targetSceneItemPlayer.hasReference()) {
		// Pass to scene item player
		targetSceneItemPlayer->touchOrMouseCancelled(reference);

		// Cleanup
		mInternals->mTouchHandlingInfo.remove(CString(reference));
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::shakeBegan()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	for (TIteratorD<CSceneItemPlayer> iterator = mInternals->mSceneItemPlayers.getIterator(); iterator.hasValue();
			iterator.advance())
		// Pass to scene item player
		iterator.getValue().shakeBegan();
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::shakeEnded()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	for (TIteratorD<CSceneItemPlayer> iterator = mInternals->mSceneItemPlayers.getIterator(); iterator.hasValue();
			iterator.advance())
		// Pass to scene item player
		iterator.getValue().shakeEnded();
}

//----------------------------------------------------------------------------------------------------------------------
void CScenePlayer::shakeCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all scene item players
	for (TIteratorD<CSceneItemPlayer> iterator = mInternals->mSceneItemPlayers.getIterator(); iterator.hasValue();
			iterator.advance())
		// Pass to scene item player
		iterator.getValue().shakeCancelled();
}
