//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerPDF.cpp			©2014 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerPDF.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

enum {
	kCSceneItemPlayerPDFTouchTrackingPartPDFContent,
	kCSceneItemPlayerPDFTouchTrackingPartPDFLink,
	kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbPageUp,
	kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbDrag,
	kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbPageDown,
	kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbPageLeft,
	kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbDrag,
	kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbPageRight,
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerPDFInternals

class CSceneItemPlayerPDFInternals {
	public:
				CSceneItemPlayerPDFInternals()
					{
						mPDFBitmapSize = k2DSizeZero;
						mPDFScreenRect = S2DRect32::mZero;
						mPDFVisibleSubrect = S2DRect32::mZero;
						mPDFRenderObject = nil;
						
						mVerticalThumbHeight = 0.0;
						mVerticalScrollValue = 0.0;
						mVerticalThumbRenderObject = nil;
						mVerticalThumbScreenRect = S2DRect32::mZero;
						
						mHorizontalThumbWidth = 0.0;
						mHorizontalScrollValue = 0.0;
						mHorizontalThumbRenderObject = nil;
						mHorizontalThumbScreenRect = S2DRect32::mZero;
					}
				~CSceneItemPlayerPDFInternals()
					{
						Delete(mPDFRenderObject);
						mPDFLinksArray.disposeAll();
						
						Delete(mHorizontalThumbRenderObject);

						Delete(mVerticalThumbRenderObject);
					}
				
		void	updatePDFVisibleSubrect()
					{
						mPDFVisibleSubrect.mOrigin.mY =
								(mPDFBitmapSize.mHeight - mPDFScreenRect.mSize.mHeight) * mVerticalScrollValue;
						mPDFVisibleSubrect.mOrigin.mX =
								(mPDFBitmapSize.mWidth - mPDFScreenRect.mSize.mWidth) * mHorizontalScrollValue;
					}
		void	updateThumbScrollValues()
					{
						if (mPDFBitmapSize.mHeight != mPDFScreenRect.mSize.mHeight)
							mVerticalScrollValue = mPDFVisibleSubrect.mOrigin.mY /
									(mPDFBitmapSize.mHeight - mPDFScreenRect.mSize.mHeight);
						if (mPDFBitmapSize.mWidth != mPDFScreenRect.mSize.mWidth)
							mHorizontalScrollValue = mPDFVisibleSubrect.mOrigin.mX /
									(mPDFBitmapSize.mWidth - mPDFScreenRect.mSize.mWidth);
					}
		
		
		S2DSize32			mPDFBitmapSize;
		S2DRect32			mPDFScreenRect;
		S2DRect32			mPDFVisibleSubrect;
		CGL2DRenderObject*	mPDFRenderObject;
		TPtrArray<CPDFLink*>	mPDFLinksArray;

		Float32				mVerticalThumbHeight;
		Float32				mVerticalScrollValue;
		CGL2DRenderObject*	mVerticalThumbRenderObject;
		S2DRect32			mVerticalThumbScreenRect;

		Float32				mHorizontalThumbWidth;
		Float32				mHorizontalScrollValue;
		CGL2DRenderObject*	mHorizontalThumbRenderObject;
		S2DRect32			mHorizontalThumbScreenRect;
		
		S2DPoint32			mTouchTrackingStartPoint;
		UInt8				mTouchTrackingPart;
		S2DPoint32			mTouchTrackingPartStartPoint;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

static	CSceneItemPlayer*	sCreateSceneItemPlayer(const CSceneItem& sceneItem,
									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: Register Scene Item Player

REGISTER_SCENE_ITEM_PLAYER(CSceneItemPlayerPDF, sCreateSceneItemPlayer, CSceneItemPDF::mType);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerPDF

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerPDF::CSceneItemPlayerPDF(CSceneItemPDF& sceneItemPDF,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy) :
		CSceneItemPlayer(sceneItemPDF, sceneAppResourceManagementInfo, sceneItemPlayerProcsInfo, makeCopy)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemPlayerPDFInternals();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerPDF::~CSceneItemPlayerPDF()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: CSceneItemPlayer methods

////----------------------------------------------------------------------------------------------------------------------
//S2DRect32 CSceneItemPlayerPDF::getCurrentScreenRect() const
////----------------------------------------------------------------------------------------------------------------------
//{
//	return getSceneItemPDF().getScreenRect();
//}

//----------------------------------------------------------------------------------------------------------------------
TPtrArray<CAction*> CSceneItemPlayerPDF::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	TPtrArray<CAction*>	actionsArray;

//	CActions*	actions = getSceneItemPDF().getActionsOrNil();
//	if (actions != nil)
//		actionsArray.addItemsFromArray(*actions);

	return actionsArray;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerPDF::load()
//----------------------------------------------------------------------------------------------------------------------
{
	CSceneItemPDF&	sceneItemPDF = getSceneItemPDF();
	
	// Initialize Rect
	mInternals->mPDFScreenRect = sceneItemPDF.getScreenRect();
	
	// Vertical Thumb
	if (sceneItemPDF.getVerticalThumbImageResourceFilename().getLength() > 0) {
		// Create render object
		mInternals->mVerticalThumbRenderObject =
				new CGL2DRenderObject(
						CFileX(eGetURLForResourceFilename(sceneItemPDF.getVerticalThumbImageResourceFilename())));
		
		// Get info
		GLsizei	verticalThumbWidth, verticalThumbHeight;
		mInternals->mVerticalThumbRenderObject->getTextureReference().getUsedPixels(verticalThumbWidth,
				verticalThumbHeight);

		// Update dimensions
		mInternals->mPDFScreenRect.mSize.mWidth -= verticalThumbWidth;
		mInternals->mVerticalThumbHeight = verticalThumbHeight;
		mInternals->mVerticalThumbScreenRect =
				S2DRect32(mInternals->mPDFScreenRect.getMaxX(), mInternals->mPDFScreenRect.mOrigin.mY,
						verticalThumbWidth, mInternals->mPDFScreenRect.mSize.mHeight);
	}
	
	// Horizontal Thumb
	if (sceneItemPDF.getHorizontalThumbImageResourceFilename().getLength() > 0) {
		// Create render object
		mInternals->mHorizontalThumbRenderObject =
				new CGL2DRenderObject(CFileX(eGetURLForResourceFilename(
						sceneItemPDF.getHorizontalThumbImageResourceFilename())));
		
		// Get info
		GLsizei	horizontalThumbWidth, horizontalThumbHeight;
		mInternals->mHorizontalThumbRenderObject->getTextureReference().getUsedPixels(horizontalThumbWidth,
				horizontalThumbHeight);

		// Update dimensions
		mInternals->mPDFScreenRect.mSize.mHeight -= horizontalThumbHeight;
		mInternals->mHorizontalThumbWidth = horizontalThumbWidth;
		mInternals->mHorizontalThumbScreenRect =
				S2DRect32(mInternals->mPDFScreenRect.mOrigin.mX, mInternals->mPDFScreenRect.getMaxY(),
						mInternals->mPDFScreenRect.mSize.mWidth, horizontalThumbHeight);
	}
	
	// PDF Bitmap
	Float32	renderWidth =
					(mInternals->mHorizontalThumbRenderObject != nil) ? 0 : mInternals->mPDFScreenRect.mSize.mWidth;
	Float32	renderHeight =
					(mInternals->mVerticalThumbRenderObject != nil) ? 0 : mInternals->mPDFScreenRect.mSize.mHeight;
	
	CPDF		pdf(eGetURLForResourceFilename(sceneItemPDF.getResourceFilename()));
	S2DRect32	pageBoxRect = pdf.getPageRect(0, sceneItemPDF.getBox());
	if ((renderWidth == 0) && (renderHeight == 0))
		// Render at page resolution
		mInternals->mPDFBitmapSize = S2DSize32(pageBoxRect.mSize.mWidth, pageBoxRect.mSize.mHeight);
	else if (renderWidth == 0)
		// Scale width to match render height
		mInternals->mPDFBitmapSize =
				S2DSize32((CGFloat) renderHeight * pageBoxRect.mSize.mWidth / pageBoxRect.mSize.mHeight, renderHeight);
	else if (renderHeight == 0)
		// Scale height to match render width
		mInternals->mPDFBitmapSize =
				S2DSize32(renderWidth, (CGFloat) renderWidth * pageBoxRect.mSize.mHeight / pageBoxRect.mSize.mWidth);
	else
		// Render at specified resolution
		mInternals->mPDFBitmapSize = S2DSize32(renderWidth, renderHeight);

	CBitmapX	bitmap =
					pdf.getPageBitmap(0, sceneItemPDF.getBox(), SBitmapSize32(mInternals->mPDFBitmapSize.mWidth,
							mInternals->mPDFBitmapSize.mHeight));
	mInternals->mPDFRenderObject = new CGL2DRenderObject(bitmap);
	
	// PDF Links
	TPtrArray<CPDFLink*>	pdfLinksArray = pdf.getPageLinks(0, sceneItemPDF.getBox());
	Float32				xScale = mInternals->mPDFBitmapSize.mWidth / pageBoxRect.mSize.mWidth;
	Float32				yScale = mInternals->mPDFBitmapSize.mHeight / pageBoxRect.mSize.mHeight;
	for (CArrayItemIndex i = 0; i < pdfLinksArray.getCount(); i++) {
		// Get PDF Link
		CPDFLink*	pdfLink = pdfLinksArray[i];
		
		// Transform link rect
		S2DRect32	pdfLinkRect = pdfLink->getLinkRect();
		pdfLinkRect.mOrigin.mX = (pdfLinkRect.mOrigin.mX - pageBoxRect.mOrigin.mX) * xScale;
		pdfLinkRect.mOrigin.mY = (pdfLinkRect.mOrigin.mY - pageBoxRect.mOrigin.mY) * yScale;
		pdfLinkRect.mSize.mWidth *= xScale;
		pdfLinkRect.mSize.mHeight *= yScale;
		
		// Add link to array
		mInternals->mPDFLinksArray += new CPDFLink(pdfLink->getLinkType(), pdfLink->getLinkString(), pdfLinkRect);
	}
	
	// Setup for drawing
	mInternals->mPDFVisibleSubrect.mSize = mInternals->mPDFScreenRect.mSize;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerPDF::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals->mPDFRenderObject);
	mInternals->mPDFLinksArray.disposeAll();

	Delete(mInternals->mVerticalThumbRenderObject);

	Delete(mInternals->mHorizontalThumbRenderObject);
}

////----------------------------------------------------------------------------------------------------------------------
//bool CSceneItemPlayerPDF::getIsFullyLoaded() const
////----------------------------------------------------------------------------------------------------------------------
//{
//	return true;
//}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerPDF::drawGL(const S2DPoint32 &offset)
//----------------------------------------------------------------------------------------------------------------------
{
	// Draw Vertical Thumb
	if (mInternals->mVerticalThumbRenderObject != nil) {
		Float32	y =
						mInternals->mVerticalThumbScreenRect.mOrigin.mY +
								(mInternals->mVerticalThumbScreenRect.mSize.mHeight -
								mInternals->mVerticalThumbHeight) * mInternals->mVerticalScrollValue;
		mInternals->mVerticalThumbRenderObject->draw(mInternals->mVerticalThumbScreenRect.mOrigin.mX + offset.mX,
				y + offset.mY);
	}
	
	// Draw Horizontal Thumb
	if (mInternals->mHorizontalThumbRenderObject != nil) {
		Float32	x =
						mInternals->mHorizontalThumbScreenRect.mOrigin.mX +
								(mInternals->mHorizontalThumbScreenRect.mSize.mWidth -
								mInternals->mHorizontalThumbWidth) * mInternals->mHorizontalScrollValue;
		mInternals->mHorizontalThumbRenderObject->draw(x + offset.mX,
				mInternals->mHorizontalThumbScreenRect.mOrigin.mY + offset.mY);
	}

	// Draw PDF
	mInternals->mPDFRenderObject->drawSubrect(mInternals->mPDFVisibleSubrect.mOrigin.mX,
			mInternals->mPDFVisibleSubrect.mOrigin.mY, mInternals->mPDFVisibleSubrect.mSize.mWidth,
			mInternals->mPDFVisibleSubrect.mSize.mHeight, mInternals->mPDFScreenRect.mOrigin.mX + offset.mX,
			mInternals->mPDFScreenRect.mOrigin.mY + offset.mY);
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayerPDF::handlesTouchOrMouseAtPoint(const S2DPoint32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mPDFScreenRect.contains(point) || mInternals->mVerticalThumbScreenRect.contains(point) ||
			mInternals->mHorizontalThumbScreenRect.contains(point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerPDF::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
		const CSceneAppTouchObjectRef touchObjectRef)
//----------------------------------------------------------------------------------------------------------------------
{
	if (tapOrClickCount == 1) {
		// Remember starting point
		mInternals->mTouchTrackingStartPoint = point;
		
		// Figure tracking part
		if (mInternals->mPDFScreenRect.contains(point)) {
			// PDF
			mInternals->mTouchTrackingPart = kCSceneItemPlayerPDFTouchTrackingPartPDFContent;
			mInternals->mTouchTrackingPartStartPoint = mInternals->mPDFVisibleSubrect.mOrigin;

			S2DPoint32	pdfPoint(point.mX - mInternals->mPDFScreenRect.mOrigin.mX +
										mInternals->mPDFVisibleSubrect.mOrigin.mX,
								point.mY - mInternals->mPDFScreenRect.mOrigin.mY +
										mInternals->mPDFVisibleSubrect.mOrigin.mY);
			for (CArrayItemIndex i = 0; i < mInternals->mPDFLinksArray.getCount(); i++) {
				CPDFLink*	pdfLink = mInternals->mPDFLinksArray[i];
				if ((pdfLink->getLinkType() == kCPDFLinkTypeURL) && pdfLink->getLinkRect().contains(pdfPoint)) {
					mInternals->mTouchTrackingPart = kCSceneItemPlayerPDFTouchTrackingPartPDFLink;
					break;
				}
			}
		} else if (mInternals->mVerticalThumbScreenRect.contains(point)) {
			// Vertical Thumb
			Float32	verticalThumbY = point.mY - mInternals->mVerticalThumbScreenRect.mOrigin.mY;
			mInternals->mTouchTrackingPartStartPoint.mY =
					(mInternals->mVerticalThumbScreenRect.mSize.mHeight -
							mInternals->mVerticalThumbHeight) * mInternals->mVerticalScrollValue;
			if (verticalThumbY < mInternals->mTouchTrackingPartStartPoint.mY)
				// Page Up
				mInternals->mTouchTrackingPart = kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbPageUp;
			else if (verticalThumbY <
					(mInternals->mTouchTrackingPartStartPoint.mY + mInternals->mVerticalThumbHeight)) {
				// Thumb Drag
				mInternals->mTouchTrackingPart = kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbDrag;
			} else
				// Page Down
				mInternals->mTouchTrackingPart = kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbPageDown;
		} else {
			// Horizontal Thumb
			Float32	verticalThumbX = point.mX - mInternals->mHorizontalThumbScreenRect.mOrigin.mX;
			mInternals->mTouchTrackingPartStartPoint.mX =
					(mInternals->mHorizontalThumbScreenRect.mSize.mWidth - mInternals->mHorizontalThumbWidth) *
							mInternals->mHorizontalScrollValue;
			if (verticalThumbX < mInternals->mTouchTrackingPartStartPoint.mX)
				// Page Left
				mInternals->mTouchTrackingPart = kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbPageLeft;
			else if (verticalThumbX <
					(mInternals->mTouchTrackingPartStartPoint.mX + mInternals->mHorizontalThumbWidth)) {
				// Thumb Drag
				mInternals->mTouchTrackingPart = kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbDrag;
			} else
				// Page Right
				mInternals->mTouchTrackingPart = kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbPageRight;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerPDF::touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
		const CSceneAppTouchObjectRef touchObjectRef)
//----------------------------------------------------------------------------------------------------------------------
{
	Float32	deltaX = point2.mX - mInternals->mTouchTrackingStartPoint.mX;
	Float32	deltaY = point2.mY - mInternals->mTouchTrackingStartPoint.mY;

	switch (mInternals->mTouchTrackingPart) {
		case kCSceneItemPlayerPDFTouchTrackingPartPDFContent:
			// PDF Content
			mInternals->mPDFVisibleSubrect.mOrigin.mX = mInternals->mTouchTrackingPartStartPoint.mX - deltaX;
			if (mInternals->mPDFVisibleSubrect.mOrigin.mX < 0.0)
				mInternals->mPDFVisibleSubrect.mOrigin.mX = 0.0;
			else if (mInternals->mPDFVisibleSubrect.mOrigin.mX >
					(mInternals->mPDFBitmapSize.mWidth - mInternals->mPDFScreenRect.mSize.mWidth))
				mInternals->mPDFVisibleSubrect.mOrigin.mX =
						(mInternals->mPDFBitmapSize.mWidth - mInternals->mPDFScreenRect.mSize.mWidth);
				
			mInternals->mPDFVisibleSubrect.mOrigin.mY = mInternals->mTouchTrackingPartStartPoint.mY - deltaY;
			if (mInternals->mPDFVisibleSubrect.mOrigin.mY < 0.0)
				mInternals->mPDFVisibleSubrect.mOrigin.mY = 0.0;
			else if (mInternals->mPDFVisibleSubrect.mOrigin.mY >
					(mInternals->mPDFBitmapSize.mHeight - mInternals->mPDFScreenRect.mSize.mHeight))
				mInternals->mPDFVisibleSubrect.mOrigin.mY =
						(mInternals->mPDFBitmapSize.mHeight - mInternals->mPDFScreenRect.mSize.mHeight);
			
			mInternals->updateThumbScrollValues();
			break;
		
		case kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbDrag:
			// Vertical Thumb Drag
			mInternals->mVerticalScrollValue =
					(mInternals->mTouchTrackingPartStartPoint.mY + deltaY) /
							(mInternals->mVerticalThumbScreenRect.mSize.mHeight - mInternals->mVerticalThumbHeight);
			if (mInternals->mVerticalScrollValue < 0.0)
				mInternals->mVerticalScrollValue = 0.0;
			else if (mInternals->mVerticalScrollValue > 1.0)
				mInternals->mVerticalScrollValue = 1.0;
			
			mInternals->updatePDFVisibleSubrect();
			break;
		
		case kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbDrag:
			// Horizontal Thumb Drag
			mInternals->mHorizontalScrollValue =
					(mInternals->mTouchTrackingPartStartPoint.mX + deltaX) /
							(mInternals->mHorizontalThumbScreenRect.mSize.mWidth - mInternals->mHorizontalThumbWidth);
			if (mInternals->mHorizontalScrollValue < 0.0)
				mInternals->mHorizontalScrollValue = 0.0;
			else if (mInternals->mHorizontalScrollValue > 1.0)
				mInternals->mHorizontalScrollValue = 1.0;
			
			mInternals->updatePDFVisibleSubrect();
			break;
		
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerPDF::touchEndedOrMouseUpAtPoint(const S2DPoint32& point,
		const CSceneAppTouchObjectRef touchObjectRef)
//----------------------------------------------------------------------------------------------------------------------
{
	switch (mInternals->mTouchTrackingPart) {
		case kCSceneItemPlayerPDFTouchTrackingPartPDFLink: {
			// PDF Link
			S2DPoint32	pdfPoint(point.mX - mInternals->mPDFScreenRect.mOrigin.mX +
										mInternals->mPDFVisibleSubrect.mOrigin.mX,
								point.mY - mInternals->mPDFScreenRect.mOrigin.mY +
										mInternals->mPDFVisibleSubrect.mOrigin.mY);
			for (CArrayItemIndex i = 0; i < mInternals->mPDFLinksArray.getCount(); i++) {
				CPDFLink*	pdfLink = mInternals->mPDFLinksArray[i];
				if ((pdfLink->getLinkType() == kCPDFLinkTypeURL) && pdfLink->getLinkRect().contains(pdfPoint)) {
					// Open URL
					CActions	actions;

					CDictionary	actionInfo;
					actionInfo.set(CString(OSSTR("URL")), pdfLink->getLinkString());
					actions += new CAction(CString(OSSTR("openURL")), actionInfo);

					perform(actions, mInternals->mTouchTrackingStartPoint);
					break;
				}
			}
			} break;
		
		case kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbPageUp:
			// Vertical Thumb Page Up
			if (isTapOrMouseClick(mInternals->mTouchTrackingStartPoint, point)) {
				// Page Thumb Up
				mInternals->mVerticalScrollValue =
						(mInternals->mTouchTrackingPartStartPoint.mY - mInternals->mVerticalThumbHeight) /
								(mInternals->mVerticalThumbScreenRect.mSize.mHeight - mInternals->mVerticalThumbHeight);
				if (mInternals->mVerticalScrollValue < 0.0)
					mInternals->mVerticalScrollValue = 0.0;
			
				mInternals->updatePDFVisibleSubrect();
			}
			break;
		
		case kCSceneItemPlayerPDFTouchTrackingPartVerticalThumbPageDown:
			// Vertical Thumb Page Down
			if (isTapOrMouseClick(mInternals->mTouchTrackingStartPoint, point)) {
				// Page Thumb Down
				mInternals->mVerticalScrollValue =
						(mInternals->mTouchTrackingPartStartPoint.mY + mInternals->mVerticalThumbHeight) /
								(mInternals->mVerticalThumbScreenRect.mSize.mHeight - mInternals->mVerticalThumbHeight);
				if (mInternals->mVerticalScrollValue > 1.0)
					mInternals->mVerticalScrollValue = 1.0;
			
				mInternals->updatePDFVisibleSubrect();
			}
			break;
		
		case kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbPageLeft:
			// Horizontal Thumb Page Left
			if (isTapOrMouseClick(mInternals->mTouchTrackingStartPoint, point)) {
				// Page Thumb Left
				mInternals->mHorizontalScrollValue =
						(mInternals->mTouchTrackingPartStartPoint.mX - mInternals->mHorizontalThumbWidth) /
								(mInternals->mHorizontalThumbScreenRect.mSize.mWidth -
										mInternals->mHorizontalThumbWidth);
				if (mInternals->mHorizontalScrollValue < 0.0)
					mInternals->mHorizontalScrollValue = 0.0;
			
				mInternals->updatePDFVisibleSubrect();
			}
			break;
		
		case kCSceneItemPlayerPDFTouchTrackingPartHorizontalThumbPageRight:
			// Horizontal Thumb Page Right
			if (isTapOrMouseClick(mInternals->mTouchTrackingStartPoint, point)) {
				// Page Thumb Right
				mInternals->mHorizontalScrollValue =
						(mInternals->mTouchTrackingPartStartPoint.mX + mInternals->mHorizontalThumbWidth) /
								(mInternals->mHorizontalThumbScreenRect.mSize.mWidth -
										mInternals->mHorizontalThumbWidth);
				if (mInternals->mHorizontalScrollValue > 1.0)
					mInternals->mHorizontalScrollValue = 1.0;
			
				mInternals->updatePDFVisibleSubrect();
			}
			break;
		
		default:
			break;
	}
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
	return new CSceneItemPlayerPDF(*((CSceneItemPDF*) &sceneItem), sceneAppResourceManagementInfo,
			sceneItemPlayerProcsInfo, makeCopy);
}
