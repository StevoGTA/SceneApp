//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayerText.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayerText.h"

#include "CGPUTextureManager.h"

#if defined(TARGET_OS_IOS)
#include <CoreText/CoreText.h>
#endif

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayerTextInternals

// Placeholder
typedef Float32 GLfloat;

enum ABC { GL_VERTEX_ARRAY, GL_TEXTURE_COORD_ARRAY, GL_FLOAT, GL_BLEND, GL_TEXTURE_2D, GL_TRIANGLE_STRIP, };
static void glEnableClientState(ABC) {}
static void glVertexPointer(int, ABC, int, GLfloat*) {}
static void glTexCoordPointer(int, ABC, int, GLfloat*) {}
static void glEnable(ABC) {}
static void glColor4f(Float32, Float32, Float32, Float32) {}
static void glDrawArrays(ABC, int, int) {}
static void glDisableClientState(ABC) {}
static void glDisable(ABC) {}



class CSceneItemPlayerTextInternals {
	public:
		CSceneItemPlayerTextInternals(CSceneItemText& sceneItemText)
			{
				mScreenRect = sceneItemText.getScreenRect();

				mText = sceneItemText.getText();
				mTextColor = sceneItemText.getTextColor();

				mOutlineColor = sceneItemText.getOutlineColor();
				mOutlineWidth = sceneItemText.getOutlineWidth();

				mFontName = sceneItemText.getFontName();
				mFontSize = sceneItemText.getFontSize();

				mRenderMaterialImageReference = nil;

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_MACOS)
				mColorSpaceRef = CGColorSpaceCreateDeviceRGB();

				CGFloat		colorComponents[4];
				colorComponents[0] = mTextColor.getRed();
				colorComponents[1] = mTextColor.getGreen();
				colorComponents[2] = mTextColor.getBlue();
				colorComponents[3] = mTextColor.getAlpha();
				mTextColorRef = CGColorCreate(mColorSpaceRef, colorComponents);

				colorComponents[0] = mOutlineColor.getRed();
				colorComponents[1] = mOutlineColor.getGreen();
				colorComponents[2] = mOutlineColor.getBlue();
				colorComponents[3] = mOutlineColor.getAlpha();
				mOutlineColorRef = CGColorCreate(mColorSpaceRef, colorComponents);

				CFStringRef	stringRef =
									::CFStringCreateWithCString(kCFAllocatorDefault,
											mFontName.getCString(kStringEncodingUTF8), kCFStringEncodingUTF8);
				mCTFontRef = CTFontCreateWithName(stringRef, mFontSize, nil);
				::CFRelease(stringRef);

				mCGFontRef = CTFontCopyGraphicsFont(mCTFontRef, nil);
#else
#endif
			}
		~CSceneItemPlayerTextInternals()
			{
				Delete(mRenderMaterialImageReference);

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_MACOS)
				CGColorSpaceRelease(mColorSpaceRef);
				CGColorRelease(mTextColorRef);
				CGColorRelease(mOutlineColorRef);
				CGFontRelease(mCGFontRef);
				CFRelease(mCTFontRef);
#else
#endif
			}

		S2DRect32						mScreenRect;

		CString							mText;
		CColor							mTextColor;

		CColor							mOutlineColor;
		Float32							mOutlineWidth;

		CString							mFontName;
		Float32							mFontSize;

		CGPUTextureReference*	mRenderMaterialImageReference;

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_MACOS)
		CGColorSpaceRef					mColorSpaceRef;
		CGColorRef						mTextColorRef;
		CGColorRef						mOutlineColorRef;
		CGFontRef						mCGFontRef;
		CTFontRef						mCTFontRef;
#else
#endif
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local proc declarations

static	CSceneItemPlayer*	sCreateSceneItemPlayer(const CSceneItem& sceneItem,
									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Register Scene Item Player

REGISTER_SCENE_ITEM_PLAYER(CSceneItemPlayerText, sCreateSceneItemPlayer, CSceneItemText::mType);

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerText

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerText::CSceneItemPlayerText(CSceneItemText& sceneItemText,
		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy) :
		CSceneItemPlayer(sceneItemText, sceneAppResourceManagementInfo, sceneItemPlayerProcsInfo, makeCopy)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CSceneItemPlayerTextInternals(sceneItemText);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayerText::~CSceneItemPlayerText()
//----------------------------------------------------------------------------------------------------------------------
{
	Delete(mInternals);
}

// MARK: CSceneItemPlayer methods

////----------------------------------------------------------------------------------------------------------------------
//S2DRect32 CSceneItemPlayerText::getCurrentScreenRect() const
////----------------------------------------------------------------------------------------------------------------------
//{
//	return getSceneItemText().getScreenRect();
//}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerText::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
	if (mInternals->mRenderMaterialImageReference == nil) {
		// Create texture
		// Create drawing context
		UInt32			byteCount = mInternals->mScreenRect.mSize.mWidth * mInternals->mScreenRect.mSize.mHeight * 4;
		UInt8*			dataPtr = (UInt8*) ::calloc(byteCount, 1);

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_MACOS)
		if (!mInternals->mText.isEmpty()) {
			CFStringRef		textStringRef =
									::CFStringCreateWithCString(kCFAllocatorDefault,
											mInternals->mText.getCString(kStringEncodingUTF8), kCFStringEncodingUTF8);
			CFIndex			charCount = ::CFStringGetLength(textStringRef);

			CGContextRef	contextRef =
									::CGBitmapContextCreate(dataPtr, mInternals->mScreenRect.mSize.mWidth,
											mInternals->mScreenRect.mSize.mHeight, 8,
											mInternals->mScreenRect.mSize.mWidth * 4,
											mInternals->mColorSpaceRef, kCGImageAlphaPremultipliedLast);
			
			// Setup
			::CGContextSetTextDrawingMode(contextRef, kCGTextFillStroke);
			::CGContextSetFillColorWithColor(contextRef, mInternals->mTextColorRef);
			::CGContextSetStrokeColorWithColor(contextRef, mInternals->mOutlineColorRef);
			::CGContextSetLineWidth(contextRef, mInternals->mOutlineWidth);

			::CGContextSetFont(contextRef, mInternals->mCGFontRef);
			::CGContextSetFontSize(contextRef, mInternals->mFontSize);

			// Draw text
			CFMutableAttributedStringRef	stringRef = ::CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
			::CFAttributedStringReplaceString(stringRef, CFRangeMake(0, 0), textStringRef);
			::CFAttributedStringSetAttribute(stringRef, CFRangeMake(0, charCount), kCTFontAttributeName,
					mInternals->mCTFontRef);

			CTLineRef	lineRef = ::CTLineCreateWithAttributedString(stringRef);
			CTRunRef	runRef = (CTRunRef) ::CFArrayGetValueAtIndex(::CTLineGetGlyphRuns(lineRef), 0);

			CGFloat		ascent, descent, leading;
			CGFloat		runWidth =
								::CTRunGetTypographicBounds(runRef, ::CFRangeMake(0, charCount), &ascent, &descent,
										&leading);

			CGFloat		x;
			switch (getSceneItemText().getHorizontalPosition()) {
				case kSceneItemTextHorizontalPositionCentered:
					// Centered
					x = (mInternals->mScreenRect.mSize.mWidth - runWidth) / 2.0;
					break;
				
				case kSceneItemTextHorizontalPositionRightJustified:
					// Right-justified
					x = mInternals->mScreenRect.mSize.mWidth - runWidth;
					break;
				
				default:
					// Left-justified
					x = 3.0;
					break;
			}

			::CGContextSetTextPosition(contextRef, x, mInternals->mScreenRect.mSize.mHeight - mInternals->mFontSize);
			::CGContextShowGlyphs(contextRef, ::CTRunGetGlyphsPtr(runRef), charCount);
			::CFRelease(lineRef);
			::CFRelease(stringRef);

			// Done
			::CGContextRelease(contextRef);
			::CFRelease(textStringRef);
		}
#else
	#warning implement
#endif

//		// Create texture
//		mInternals->mRenderMaterialImageReference =
////				CGLTextureCache::newTextureReferenceFromImageData(CData(dataPtr, byteCount, true),
////						mInternals->mScreenRect.mSize.mWidth, mInternals->mScreenRect.mSize.mHeight,
////						kCGLTextureReferenceDataFormatUncompressed, kCGLTextureReferencePixelFormatRGBA8888,
////						kCGLTextureReferenceOptionsNone);
//				new CGPUTextureReference(
//						CGPUTextureManager::mDefault.renderMaterialTextureReference(
//								CData(dataPtr, byteCount, true), kGPUTextureDataFormatUncompressed,
//								kEGPUTextureFormatRGBA8888,
//								SGPUTextureSize(mInternals->mScreenRect.mSize.mWidth,
//										mInternals->mScreenRect.mSize.mHeight)));

		// Cleanup
		::free(dataPtr);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerText::render(CGPURenderEngine& gpuRenderEngine, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
//	// Screen coordinates
//	GLfloat	minX = mInternals->mScreenRect.getMinX() + offset.mX;
//	GLfloat	maxX = mInternals->mScreenRect.getMaxX() + offset.mX;
//	GLfloat	minY = mInternals->mScreenRect.getMinY() + offset.mY;
//	GLfloat	maxY = mInternals->mScreenRect.getMaxY() + offset.mY;
//
//	GLfloat	vertices[8] = {
//					minX,	maxY,
//					maxX,	maxY,
//					minX,	minY,
//					maxX,	minY,
//				};
//
//	// Texture Coordinates
//	GLfloat	maxU, maxV;
//	mInternals->mRenderMaterialImageReference->getMaxUVs(maxU, maxV);
//	GLfloat textureUVs[8] = {
//					0.0f,	maxV,
//					maxU,	maxV,
//					0.0f,	0.0f,
//					maxU,	0.0f,
//				};
//
//	// Draw
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(2, GL_FLOAT, 0, vertices);
//
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glTexCoordPointer(2, GL_FLOAT, 0, textureUVs);
//
//	glEnable(GL_BLEND);
//	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//
//	glEnable(GL_TEXTURE_2D);
//	mInternals->mRenderMaterialImageReference->setupForRendering();
//
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisable(GL_BLEND);
//	glDisable(GL_TEXTURE_2D);
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerText::setScreenRect(const S2DRect32& screenRect)
//----------------------------------------------------------------------------------------------------------------------
{
	if (screenRect != mInternals->mScreenRect) {
		mInternals->mScreenRect = screenRect;
		Delete(mInternals->mRenderMaterialImageReference);
	}
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayerText::setText(const CString& string)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mText = string;
	Delete(mInternals->mRenderMaterialImageReference);
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
	return new CSceneItemPlayerText(*((CSceneItemText*) &sceneItem), sceneAppResourceManagementInfo,
			sceneItemPlayerProcsInfo, makeCopy);
}
