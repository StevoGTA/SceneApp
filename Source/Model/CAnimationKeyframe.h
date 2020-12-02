//----------------------------------------------------------------------------------------------------------------------
//	CAnimationKeyframe.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"

#include "C2DGeometry.h"
#include "TimeAndDate.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Frame transition types

enum EAnimationKeyframeTransitionType {
	// Values hold current values and then snap to next values
	kAnimationKeyframeTransitionTypeJump			= 1,
	
	// Values change linearly to next values
	kAnimationKeyframeTransitionTypeLinear			= 2,
	
	// Values change linearly for 1st half and slow down for 2nd half
	kAnimationKeyframeTransitionTypeEaseIn			= 3,
	
	// Values speed up for 1st half and change linearly for 2nd half
	kAnimationKeyframeTransitionTypeEaseOut			= 4,
	
	// Values speed up for 1st half and slow down for 2nd half
	kAnimationKeyframeTransitionTypeEaseInEaseOut	= 5,
	
	// Hold until scene is started
	kAnimationKeyframeTransitionTypeHoldUntilStart	= 6,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Options

enum EAnimationKeyframeOptions {
	kAnimationKeyframeOptionsNone					= 0,

//	kAnimationKeyframeOptionsTextureWillChangeAlpha	= 1 << 0,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Property info

enum EAnimationKeyframePropertyType {
	kAnimationKeyframePropertyTypeAnchorPoint			= MAKE_OSTYPE('A', 'K', 'A', 'P'),
	kAnimationKeyframePropertyTypeScreenPositionPoint	= MAKE_OSTYPE('A', 'K', 'S', 'P'),
	kAnimationKeyframePropertyTypeAngle					= MAKE_OSTYPE('A', 'K', 'A', 'n'),
	kAnimationKeyframePropertyTypeAlpha					= MAKE_OSTYPE('A', 'K', 'A', 'l'),
	kAnimationKeyframePropertyTypeScale					= MAKE_OSTYPE('A', 'K', 'S', 'c'),
	kAnimationKeyframePropertyTypeDelay					= MAKE_OSTYPE('A', 'K', 'D', 'e'),
	kAnimationKeyframePropertyTypeTransitionType		= MAKE_OSTYPE('A', 'K', 'T', 'T'),
	kAnimationKeyframePropertyTypeImageFilename			= MAKE_OSTYPE('A', 'K', 'I', 'F'),
	kAnimationKeyframePropertyTypeOptions				= MAKE_OSTYPE('A', 'K', 'O', 'p'),
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAnimationKeyframe

class CAnimationKeyframeInternals;
class CAnimationKeyframe {
	// Methods
	public:
														// Lifecycle methods
														CAnimationKeyframe();
														CAnimationKeyframe(const CDictionary& info);
														CAnimationKeyframe(const CAnimationKeyframe& other);
														~CAnimationKeyframe();

														// Instance methods
				TArray<CDictionary>						getProperties() const;
				CDictionary								getInfo() const;

		const	OI<CActions>&							getActions() const;
				void									setActions(const OI<CActions>& actions);

		const	OV<S2DPointF32>&						getAnchorPoint() const;
				void									setAnchorPoint(const OV<S2DPointF32>& anchorPoint);

		const	OV<S2DPointF32>&						getScreenPositionPoint() const;
				void									setScreenPositionPoint(
																const OV<S2DPointF32>& screenPositionPoint);

		const	OV<Float32>&							getAngleDegrees() const;
				void									setAngleDegrees(const OV<Float32>& angleDegrees);

		const	OV<Float32>&							getAlpha() const;
				void									setAlpha(const OV<Float32>& alpha);

		const	OV<Float32>&							getScale() const;
				void									setScale(const OV<Float32>& scale);

		const	OV<UniversalTimeInterval>&				getDelay() const;
				void									setDelay(const OV<UniversalTimeInterval>& delay);

		const	OV<EAnimationKeyframeTransitionType>&	getTransitionType() const;
				void									setTransitionType(
																const OV<EAnimationKeyframeTransitionType>&
																		transitionType);

		const	OV<CString>&							getImageResourceFilename() const;
				void									setImageResourceFilename(
																const OV<CString>& imageResourceFilename);

				EAnimationKeyframeOptions				getOptions() const;
				void									setOptions(EAnimationKeyframeOptions options);

	// Properties
	private:
		CAnimationKeyframeInternals*	mInternals;
};
