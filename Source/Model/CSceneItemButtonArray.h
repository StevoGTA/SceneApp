//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButtonArray.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CSceneItem.h"

#include "C2DGeometry.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Property info

enum {
	kSceneItemButtonArrayPropertyTypeButtonArray	= MAKE_OSTYPE('S', 'B', 'B', 'A'),
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArrayButton

class CSceneItemButtonArrayButtonInternals;
class CSceneItemButtonArrayButton {
	// Methods
	public:
													// Lifecycle methods
													CSceneItemButtonArrayButton();
													CSceneItemButtonArrayButton(const CDictionary& info);
													CSceneItemButtonArrayButton(
															const CSceneItemButtonArrayButton& other);
													~CSceneItemButtonArrayButton();

													// Instance methods
						TArray<CDictionary>			getProperties() const;
						CDictionary					getInfo() const;

				const	OI<CActions>&				getActions() const;
						void						setActions(const OI<CActions>& actions);

				const	S2DRectF32&					getUpImageRect() const;
						void						setUpImageRect(const S2DRectF32& upImageRect);

				const	S2DRectF32&					getDownImageRect() const;
						void						setDownImageRect(const S2DRectF32& downImageRect);

				const	S2DPointF32&				getScreenPositionPoint() const;
						void						setScreenPositionPoint(const S2DPointF32& screenPositionPoint);

													// Class methods
		static			CSceneItemButtonArrayButton	makeFrom(const CDictionary& info)
														{ return CSceneItemButtonArrayButton(info); }
		static			CDictionary					getInfoFrom(
															const CSceneItemButtonArrayButton&
																	sceneItemButtonArrayButton)
														{ return sceneItemButtonArrayButton.getInfo(); }

	// Properties
	private:
		CSceneItemButtonArrayButtonInternals*	mInternals;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButtonArray

class CSceneItemButtonArrayInternals;
class CSceneItemButtonArray : public CSceneItem {
	// Methods
	public:
														// Lifecycle methods
														CSceneItemButtonArray();
														CSceneItemButtonArray(const CDictionary& info);
														CSceneItemButtonArray(
																const CSceneItemButtonArray& other);
														~CSceneItemButtonArray();

														// CSceneItem methods
				CSceneItemButtonArray*					copy() const
															{ return new CSceneItemButtonArray(*this); }

		const	CString&								getType() const
															{ return mType; }
				TArray<CDictionary>						getProperties() const;
				CDictionary								getInfo() const;

														// Instance methods
		const	OV<UniversalTimeInterval>&				getStartTimeInterval() const;
				void									setStartTimeInterval(
																const OV<UniversalTimeInterval>& startTimeInterval);

		const	CString&								getImageResourceFilename() const;
				void									setImageResourceFilename(const CString& imageResourceFilename);

		const	TArray<CSceneItemButtonArrayButton>&	getSceneItemButtonArrayButtons() const;

	// Properties
	public:
		static	CString							mType;

	private:
				CSceneItemButtonArrayInternals*	mInternals;
};
