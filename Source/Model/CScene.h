//----------------------------------------------------------------------------------------------------------------------
//	CScene.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CAudioInfo.h"
#include "C2DGeometry.h"
#include "CSceneItem.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CScene

class CScene : public CEquatable {
	// Types
	public:
		typedef SInt32 Index;

	// Constants
	public:
		const	Index	kIndexLastViewed = -1;

	// Options
	public:
		enum Options {
			kOptionsNone		= 0,
			kOptionsDontUnload	= 1 << 0,
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
												// Lifecycle methods
												CScene(const CDictionary& info);
												CScene(const CScene& other);
												~CScene();

												// CEquatable methods
						bool					operator==(const CEquatable& other) const;

												// Instance methods
						TArray<CDictionary>		getProperties() const;
						CDictionary				getInfo() const;

				const	CString&				getName() const;
						void					setName(const CString& name);

						Options					getOptions() const;
						void					setOptions(Options options);

				const	CString&				getStoreSceneIndexAsString() const;
						void					setStoreSceneIndexAsString(const CString& string);

				const	S2DRectF32&				getBoundsRect() const;
						void					setBoundsRect(const S2DRectF32& rect);

				const	OI<CAudioInfo>&			getBackground1AudioInfo() const;
						void					setBackground1AudioInfo(const OI<CAudioInfo>& audioInfo);

				const	OI<CAudioInfo>&			getBackground2AudioInfo() const;
						void					setBackground2AudioInfo(const OI<CAudioInfo>& audioInfo);

				const	OI<CActions>&			getDoubleTapActions() const;
						void					setDoubleTapActions(const OI<CActions>& doubleTapActions);

				const	TArray<I<CSceneItem> >&	getSceneItems() const;

												// Class methods
		static			CScene					makeFrom(const CDictionary& info)
													{ return CScene(info); }
		static			CDictionary				getInfoFrom(const CScene& scene)
													{ return scene.getInfo(); }

	// Properties
	private:
		Internals*	mInternals;
};
