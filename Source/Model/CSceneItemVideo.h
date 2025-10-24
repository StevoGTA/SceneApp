//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemVideo.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

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

	// Classes
	private:
		class Internals;

	// Methods
	public:
												// Lifecycle methods
												CSceneItemVideo(const CDictionary& info);
												CSceneItemVideo(const CSceneItemVideo& other);
												~CSceneItemVideo();

												// CSceneItem methods
		const	CString&						getType() const
													{ return mType; }
				TMArray<CDictionary>			getProperties() const;
				CDictionary						getInfo() const;

												// Instance methods
				ControlMode						getControlMode() const;
				void							setControlMode(ControlMode controlMode);

		const	OV<CString>&					getResourceFilename() const;
				void							setResourceFilename(const OV<CString>& resourceFilename);

		const	OV<CActions>&					getFinishedActions() const;
				void							setFinishedActions(const OV<CActions>& actions);

		const	S2DRectF32&						getScreenRect() const;
				void							setScreenRect(const S2DRectF32& rect);

		const	OV<UniversalTimeInterval>&		getStartTimeInterval() const;
				void							setStartTimeInterval(OV<UniversalTimeInterval> startTimeInterval);

	// Properties
	public:
		static	const	CString		mType;

		static	const	CString		mPropertyNameControlMode;
		static	const	CString		mPropertyNameFilename;
		static	const	CString		mPropertyNameFinishedActions;
		static	const	CString		mPropertyNameHotspots;
		static	const	CString		mPropertyNameInBackgroundImageFilename;
		static	const	CString		mPropertyNameScreenRect;
		static	const	CString		mPropertyNameStartTimeInterval;

	private:
				Internals*	mInternals;
};
