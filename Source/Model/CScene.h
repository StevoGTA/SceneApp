//----------------------------------------------------------------------------------------------------------------------
//	CScene.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
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

			const	S2DRect32&			getBoundsRect() const;
					void				setBoundsRect(const S2DRect32& rect);

			const	OO<CActions>&		getDoubleTapActions() const;
					void				setDoubleTapActions(const OO<CActions>& doubleTapActions);

					ESceneOptions		getOptions() const;
					void				setOptions(ESceneOptions options);

			const	CString&			getStoreSceneIndexAsString() const;
					void				setStoreSceneIndexAsString(const CString& string);

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
