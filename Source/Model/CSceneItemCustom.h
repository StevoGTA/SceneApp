//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemCustom.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAudioInfo.h"
#include "CSceneItemAnimation.h"
#include "CSceneItemButton.h"
//#include "CSceneItemText.h"

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
				CDictionary				getInfo() const;

										// Instance methods
				bool					contains(const CString& key) const;

				bool					getBool(const CString& key) const;
				CDictionary				getDictionary(const CString& key) const;
				CString					getString(const CString& key) const;
				Float32					getFloat32(const CString& key) const;
				S2DPointF32				get2DPoint(const CString& key) const;
				S2DRectF32				get2DRect(const CString& key) const;
				UInt32					getUInt32(const CString& key) const;
				UniversalTimeInterval	getUniversalTimeInterval(const CString& key) const;

		const	CActions&				getActions(const CString& key) const;
		const	CAudioInfo&				getAudioInfo(const CString& key) const;
		const	CSceneItemAnimation&	getSceneItemAnimation(const CString& key) const;
		const	CSceneItemButton&		getSceneItemButton(const CString& key) const;
//		const	CSceneItemText&			getSceneItemText(const CString& key) const;

	// Properties
	private:
		CSceneItemCustomInternals*	mInternals;
};
