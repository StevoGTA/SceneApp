//----------------------------------------------------------------------------------------------------------------------
//	CAnimationKeyframe.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"

#include "C2DGeometry.h"
#include "TimeAndDate.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CAnimationKeyframe

class CAnimationKeyframeInternals;
class CAnimationKeyframe {
	// Enums
	public:
		enum TransitionType {
			// Values hold current values and then snap to next values
			kTransitionTypeJump				= 1,

			// Values change linearly to next values
			kTransitionTypeLinear			= 2,

			// Values change linearly for 1st half and slow down for 2nd half
			kTransitionTypeEaseIn			= 3,

			// Values speed up for 1st half and change linearly for 2nd half
			kTransitionTypeEaseOut			= 4,

			// Values speed up for 1st half and slow down for 2nd half
			kTransitionTypeEaseInEaseOut	= 5,

			// Hold until scene is started
			kTransitionTypeHoldUntilStart	= 6,
		};

		enum Options {
			kOptionsNone					= 0,

		//	kOptionsTextureWillChangeAlpha	= 1 << 0,
		};

		enum PropertyType {
			kPropertyTypeAnchorPoint			= MAKE_OSTYPE('A', 'K', 'A', 'P'),
			kPropertyTypeScreenPositionPoint	= MAKE_OSTYPE('A', 'K', 'S', 'P'),
			kPropertyTypeAngle					= MAKE_OSTYPE('A', 'K', 'A', 'n'),
			kPropertyTypeAlpha					= MAKE_OSTYPE('A', 'K', 'A', 'l'),
			kPropertyTypeScale					= MAKE_OSTYPE('A', 'K', 'S', 'c'),
			kPropertyTypeDelay					= MAKE_OSTYPE('A', 'K', 'D', 'e'),
			kPropertyTypeTransitionType			= MAKE_OSTYPE('A', 'K', 'T', 'T'),
			kPropertyTypeImageFilename			= MAKE_OSTYPE('A', 'K', 'I', 'F'),
			kPropertyTypeOptions				= MAKE_OSTYPE('A', 'K', 'O', 'p'),
		};

	// Methods
	public:
											// Lifecycle methods
											CAnimationKeyframe();
											CAnimationKeyframe(const CDictionary& info);
											CAnimationKeyframe(const CAnimationKeyframe& other);
											~CAnimationKeyframe();

											// Instance methods
				TArray<CDictionary>			getProperties() const;
				CDictionary					getInfo() const;

		const	OI<CActions>&				getActions() const;
				void						setActions(const OI<CActions>& actions);

		const	OV<S2DPointF32>&			getAnchorPoint() const;
				void						setAnchorPoint(const OV<S2DPointF32>& anchorPoint);

		const	OV<S2DPointF32>&			getScreenPositionPoint() const;
				void						setScreenPositionPoint(const OV<S2DPointF32>& screenPositionPoint);

		const	OV<Float32>&				getAngleDegrees() const;
				void						setAngleDegrees(const OV<Float32>& angleDegrees);

		const	OV<Float32>&				getAlpha() const;
				void						setAlpha(const OV<Float32>& alpha);

		const	OV<Float32>&				getScale() const;
				void						setScale(const OV<Float32>& scale);

		const	OV<UniversalTimeInterval>&	getDelay() const;
				void						setDelay(const OV<UniversalTimeInterval>& delay);

		const	OV<TransitionType>&			getTransitionType() const;
				void						setTransitionType(const OV<TransitionType>& transitionType);

		const	OV<CString>&				getImageResourceFilename() const;
				void						setImageResourceFilename(const OV<CString>& imageResourceFilename);

				Options						getOptions() const;
				void						setOptions(Options options);

	// Properties
	private:
		CAnimationKeyframeInternals*	mInternals;
};
