//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButtonArray.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerButtonArray.h"

#include "CGPURenderObject2D.h"
#include "CImage.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButtonArrayInternals

class CSceneItemPlayerButtonArrayInternals {
	public:
		CSceneItemPlayerButtonArrayInternals(const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo) :
			mGPURenderObject2D(nil), mActiveButtonDown(false),
					mSceneAppResourceManagementInfo(sceneAppResourceManagementInfo)
			{}
		~CSceneItemPlayerButtonArrayInternals()
			{
				DisposeOf(mGPURenderObject2D);
			}

				CGPURenderObject2D*					mGPURenderObject2D;

				OR<CSceneItemButtonArrayButton>		mActiveButton;
				bool								mActiveButtonDown;
				S2DRect32							mActiveButtonScreenRect;

		const	SSceneAppResourceManagementInfo&	mSceneAppResourceManagementInfo;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerButtonArray

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButtonArray::CSceneItemPlayerButtonArray(const CSceneItemButtonArray& sceneItemButtonArray,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
		CSceneItemPlayer(sceneItemButtonArray, sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemPlayerButtonArrayInternals(sceneAppResourceManagementInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButtonArray::~CSceneItemPlayerButtonArray()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
CActions CSceneItemPlayerButtonArray::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Collect all actions
	CActions	allActions;
	for (TIteratorD<CSceneItemButtonArrayButton> iterator =
					getSceneItemButtonArray().getSceneItemButtonArrayButtons().getIterator();
			iterator.hasValue(); iterator.advance()) {
		// Get actions for this button
		const	OO<CActions>&	actions = iterator.getValue().getActions();
		if (actions.hasObject())
			// Add to all actions
			allActions += *actions;
	}

	return allActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::load()
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if loaded
	if (mInternals->mGPURenderObject2D == nil)
		// Load
		mInternals->mGPURenderObject2D =
				new CGPURenderObject2D(
						mInternals->mSceneAppResourceManagementInfo.mGPUTextureManager.gpuTextureReference(
								mInternals->mSceneAppResourceManagementInfo.createByteParceller(
										getSceneItemButtonArray().getImageResourceFilename()),
								CImage::getBitmap, getSceneItemButtonArray().getImageResourceFilename()));

	// Do super
	CSceneItemPlayer::load();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mGPURenderObject2D);

	CSceneItemPlayer::unload();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	// Reset
	mInternals->mActiveButton = OR<CSceneItemButtonArrayButton>();
	mInternals->mActiveButtonDown = false;

	// Super
	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mGPURenderObject2D->getGPUTextureReference().finishLoading();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::render(CGPU& gpu, const S2DPoint32& offset) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all buttons
	for (TIteratorD<CSceneItemButtonArrayButton> iterator =
					getSceneItemButtonArray().getSceneItemButtonArrayButtons().getIterator();
			iterator.hasValue(); iterator.advance()) {
		// Render this button
		S2DPoint32	point = iterator.getValue().getScreenPositionPoint().offset(offset.mX, offset.mY);
		S2DRect32	rect =
							(!mInternals->mActiveButtonDown ||
											!mInternals->mActiveButton.hasReference() ||
											(&iterator.getValue() != &(*mInternals->mActiveButton))) ?
									iterator.getValue().getUpImageRect() : iterator.getValue().getDownImageRect();
		mInternals->mGPURenderObject2D->render(gpu, rect, point);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerButtonArray::handlesTouchOrMouseAtPoint(const S2DPoint32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all buttons
	for (TIteratorD<CSceneItemButtonArrayButton> iterator =
					getSceneItemButtonArray().getSceneItemButtonArrayButtons().getIterator();
			iterator.hasValue(); iterator.advance()) {
		// Do cover check
		S2DRect32	screenRect(iterator.getValue().getScreenPositionPoint(),
							iterator.getValue().getUpImageRect().mSize);
		if (screenRect.contains(point)) {
			// Got it
			mInternals->mActiveButton = OR<CSceneItemButtonArrayButton>(iterator.getValue());
			mInternals->mActiveButtonDown = false;
			mInternals->mActiveButtonScreenRect = screenRect;

			return true;
		}
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mActiveButtonDown = mInternals->mActiveButtonScreenRect.contains(point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mActiveButtonDown = mInternals->mActiveButtonScreenRect.contains(point2);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if active button is down
	if (mInternals->mActiveButtonDown) {
		// Collect actions
		OO<CActions>	actions = mInternals->mActiveButton->getActions();
		if (actions.hasObject())
			// Perform actions
			perform(*actions);
	}

	// Reset
	mInternals->mActiveButton = OR<CSceneItemButtonArrayButton>();
	mInternals->mActiveButtonDown = false;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::touchOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Reset
	mInternals->mActiveButton = OR<CSceneItemButtonArrayButton>();
	mInternals->mActiveButtonDown = false;
}
