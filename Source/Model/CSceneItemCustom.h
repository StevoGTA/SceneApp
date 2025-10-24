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

class CSceneItemCustom : public CSceneItem {
	// Classes
	private:
		class Internals;

	// Methods
	public:
										// Lifecycle methods
										CSceneItemCustom(const CString& type, const CDictionary& info);
										CSceneItemCustom(const CSceneItemCustom& other);
										~CSceneItemCustom();

										// CSceneItem methods
				CString&				getType() const;
				CDictionary				getInfo() const;

										// Instance methods
				bool					contains(const CString& key) const;

				bool					getBool(const CString& key) const;
		const	TArray<CString>&		getArrayOfStrings(const CString& key) const;
		const	CDictionary&			getDictionary(const CString& key) const;
		const	CString&				getString(const CString& key) const;
				Float32					getFloat32(const CString& key) const;
				S2DPointF32				get2DPoint(const CString& key) const;
				S2DRectF32				get2DRect(const CString& key) const;
				UInt32					getUInt32(const CString& key) const;
				UniversalTimeInterval	getUniversalTimeInterval(const CString& key) const;

		const	CActions&				getActions(const CString& key) const;
		const	CAudioInfo&				getAudioInfo(const CString& key) const;
				CSceneItemAnimation&	getSceneItemAnimation(const CString& key) const;
				CSceneItemButton&		getSceneItemButton(const CString& key) const;
				CSceneItemText&			getSceneItemText(const CString& key) const;

	// Properties
	private:
		Internals*	mInternals;
};
