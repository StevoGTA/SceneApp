//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemButton.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CAudioInfo.h"
#include "CKeyframeAnimationInfo.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemButton

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
				OR<CActionArray>			getActionArray() const;
				void						setActionArray(const CActionArray& actionArray);

				OR<CAudioInfo>				getAudioInfo() const;
				void						setAudioInfo(const CAudioInfo& audioInfo);

				OR<CKeyframeAnimationInfo>	getUpKeyframeAnimationInfo() const;
				void						setUpKeyframeAnimationInfo(
													const CKeyframeAnimationInfo& keyframeAnimationInfo);

				OR<CKeyframeAnimationInfo>	getDownKeyframeAnimationInfo() const;
				void						setDownKeyframeAnimationInfo(
													const CKeyframeAnimationInfo& keyframeAnimationInfo);

				OR<CKeyframeAnimationInfo>	getDisabledKeyframeAnimationInfo() const;
				void						setDisabledKeyframeAnimationInfo(
													const CKeyframeAnimationInfo& keyframeAnimationInfo);

				UniversalTimeInterval		getStartTimeInterval() const;
				void						setStartTimeInterval(UniversalTimeInterval startTimeInterval);

				Float32						getHitRadius() const;
				void						setHitRadius(Float32 hitRadius);

	// Properties
	public:
		static	CString						mType;

	private:
				CSceneItemButtonInternals*	mInternals;
};
