//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemAnimation.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CAudioInfo.h"
#include "CKeyframeAnimationInfo.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemAnimation

class CSceneItemAnimation : public CSceneItem {
	// Constants
	public:
		// 0: Loop Forever
		// 1 - 0xFFFFFFFE: Loop that many times and then hold last frame
		static	const	UInt32	kLoopForever = 0;

	// PropertyType
	public:
		enum PropertyType {
			kPropertyTypeLoopCount	= MAKE_OSTYPE('L', 'p', 'C', 't'),
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
												// Lifecycle methods
												CSceneItemAnimation();
												CSceneItemAnimation(const CDictionary& info);
												CSceneItemAnimation(const CSceneItemAnimation& other);
												~CSceneItemAnimation();

												// CSceneItem methods
				CSceneItem*						copy() const
													{ return new CSceneItemAnimation(*this); }

		const	CString&						getType() const
													{ return mType; }
				TMArray<CDictionary>			getProperties() const;
				CDictionary						getInfo() const;

												// Instance methods
		const	OI<CActions>&					getStartedActions() const;
				void							setStartedActions(const OI<CActions>& actions);

		const	OI<CActions>&					getFinishedActions() const;
				void							setFinishedActions(const OI<CActions>& actions);

		const	OI<CAudioInfo>&					getAudioInfo() const;
				void							setAudioInfo(const OI<CAudioInfo>& audioInfo);

//		const	OI<CCelAnimationInfo>&			getCelAnimationInfo() const;
//				void							setCelAnimationInfo(const OI<CCelAnimationInfo>& celAnimationInfo);

		const	OI<CKeyframeAnimationInfo>&		getKeyframeAnimationInfo() const;
				void							setKeyframeAnimationInfo(
														const OI<CKeyframeAnimationInfo>& keyframeAnimationInfo);

				UInt32							getLoopCount() const;
				void							setLoopCount(UInt32 loopCount);

		const	OV<UniversalTimeInterval>&		getStartTimeInterval() const;
				void							setStartTimeInterval(
														const OV<UniversalTimeInterval>& startTimeInterval);

	// Properties
	public:
		static	CString		mType;

	private:
				Internals*	mInternals;
};
