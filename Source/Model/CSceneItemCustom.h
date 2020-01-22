//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemCustom.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAudioInfo.h"
#include "CSceneItemAnimation.h"
#include "CSceneItemButton.h"
#include "CSceneItemText.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemCustom

class CSceneItemCustomInternals;
class CSceneItemCustom : public CSceneItem {
	// Methods
	public:
								// Lifecycle methods
								CSceneItemCustom();
								CSceneItemCustom(const CString& type, const CDictionary& info);
								CSceneItemCustom(const CSceneItemCustom& other);
								~CSceneItemCustom();

								// CSceneItem methods
		CSceneItemCustom*		copy() const
									{ return new CSceneItemCustom(*this); }

		CString&				getType() const;
		TArray<CDictionary>		getProperties() const;
		CDictionary				getInfo() const;

								// Instance methods
		bool					hasValueForKey(const CString& key) const;

		bool					getBoolValueForKey(const CString& key) const;
		Float32					getFloat32(const CString& key) const;
		S2DPoint32				get2DPointValueForKey(const CString& key) const;
		S2DRect32				get2DRectValueForKey(const CString& key) const;
		UInt32					getUInt32(const CString& key) const;
		UniversalTimeInterval	getUniversalTimeIntervalValueForKey(const CString& key) const;

		CDictionary				getDictionaryForKey(const CString& key) const;
		CString					getStringForKey(const CString& key) const;

		CActionArray&			getActionArrayForKey(const CString& key);
		CAudioInfo&				getAudioInfoForKey(const CString& key);
		CSceneItemAnimation&	getSceneItemAnimationForKey(const CString& key);
		CSceneItemButton&		getSceneItemButtonForKey(const CString& key);
		CSceneItemText&			getSceneItemTextForKey(const CString& key);

	// Properties
	private:
		CSceneItemCustomInternals*	mInternals;
};
