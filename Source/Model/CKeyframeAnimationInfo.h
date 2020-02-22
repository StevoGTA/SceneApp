//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationInfo.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAnimationKeyframe.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Property info

enum {
	kSceneItemPropertyTypeKeyframeAnimationInfo	= MAKE_OSTYPE('K', 'e', 'A', 'I'),
	
	kKeyframeAnimationPropertyTypeKeyframeArray	= MAKE_OSTYPE('K', 'A', 'I', 'K'),
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CKeyframeAnimationInfo

class CKeyframeAnimationInfoInternals;
class CKeyframeAnimationInfo {
	// Methods
	public:
											// Lifecycle methods
											CKeyframeAnimationInfo();
											CKeyframeAnimationInfo(const CDictionary& info);
											CKeyframeAnimationInfo(const CKeyframeAnimationInfo& other);
											~CKeyframeAnimationInfo();

											// Instance methods
				TArray<CDictionary>			getProperties() const;
				CDictionary					getInfo() const;

		const	TArray<CAnimationKeyframe>&	getAnimationKeyframesArray() const;
				void						addAnimationKeyframe(const CAnimationKeyframe& animationKeyframe);

	// Properties
	private:
		CKeyframeAnimationInfoInternals*	mInternals;
};
