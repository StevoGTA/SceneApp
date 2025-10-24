//----------------------------------------------------------------------------------------------------------------------
//	CKeyframeAnimationInfo.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAnimationKeyframe.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CKeyframeAnimationInfo

class CKeyframeAnimationInfo {
	// PropertyType
	public:
		enum PropertyType {
			kPropertyTypeKeyframeAnimationInfo	= MAKE_OSTYPE('K', 'e', 'A', 'I'),

			kPropertyTypeKeyframeArray			= MAKE_OSTYPE('K', 'A', 'I', 'K'),
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
											// Lifecycle methods
											CKeyframeAnimationInfo(const CAnimationKeyframe& animationKeyframe);
											CKeyframeAnimationInfo(const CDictionary& info);
											CKeyframeAnimationInfo(const CKeyframeAnimationInfo& other);
											~CKeyframeAnimationInfo();

											// Instance methods
				TArray<CDictionary>			getProperties() const;
				CDictionary					getInfo() const;

		const	TArray<CAnimationKeyframe>&	getAnimationKeyframes() const;
				void						addAnimationKeyframe(const CAnimationKeyframe& animationKeyframe);

				bool						operator==(const CKeyframeAnimationInfo& other) const
												{ return (getAnimationKeyframes() == other.getAnimationKeyframes()); }
				bool						operator!=(const CKeyframeAnimationInfo& other) const
												{ return !(*this == other); }

	// Properties
	private:
		Internals*	mInternals;
};
