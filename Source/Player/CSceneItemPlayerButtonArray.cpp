//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButtonArray.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerButtonArray.h"

#include "CGPURenderObject2D.h"
#include "CImage.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButtonArray::Internals

class CSceneItemPlayerButtonArray::Internals {
	public:
		Internals(const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo) :
			mSceneAppResourceManagementInfo(sceneAppResourceManagementInfo),
					mGPURenderObject2D(nil),
					mActiveButtonDown(false)
			{}
		~Internals()
			{
				Delete(mGPURenderObject2D);
			}

		SSceneAppResourceManagementInfo	mSceneAppResourceManagementInfo;

		CGPURenderObject2D*				mGPURenderObject2D;

		OR<CSceneItemButtonArrayButton>	mActiveButton;
		bool							mActiveButtonDown;
		S2DRectF32						mActiveButtonScreenRect;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerButtonArray

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButtonArray::CSceneItemPlayerButtonArray(const CSceneItemButtonArray& sceneItemButtonArray,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo, const Procs& procs) :
		CSceneItemPlayer(sceneItemButtonArray, procs)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new Internals(sceneAppResourceManagementInfo);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButtonArray::~CSceneItemPlayerButtonArray()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
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
		const	OI<CActions>&	actions = iterator->getActions();
		if (actions.hasInstance())
			// Add to all actions
			allActions += *actions;
	}

	return allActions;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::load(CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if loaded
	if (mInternals->mGPURenderObject2D == nil) {
		// Compose items
		TNArray<CGPURenderObject2D::Item>	items;
		for (TIteratorD<CSceneItemButtonArrayButton> iterator =
						getSceneItemButtonArray().getSceneItemButtonArrayButtons().getIterator();
				iterator.hasValue(); iterator.advance()) {
			// Get info
			const	CSceneItemButtonArrayButton&	button = *iterator;
			const	S2DPointF32&					screenPositionPoint = button.getScreenPositionPoint();
			const	S2DRectF32&						upImageRect = button.getUpImageRect();

			// Add up image info
			items += CGPURenderObject2D::Item(S2DRectF32(screenPositionPoint, upImageRect.mSize), 0, upImageRect);
		}
		for (TIteratorD<CSceneItemButtonArrayButton> iterator =
						getSceneItemButtonArray().getSceneItemButtonArrayButtons().getIterator();
				iterator.hasValue(); iterator.advance()) {
			// Get info
			const	CSceneItemButtonArrayButton&	button = *iterator;
			const	S2DPointF32&					screenPositionPoint = button.getScreenPositionPoint();
			const	S2DRectF32&						downImageRect = button.getDownImageRect();

			// Add down image info
			items += CGPURenderObject2D::Item(S2DRectF32(screenPositionPoint, downImageRect.mSize), 0, downImageRect);
		}

		// Create GPU Render Object 2D
		mInternals->mGPURenderObject2D =
				new CGPURenderObject2D(gpu, items,
						TNArray<CGPUTextureReference>(
								mInternals->mSceneAppResourceManagementInfo.mGPUTextureManager.gpuTextureReference(
										mInternals->mSceneAppResourceManagementInfo.createDataSource(
												getSceneItemButtonArray().getImageResourceFilename()),
										CImage::getBitmap, getSceneItemButtonArray().getImageResourceFilename())));
	}

	// Do super
	CSceneItemPlayer::load(gpu);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals->mGPURenderObject2D);

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
	mInternals->mGPURenderObject2D->finishLoading();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all buttons
	const	TArray<CSceneItemButtonArrayButton>&	buttonArrayButtons =
															getSceneItemButtonArray().getSceneItemButtonArrayButtons();
			CGPURenderObject2D::Indexes				indexes =
															CGPURenderObject2D::Indexes::withCount(
																	buttonArrayButtons.getCount() * 2);
	for (CArray::ItemIndex i = 0; i < buttonArrayButtons.getCount(); i++) {
		// Render this button
		const	CSceneItemButtonArrayButton&	button = buttonArrayButtons[i];

				UInt16							index =
														(!mInternals->mActiveButtonDown ||
																		!mInternals->mActiveButton.hasReference() ||
																		(&button != &(*mInternals->mActiveButton))) ?
																i : buttonArrayButtons.getCount() + i;
		indexes.add(index);
	}

	// Render
	mInternals->mGPURenderObject2D->render(indexes, renderInfo);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerButtonArray::handlesTouchOrMouseAtPoint(const S2DPointF32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate all buttons
	for (TIteratorD<CSceneItemButtonArrayButton> iterator =
					getSceneItemButtonArray().getSceneItemButtonArrayButtons().getIterator();
			iterator.hasValue(); iterator.advance()) {
		// Do cover check
		S2DRectF32	screenRect(iterator->getScreenPositionPoint(), iterator->getUpImageRect().mSize);
		if (screenRect.contains(point)) {
			// Got it
			mInternals->mActiveButton = OR<CSceneItemButtonArrayButton>(*iterator);
			mInternals->mActiveButtonDown = false;
			mInternals->mActiveButtonScreenRect = screenRect;

			return true;
		}
	}
	
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mActiveButtonDown = mInternals->mActiveButtonScreenRect.contains(point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mActiveButtonDown = mInternals->mActiveButtonScreenRect.contains(point2);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if active button is down
	if (mInternals->mActiveButtonDown) {
		// Collect actions
		OI<CActions>	actions = mInternals->mActiveButton->getActions();
		if (actions.hasInstance())
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
