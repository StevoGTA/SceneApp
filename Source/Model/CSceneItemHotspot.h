//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemHotspot.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CSceneItem.h"

#include "C2DGeometry.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemHotspot

class CSceneItemHotspotInternals;
class CSceneItemHotspot : public CSceneItem {
	// Methods
	public:
									// Lifecycle methods
									CSceneItemHotspot();
									CSceneItemHotspot(const CDictionary& info);
									CSceneItemHotspot(const CSceneItemHotspot& other);
									~CSceneItemHotspot();

									// CSceneItem methods
				CSceneItemHotspot*	copy() const
										{ return new CSceneItemHotspot(*this); }

		const	CString&			getType() const
										{ return mType; }
				TArray<CDictionary>	getProperties() const;
				CDictionary			getInfo() const;

									// Instance methods
				OR<CActionArray>	getActionArray() const;
				void				setActionArray(const CActionArray& actionArray);

		const	S2DRect32&			getScreenRect() const;
				void				setScreenRect(const S2DRect32& rect);

	// Properties
	public:
		static	CString						mType;

	private:
				CSceneItemHotspotInternals*	mInternals;
};
