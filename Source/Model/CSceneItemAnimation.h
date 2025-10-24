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
												CSceneItemAnimation(
														const CKeyframeAnimationInfo& keyframeAnimationInfo);
												CSceneItemAnimation(const CDictionary& info);
												CSceneItemAnimation(const CSceneItemAnimation& other);
												~CSceneItemAnimation();

												// CSceneItem methods
		const	CString&						getType() const
													{ return mType; }
				TMArray<CDictionary>			getProperties() const;
				CDictionary						getInfo() const;

												// Instance methods
		const	OV<CActions>&					getStartedActions() const;
				void							setStartedActions(const OV<CActions>& actions);

		const	OV<CActions>&					getFinishedActions() const;
				void							setFinishedActions(const OV<CActions>& actions);

		const	OV<CAudioInfo>&					getAudioInfo() const;
				void							setAudioInfo(const OV<CAudioInfo>& audioInfo);

		const	OV<CKeyframeAnimationInfo>&		getKeyframeAnimationInfo() const;
				void							setAnimationInfo(const CKeyframeAnimationInfo& keyframeAnimationInfo);

				UInt32							getLoopCount() const;
				void							setLoopCount(UInt32 loopCount);

		const	OV<UniversalTimeInterval>&		getStartTimeInterval() const;
				void							setStartTimeInterval(
														const OV<UniversalTimeInterval>& startTimeInterval);

				bool							operator==(const CSceneItemAnimation& other) const
													{ return mInternals == other.mInternals; }
	// Properties
	public:
		static	const	CString		mType;

		static	const	CString		mPropertyNameStartedActions;
		static	const	CString		mPropertyNameFinishedActions;
		static	const	CString		mPropertyNameAudio;
		static	const	CString		mPropertyNameCelAnimations;
		static	const	CString		mPropertyNameKeyframeAnimations;
		static	const	CString		mPropertyNameLoopCount;
		static	const	CString		mPropertyNameStartTime;

	private:
				Internals*	mInternals;
};
