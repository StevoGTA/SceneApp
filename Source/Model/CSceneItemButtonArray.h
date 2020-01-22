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
									CSceneItemButtonArrayButton(const CSceneItemButtonArrayButton& other);
									~CSceneItemButtonArrayButton();

									// Instance methods
				TArray<CDictionary>	getProperties() const;
				CDictionary			getInfo();

				OR<CActionArray>	getActionArray() const;
				void				setActionArray(const CActionArray& actionArray);

		const	S2DRect32&			getUpImageRect() const;
				void				setUpImageRect(const S2DRect32& upImageRect);

		const	S2DRect32&			getDownImageRect() const;
				void				setDownImageRect(const S2DRect32& downImageRect);

		const	S2DPoint32&			getScreenPositionPoint() const;
				void				setScreenPositionPoint(const S2DPoint32& screenPositionPoint);

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
				CSceneItemButtonArray*						copy() const
																{ return new CSceneItemButtonArray(*this); }

		const	CString&									getType() const
																{ return mType; }
				TArray<CDictionary>							getProperties() const;
				CDictionary									getInfo();

															// Instance methods
				UniversalTimeInterval						getStartTimeInterval() const;
				void										setStartTimeInterval(
																	UniversalTimeInterval startTimeInterval);

		const	CString&									getImageResourceFilename() const;
				void										setImageResourceFilename(
																	const CString& imageResourceFilename);

		const	TPtrArray<CSceneItemButtonArrayButton*>&	getSceneButtonArrayButtonArray() const;

	// Properties
	public:
		static	CString							mType;

	private:
				CSceneItemButtonArrayInternals*	mInternals;
};
