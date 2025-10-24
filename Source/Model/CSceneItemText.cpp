//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemText.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemText.h"

#include "CReferenceCountable.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemText::Internals

class CSceneItemText::Internals : public TCopyOnWriteReferenceCountable<Internals> {
	public:
		Internals(const S2DRectF32 screenRect, const CString& text, const CColor& textColor, const CColor& outlineColor,
				Float32 outlineWidth, const CString& fontName, Float32 fontSize,
				HorizontalPosition horizontalPosition) :
			TCopyOnWriteReferenceCountable(),
					mScreenRect(screenRect),
					mText(text), mTextColor(textColor),
					mOutlineColor(outlineColor), mOutlineWidth(outlineWidth),
					mFontName(fontName), mFontSize(fontSize),
					mHorizontalPosition(horizontalPosition)
			{}
		Internals(const Internals& other) :
			TCopyOnWriteReferenceCountable(),
					mScreenRect(other.mScreenRect),
					mText(other.mText), mTextColor(other.mTextColor),
					mOutlineColor(other.mOutlineColor), mOutlineWidth(other.mOutlineWidth),
					mFontName(other.mFontName), mFontSize(other.mFontSize),
					mHorizontalPosition(other.mHorizontalPosition)
			{}

		S2DRectF32			mScreenRect;

		CString				mText;
		CColor				mTextColor;

		CColor				mOutlineColor;
		Float32				mOutlineWidth;

		CString				mFontName;
		Float32				mFontSize;

		HorizontalPosition	mHorizontalPosition;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemText

// MARK: Properties

const	CString	CSceneItemText::mType(OSSTR("text"));

const	CString	CSceneItemText::mPropertyNameScreenRect(OSSTR("screenRect"));
const	CString	CSceneItemText::mPropertyNameText(OSSTR("text"));
const	CString	CSceneItemText::mPropertyNameTextColor(OSSTR("textColor"));
const	CString	CSceneItemText::mPropertyNameOutlineColor(OSSTR("outlineColor"));
const	CString	CSceneItemText::mPropertyNameOutlineWidth(OSSTR("outlineWidth"));
const	CString	CSceneItemText::mPropertyNameFontName(OSSTR("fontName"));
const	CString	CSceneItemText::mPropertyNameFontSize(OSSTR("fontSize"));
const	CString	CSceneItemText::mPropertyNameHorizontalPosition(OSSTR("horizontalPosition"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemText::CSceneItemText(const CString& name, bool isInitiallyVisible, Options options,
		const S2DRectF32 screenRect, const CString& text, const CColor& textColor, const CColor& outlineColor,
		Float32 outlineWidth, const CString& fontName, Float32 fontSize, HorizontalPosition horizontalPosition) :
		CSceneItem(name, isInitiallyVisible, options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new Internals(screenRect, text, textColor, outlineColor, outlineWidth, fontName, fontSize,
					horizontalPosition);
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemText::CSceneItemText(const CDictionary& info) : CSceneItem(info)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals =
			new Internals(S2DRectF32(info.getString(mPropertyNameScreenRect)),
					info.getString(mPropertyNameText, CString::mEmpty),
					info.contains(mPropertyNameTextColor) ?
							CColor(info.getDictionary(mPropertyNameTextColor)) : CColor::mBlack,
					info.contains(mPropertyNameOutlineColor) ?
							CColor(info.getDictionary(mPropertyNameOutlineColor)) : CColor::mClear,
					info.getFloat32(mPropertyNameOutlineWidth, 0.0),
					info.getString(mPropertyNameFontName, CString(OSSTR("Helvetica-Bold"))),
					info.getFloat32(mPropertyNameFontSize, 28.0),
					(HorizontalPosition) info.getUInt8(mPropertyNameHorizontalPosition,
							kHorizontalPositionLeftJustified));
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemText::CSceneItemText(const CSceneItemText& other) : CSceneItem(other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemText::~CSceneItemText()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: CSceneItem methods

//----------------------------------------------------------------------------------------------------------------------
TMArray<CDictionary> CSceneItemText::getProperties() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	TMArray<CDictionary>	properties = CSceneItem::getProperties();

	// Add properties
	CDictionary	screenRectPropertyInfo;
	screenRectPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Screen Rect")));
	screenRectPropertyInfo.set(mPropertyNameKey, mPropertyNameScreenRect);
	screenRectPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeScreenRect);
	properties += screenRectPropertyInfo;

	CDictionary	textPropertyInfo;
	textPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Text")));
	textPropertyInfo.set(mPropertyNameKey, mPropertyNameText);
	textPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeText);
	properties += textPropertyInfo;

	CDictionary	textColorPropertyInfo;
	textColorPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Text Color")));
	textColorPropertyInfo.set(mPropertyNameKey, mPropertyNameTextColor);
	textColorPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeColor);
	properties += textColorPropertyInfo;

	CDictionary	outlineColorPropertyInfo;
	outlineColorPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Outline Color")));
	outlineColorPropertyInfo.set(mPropertyNameKey, mPropertyNameOutlineColor);
	outlineColorPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeColor);
	properties += outlineColorPropertyInfo;

	CDictionary	outlineWidthPropertyInfo;
	outlineWidthPropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Outline Width")));
	outlineWidthPropertyInfo.set(mPropertyNameKey, mPropertyNameOutlineWidth);
	outlineWidthPropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypePixelWidth);
	properties += outlineWidthPropertyInfo;

	CDictionary	fontNamePropertyInfo;
	fontNamePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Font Name")));
	fontNamePropertyInfo.set(mPropertyNameKey, mPropertyNameFontName);
	fontNamePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeFontName);
	properties += fontNamePropertyInfo;

	CDictionary	fontSizePropertyInfo;
	fontSizePropertyInfo.set(mPropertyTitleKey, CString(OSSTR("Font Size")));
	fontSizePropertyInfo.set(mPropertyNameKey, mPropertyNameFontSize);
	fontSizePropertyInfo.set(mPropertyTypeKey, CSceneItem::kPropertyTypeFontSize);
	properties += fontSizePropertyInfo;

	return properties;
}

//----------------------------------------------------------------------------------------------------------------------
CDictionary CSceneItemText::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info = CSceneItem::getInfo();

	info.set(mPropertyNameScreenRect, mInternals->mScreenRect.asString());
	info.set(mPropertyNameText, mInternals->mText);
	info.set(mPropertyNameTextColor, mInternals->mTextColor.getInfo());
	info.set(mPropertyNameOutlineColor, mInternals->mOutlineColor.getInfo());
	info.set(mPropertyNameOutlineWidth, mInternals->mOutlineWidth);
	info.set(mPropertyNameFontName, mInternals->mFontName);
	info.set(mPropertyNameFontSize, mInternals->mFontSize);
	info.set(mPropertyNameHorizontalPosition, (UInt8) mInternals->mHorizontalPosition);

	return info;
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const S2DRectF32& CSceneItemText::getScreenRect() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenRect;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemText::setScreenRect(const S2DRectF32& rect)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (rect != mInternals->mScreenRect) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mScreenRect = rect;

		// Note updated
		noteUpdated(mPropertyNameScreenRect);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CSceneItemText::getText() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mText;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemText::setText(const CString& text)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (text != mInternals->mText) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mText = text;

		// Note updated
		noteUpdated(mPropertyNameText);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const CColor& CSceneItemText::getTextColor() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mTextColor;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemText::setTextColor(const CColor& textColor)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (textColor != mInternals->mTextColor) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mTextColor = textColor;

		// Note updated
		noteUpdated(mPropertyNameTextColor);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const CColor& CSceneItemText::getOutlineColor() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOutlineColor;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemText::setOutlineColor(const CColor& outlineColor)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (outlineColor != mInternals->mOutlineColor) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mOutlineColor = outlineColor;

		// Note updated
		noteUpdated(mPropertyNameOutlineColor);
	}
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CSceneItemText::getOutlineWidth() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOutlineWidth;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemText::setOutlineWidth(Float32 outlineWidth)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (outlineWidth != mInternals->mOutlineWidth) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mOutlineWidth = outlineWidth;

		// Note updated
		noteUpdated(mPropertyNameOutlineWidth);
	}
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CSceneItemText::getFontName() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mFontName;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemText::setFontName(const CString& fontName)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (fontName != mInternals->mFontName) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mFontName = fontName;

		// Note updated
		noteUpdated(mPropertyNameFontName);
	}
}

//----------------------------------------------------------------------------------------------------------------------
Float32 CSceneItemText::getFontSize() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mFontSize;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemText::setFontSize(Float32 fontSize)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (fontSize != mInternals->mFontSize) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mFontSize = fontSize;

		// Note updated
		noteUpdated(mPropertyNameFontSize);
	}
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemText::HorizontalPosition CSceneItemText::getHorizontalPosition() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mHorizontalPosition;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemText::setHorizontalPosition(HorizontalPosition horizontalPosition)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check if changed
	if (horizontalPosition != mInternals->mHorizontalPosition) {
		// Prepare to write
		Internals::prepareForWrite(&mInternals);

		// Update
		mInternals->mHorizontalPosition = horizontalPosition;

		// Note updated
		noteUpdated(mPropertyNameHorizontalPosition);
	}
}
