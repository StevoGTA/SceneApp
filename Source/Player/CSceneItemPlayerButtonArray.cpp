//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerButtonArray.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerButtonArray.h"

#include "CFileX.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerButtonArrayInternals

// Placeholders
enum ABC { kCGLTextureReferencePixelFormatRGBA8888, };
enum DEF { kCGLTextureReferenceOptionsNone, };

class CGL2DRenderObject {
public:
	CGL2DRenderObject(const CFileX&, ABC, DEF) {}
	void drawSubrect(Float32, Float32, Float32, Float32, Float32, Float32) {}
};

class CSceneItemPlayerButtonArrayInternals {
	public:
		CSceneItemPlayerButtonArrayInternals()
			{
				mGL2DRenderObject = nil;

				mActiveButton = nil;
				mActiveButtonDown = false;
			}
		~CSceneItemPlayerButtonArrayInternals()
			{
				DisposeOf(mGL2DRenderObject);
			}

		CGL2DRenderObject*				mGL2DRenderObject;

		CSceneItemButtonArrayButton*	mActiveButton;
		bool							mActiveButtonDown;
		S2DRect32						mActiveButtonScreenRect;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

//static	CSceneItemPlayer*	sCreateSceneItemPlayer(const CSceneItem& sceneItem,
//									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: Register Scene Item Player

//REGISTER_SCENE_ITEM_PLAYER(CSceneItemPlayerButtonArray, sCreateSceneItemPlayer, CSceneItemButtonArray::mType);

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
	mInternals = new CSceneItemPlayerButtonArrayInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerButtonArray::~CSceneItemPlayerButtonArray()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: CSceneItemPlayer methods

//----------------------------------------------------------------------------------------------------------------------
S2DRect32 CSceneItemPlayerButtonArray::getCurrentScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
#warning finish
	return S2DRect32();
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray CSceneItemPlayerButtonArray::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
			CActionArray								allActionArray;
	const	TPtrArray<CSceneItemButtonArrayButton*>&	buttonsArray =
															getSceneItemButtonArray().getSceneButtonArrayButtonArray();
	for (CArrayItemIndex i = 0; i < buttonsArray.getCount(); i++) {
		CSceneItemButtonArrayButton*	button = buttonsArray[i];
		OR<CActionArray>				actionArray = button->getActionArray();
		if (actionArray.hasReference())
			allActionArray.addFrom(*actionArray);
	}

	return allActionArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::load()
//----------------------------------------------------------------------------------------------------------------------
{
//	if (mInternals->mGL2DRenderObject == nil)
//		mInternals->mGL2DRenderObject =
//				new CGL2DRenderObject(
//						CFileX(eGetURLForResourceFilename(getSceneItemButtonArray().getImageResourceFilename())),
//								kCGLTextureReferencePixelFormatRGBA8888, kCGLTextureReferenceOptionsNone);

	CSceneItemPlayer::load();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals->mGL2DRenderObject);

	CSceneItemPlayer::unload();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mActiveButton = nil;

	// Super
	CSceneItemPlayer::reset();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::render(CGPU& gpu, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
//	const	TPtrArray<CSceneItemButtonArrayButton*>&	buttonsArray =
//															getSceneItemButtonArray().getSceneButtonArrayButtonArray();
//	for (CArrayItemIndex i = 0; i < buttonsArray.getCount(); i++) {
//		CSceneItemButtonArrayButton*	button = buttonsArray[i];
//		S2DPoint32						point(button->getScreenPositionPoint().mX + offset.mX,
//												button->getScreenPositionPoint().mY + offset.mY);
//		S2DRect32						rect;
//		if ((button != mInternals->mActiveButton) || !mInternals->mActiveButtonDown)
//			rect = button->getUpImageRect();
//		else
//			rect = button->getDownImageRect();
//		mInternals->mGL2DRenderObject->drawSubrect(rect.mOrigin.mX, rect.mOrigin.mY, rect.mSize.mWidth, rect.mSize.mHeight,
//				point.mX, point.mY);
//	}
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerButtonArray::handlesTouchOrMouseAtPoint(const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	const	TPtrArray<CSceneItemButtonArrayButton*>&	buttonsArray =
															getSceneItemButtonArray().getSceneButtonArrayButtonArray();
	for (CArrayItemIndex i = 0; i < buttonsArray.getCount(); i++) {
		// Construct screenRect
		CSceneItemButtonArrayButton*	button = buttonsArray[i];
		S2DRect32						screenRect;
		screenRect.mOrigin = button->getScreenPositionPoint();
		screenRect.mSize = button->getUpImageRect().mSize;

		// Do cover check
		if (screenRect.contains(point)) {
			// Got it
			mInternals->mActiveButton = button;
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
	if (mInternals->mActiveButtonDown) {
		OR<CActionArray>	actionArray = mInternals->mActiveButton->getActionArray();
		if (actionArray.hasReference())
			perform(*actionArray);
	}
	
	mInternals->mActiveButtonDown = false;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerButtonArray::touchOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mActiveButtonDown = false;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc definitions

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* sCreateSceneItemPlayer(const CSceneItem& sceneItem,
//		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return new CSceneItemPlayerButtonArray(*((CSceneItemButtonArray*) &sceneItem), sceneAppResourceManagementInfo,
//			sceneItemPlayerProcsInfo, makeCopy);
//}
