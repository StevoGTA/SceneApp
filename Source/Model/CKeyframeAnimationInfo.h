//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationInfo.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAnimationKeyframe.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CKeyframeAnimationInfo

class CKeyframeAnimationInfoInternals;
class CKeyframeAnimationInfo {
	// Enums
	public:
		enum PropertyType {
			kPropertyTypeKeyframeAnimationInfo	= MAKE_OSTYPE('K', 'e', 'A', 'I'),

			kPropertyTypeKeyframeArray			= MAKE_OSTYPE('K', 'A', 'I', 'K'),
		};

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
