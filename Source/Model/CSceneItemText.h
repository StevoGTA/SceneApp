//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemText.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "C2DGeometry.h"
#include "CColor.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemText

class CSceneItemText : public CSceneItem {
	// Horizontal Position
	public:
		enum HorizontalPosition {
			kHorizontalPositionLeftJustified	= 0,
			kHorizontalPositionCentered			= 1,
			kHorizontalPositionRightJustified	= 2,
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
										// Lifecycle methods
										CSceneItemText(const CString& name = CUUID().getBase64String(),
												bool isInitiallyVisible = true, Options options = kOptionsNone,
												const S2DRectF32 screenRect = S2DRectF32(),
												const CString& text = CString::mEmpty,
												const CColor& textColor = CColor::mBlack,
												const CColor& outlineColor = CColor::mClear, Float32 outlineWidth = 0.0,
												const CString& fontName = CString(OSSTR("Helvetica-Bold")),
												Float32 fontSize = 28.0,
												HorizontalPosition horizontalPosition =
														kHorizontalPositionLeftJustified);
										CSceneItemText(const CDictionary& info);
										CSceneItemText(const CSceneItemText& other);
										~CSceneItemText();

										// CSceneItem methods
		const	CString&				getType() const
											{ return mType; }
				TMArray<CDictionary>	getProperties() const;
				CDictionary				getInfo() const;

										// Instance methods
		const	S2DRectF32&				getScreenRect() const;
				void					setScreenRect(const S2DRectF32& rect);

		const	CString&				getText() const;
				void					setText(const CString& text);

		const	CColor&					getTextColor() const;
				void					setTextColor(const CColor& textColor);

		const	CColor&					getOutlineColor() const;
				void					setOutlineColor(const CColor& outlineColor);

				Float32					getOutlineWidth() const;
				void					setOutlineWidth(Float32 outlineWidth);

		const	CString&				getFontName() const;
				void					setFontName(const CString& fontName);

				Float32					getFontSize() const;
				void					setFontSize(Float32 fontSize);

				HorizontalPosition		getHorizontalPosition() const;
				void					setHorizontalPosition(HorizontalPosition horizontalPosition);

	// Properties
	public:
		static	const	CString		mType;

		static	const	CString		mPropertyNameScreenRect;
		static	const	CString		mPropertyNameText;
		static	const	CString		mPropertyNameTextColor;
		static	const	CString		mPropertyNameOutlineColor;
		static	const	CString		mPropertyNameOutlineWidth;
		static	const	CString		mPropertyNameFontName;
		static	const	CString		mPropertyNameFontSize;
		static	const	CString		mPropertyNameHorizontalPosition;

	private:
						Internals*	mInternals;
};
