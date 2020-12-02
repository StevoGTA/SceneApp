//----------------------------------------------------------------------------------------------------------------------
//	CScene.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CAudioInfo.h"
#include "C2DGeometry.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Scene Index values

typedef SInt32 CSceneIndex;

const	CSceneIndex	kSceneIndexLastViewed = -1;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Scene Options

enum ESceneOptions {
	kSceneOptionsNone		= 0,
	kSceneOptionsDontUnload	= 1 << 0,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CScene

class CSceneInternals;
class CScene : public CEquatable {
	// Methods
	public:
										// Lifecycle methods
										CScene();
										CScene(const CDictionary& info);
										CScene(const CScene& other);
										~CScene();

										// CEquatable methods
					bool				operator==(const CEquatable& other) const;

										// Instance methods
					TArray<CDictionary>	getProperties() const;
					CDictionary			getInfo() const;

			const	CString&			getName() const;
					void				setName(const CString& name);

					ESceneOptions		getOptions() const;
					void				setOptions(ESceneOptions options);

			const	CString&			getStoreSceneIndexAsString() const;
					void				setStoreSceneIndexAsString(const CString& string);

			const	S2DRectF32&			getBoundsRect() const;
					void				setBoundsRect(const S2DRectF32& rect);

			const	OI<CAudioInfo>&		getBackground1AudioInfo() const;
					void				setBackground1AudioInfo(const OI<CAudioInfo>& audioInfo);

			const	OI<CAudioInfo>&		getBackground2AudioInfo() const;
					void				setBackground2AudioInfo(const OI<CAudioInfo>& audioInfo);

			const	OI<CActions>&		getDoubleTapActions() const;
					void				setDoubleTapActions(const OI<CActions>& doubleTapActions);

			const	TArray<CSceneItem>&	getSceneItems() const;

										// Class methods
	static			CScene				makeFrom(const CDictionary& info)
											{ return CScene(info); }
	static			CDictionary			getInfoFrom(const CScene& scene)
											{ return scene.getInfo(); }

	// Properties
	private:
		CSceneInternals*	mInternals;
};
