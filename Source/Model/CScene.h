//----------------------------------------------------------------------------------------------------------------------
//	CScene.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "C2DGeometry.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Scene Index values

typedef SInt32 SceneIndex;

const	SceneIndex	kSceneIndexLastViewed = -1;

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
									~CScene();

									// CEquatable methods
				bool				operator==(const CEquatable& other) const
										{ return this == &other; }

									// Instance methods
				TArray<CDictionary>	getProperties() const;
				CDictionary			getInfo() const;

		const	CString&			getName() const;
				void				setName(const CString& name);

		const	S2DRect32&			getBoundsRect() const;
				void				setBoundsRect(const S2DRect32& rect);

				OR<CActionArray>	getDoubleTapActionArray() const;
				void				setDoubleTapActionArray(OR<CActionArray> doubleTapActionArray);

				ESceneOptions		getOptions() const;
				void				setOptions(ESceneOptions options);

		const	CString&			getStoreSceneIndexAsString() const;
				void				setStoreSceneIndexAsString(const CString& string);

				UInt32				getSceneItemsCount() const;
				CSceneItem&			getSceneItemAtIndex(UInt32 index) const;

	// Properties
	private:
		CSceneInternals*	mInternals;
};
