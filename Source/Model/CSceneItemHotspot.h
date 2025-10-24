//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemHotspot.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "C2DGeometry.h"
#include "CAction.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemHotspot

class CSceneItemHotspot : public CSceneItem {
	// Classes
	private:
		class Internals;

	// Methods
	public:
										// Lifecycle methods
										CSceneItemHotspot(const CDictionary& info);
										CSceneItemHotspot(const CSceneItemHotspot& other);
										~CSceneItemHotspot();

										// CSceneItem methods
		const	CString&				getType() const
											{ return mType; }
				TMArray<CDictionary>	getProperties() const;
				CDictionary				getInfo() const;

										// Instance methods
		const	OV<CActions>&			getActions() const;
				void					setActions(const OV<CActions>& actions);

		const	S2DRectF32&				getScreenRect() const;
				void					setScreenRect(const S2DRectF32& rect);

	// Properties
	public:
		static	const	CString		mType;

		static	const	CString		mPropertyNameActions;
		static	const	CString		mPropertyNameScreenRects;

	private:
						Internals*	mInternals;
};
