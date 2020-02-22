//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButton.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CAudioInfo.h"
#include "CKeyframeAnimationInfo.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemButton

class CSceneItemButtonInternals;
class CSceneItemButton : public CSceneItem {
	// Methods
	public:
											// Lifecycle methods
											CSceneItemButton();
											CSceneItemButton(const CDictionary& info);
											CSceneItemButton(const CSceneItemButton& other);
											~CSceneItemButton();

											// CSceneItem methods
				CSceneItemButton*			copy() const
												{ return new CSceneItemButton(*this); }

		const	CString&					getType() const
												{ return mType; }
				TArray<CDictionary>			getProperties() const;
				CDictionary					getInfo() const;

											// Instance methods
		const	OO<CActions>&				getActions() const;
				void						setActions(const OO<CActions>& actions);

		const	OO<CAudioInfo>&				getAudioInfo() const;
				void						setAudioInfo(const OO<CAudioInfo>& audioInfo);

		const	OO<CKeyframeAnimationInfo>&	getUpKeyframeAnimationInfo() const;
				void						setUpKeyframeAnimationInfo(
													const OO<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OO<CKeyframeAnimationInfo>&	getDownKeyframeAnimationInfo() const;
				void						setDownKeyframeAnimationInfo(
													const OO<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OO<CKeyframeAnimationInfo>&	getDisabledKeyframeAnimationInfo() const;
				void						setDisabledKeyframeAnimationInfo(
													const OO<CKeyframeAnimationInfo>& keyframeAnimationInfo);

		const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;
				void						setStartTimeInterval(const OV<UniversalTimeInterval>& startTimeInterval);

				Float32						getHitRadius() const;
				void						setHitRadius(Float32 hitRadius);

	// Properties
	public:
		static	CString						mType;

	private:
				CSceneItemButtonInternals*	mInternals;
};
