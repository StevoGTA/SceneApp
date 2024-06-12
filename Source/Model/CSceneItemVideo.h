//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemVideo.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

//#include "CSceneItemHotspot.h"
#include "C2DGeometry.h"
#include "CAction.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemVideo

class CSceneItemVideo : public CSceneItem {
	// ControlMode
	public:
		enum ControlMode {
			kControlModeHidden		= 0,
			kControlModeAllControls	= 1,

			kControlModeDefault = kControlModeHidden,
		};

	// PropertyType
	public:
		enum PropertyType {
//			kPropertyTypeControlMode	= MAKE_OSTYPE('C', 'l', 'M', 'd'),
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
												// Lifecycle methods
												CSceneItemVideo();
												CSceneItemVideo(const CDictionary& info);
												CSceneItemVideo(const CSceneItemVideo& other);
												~CSceneItemVideo();

												// CSceneItem methods
				CSceneItem*						copy() const
													{ return new CSceneItemVideo(*this); }

		const	CString&						getType() const
													{ return mType; }
				TMArray<CDictionary>			getProperties() const;
				CDictionary						getInfo() const;

												// Instance methods
				ControlMode						getControlMode() const;
				void							setControlMode(ControlMode controlMode);

		const	OI<CString>&					getResourceFilename() const;
				void							setResourceFilename(const OI<CString>& resourceFilename);

		const	OI<CActions>&					getFinishedActions() const;
				void							setFinishedActions(const OI<CActions>& actions);

//		const	TPtrArray<CSceneItemHotspot*>&	getSceneHotspotsArray() const;

//		const	CString&						getInImageResourceFilename() const;
//				void							setInImageResourceFilename(
//														const CString& inImageResourceFilename);

		const	S2DRectF32&						getScreenRect() const;
				void							setScreenRect(const S2DRectF32& rect);

		const	OV<UniversalTimeInterval>&		getStartTimeInterval() const;
				void							setStartTimeInterval(OV<UniversalTimeInterval> startTimeInterval);

	// Properties
	public:
		static	CString		mType;

	private:
				Internals*	mInternals;
};
