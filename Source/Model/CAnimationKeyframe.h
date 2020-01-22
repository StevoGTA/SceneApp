//----------------------------------------------------------------------------------------------------------------------
//	CAnimationKeyframe.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"

#include "C2DGeometry.h"
#include "TimeAndDate.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Point values

const	S2DPoint32	kAnimationKeyframeAnchorPointUseCurrent = {INFINITY, INFINITY};
const	S2DPoint32	kAnimationKeyframeScreenPositionPointUseCurrent = {INFINITY, INFINITY};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Angle values

// Don't change the angle at this keyframe
const	Float32	kAnimationKeyframeAngleDegreesUseCurrent = INFINITY;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Alpha values

// Don't change the alpha at this keyframe
const	Float32	kAnimationKeyframeAlphaUseCurrent = -1.0;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Scale values

// Don't change the scale at this keyframe
const	Float32	kAnimationKeyframeScaleUseCurrent = -1.0;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Delay values

const	UniversalTimeInterval	kAnimationKeyframeDelayForever = -1.0;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Frame transition types

enum EAnimationKeyframeTransitionType {
	// No transition and values stay constant
	kAnimationKeyframeTransitionTypeNone			= 0,
	
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

	kAnimationKeyframeOptionsTextureWillChangeAlpha	= 1 << 0,
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
				TArray<CDictionary>					getProperties() const;
				CDictionary							getInfo() const;

				OR<CActionArray>					getActionArray() const;
				void								setActionArray(OR<CActionArray> actionArray);

				bool								hasAnchorPoint() const;
		const	S2DPoint32&							getAnchorPoint() const;
				void								setAnchorPoint(const S2DPoint32& anchorPoint);

				bool								hasScreenPositionPoint() const;
		const	S2DPoint32&							getScreenPositionPoint() const;
				void								setScreenPositionPoint(const S2DPoint32& screenPositionPoint);

				bool								hasAngleDegrees() const;
				Float32								getAngleDegrees() const;
				void								setAngleDegrees(Float32 angleDegrees);

				bool								hasAlpha() const;
				Float32								getAlpha() const;
				void								setAlpha(Float32 alpha);

				bool								hasScale() const;
				Float32								getScale() const;
				void								setScale(Float32 scale);

				UniversalTimeInterval				getDelay() const;
				void								setDelay(UniversalTimeInterval delay);

				bool								hasTransitionType() const;
				EAnimationKeyframeTransitionType	getTransitionType() const;
				void								setTransitionType(EAnimationKeyframeTransitionType transitionType);

				bool								hasImageResourceFilename() const;
		const	CString&							getImageResourceFilename() const;
				void								setImageResourceFilename(const CString& imageResourceFilename);

				EAnimationKeyframeOptions			getOptions() const;
				void								setOptions(EAnimationKeyframeOptions options);

	// Properties
	private:
		CAnimationKeyframeInternals*	mInternals;
};
