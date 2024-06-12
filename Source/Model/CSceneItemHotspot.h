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
										CSceneItemHotspot();
										CSceneItemHotspot(const CDictionary& info);
										CSceneItemHotspot(const CSceneItemHotspot& other);
										~CSceneItemHotspot();

										// CSceneItem methods
				CSceneItemHotspot*		copy() const
											{ return new CSceneItemHotspot(*this); }

		const	CString&				getType() const
											{ return mType; }
				TMArray<CDictionary>	getProperties() const;
				CDictionary				getInfo() const;

										// Instance methods
		const	OI<CActions>&			getActions() const;
				void					setActions(const OI<CActions>& actions);

		const	S2DRectF32&				getScreenRect() const;
				void					setScreenRect(const S2DRectF32& rect);

	// Properties
	public:
		static	CString		mType;

	private:
				Internals*	mInternals;
};
