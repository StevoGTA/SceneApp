//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CGPURenderObject.h"
#include "CSceneItem.h"
#include "SSceneAppResourceManagementInfo.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayer

class CSceneItemPlayer {
	// Procs
	public:
		struct Procs {
			// Procs
			public:
				typedef	I<CSceneItemPlayer>			(*AddSceneItemProc)(CSceneItem& sceneItem, CGPU& gpu,
															void* userData);
				typedef	I<CSceneItemPlayer>			(*AddSceneItemPlayerProc)(
															const I<CSceneItemPlayer>& sceneItemPlayer, CGPU& gpu,
															void* userData);
				typedef	OR<I<CSceneItemPlayer> >	(*GetSceneItemPlayerProc)(const CSceneItem& sceneItem,
															void* userData);
				typedef	void						(*RemoveSceneItemPlayerProc)(
															const I<CSceneItemPlayer>& sceneItemPlayer, void* userData);

				typedef	void						(*PerformActionsProc)(const CActions& actions,
															const S2DPointF32& point, void* userData);

			// Methods
			public:
											// Lifecycle methods
											Procs(AddSceneItemProc addSceneItemProc,
													AddSceneItemPlayerProc addSceneItemPlayerProc,
													GetSceneItemPlayerProc getSceneItemPlayerProc,
													RemoveSceneItemPlayerProc removeSceneItemPlayerProc,
													PerformActionsProc performActionsProc, void* userData) :
												mAddSceneItemProc(addSceneItemProc),
														mAddSceneItemPlayerProc(addSceneItemPlayerProc),
														mGetSceneItemPlayerProc(getSceneItemPlayerProc),
														mRemoveSceneItemPlayerProc(removeSceneItemPlayerProc),
														mPerformActionsProc(performActionsProc), mUserData(userData)
												{}
											Procs(const Procs& other) :
												mAddSceneItemProc(other.mAddSceneItemProc),
														mAddSceneItemPlayerProc(other.mAddSceneItemPlayerProc),
														mGetSceneItemPlayerProc(other.mGetSceneItemPlayerProc),
														mRemoveSceneItemPlayerProc(other.mRemoveSceneItemPlayerProc),
														mPerformActionsProc(other.mPerformActionsProc),
														mUserData(other.mUserData)
												{}

											// Instance methods
				I<CSceneItemPlayer>			addSceneItem(CSceneItem& sceneItem, CGPU& gpu) const
												{ return mAddSceneItemProc(sceneItem, gpu, mUserData); }
				I<CSceneItemPlayer>			addSceneItemPlayer(const I<CSceneItemPlayer>& sceneItemPlayer, CGPU& gpu)
													const
												{ return mAddSceneItemPlayerProc(sceneItemPlayer, gpu, mUserData); }
				OR<I<CSceneItemPlayer> >	getSceneItemPlayer(const CSceneItem& sceneItem) const
												{ return mGetSceneItemPlayerProc(sceneItem, mUserData); }
				void						removeSceneItemPlayer(const I<CSceneItemPlayer>& sceneItemPlayer) const
												{ mRemoveSceneItemPlayerProc(sceneItemPlayer, mUserData); }

				void						performActions(const CActions& actions,
													const S2DPointF32& point = S2DPointF32()) const
												{ mPerformActionsProc(actions, point, mUserData); }

			// Properties
			private:
				AddSceneItemProc			mAddSceneItemProc;
				AddSceneItemPlayerProc		mAddSceneItemPlayerProc;
				GetSceneItemPlayerProc		mGetSceneItemPlayerProc;
				RemoveSceneItemPlayerProc	mRemoveSceneItemPlayerProc;

				PerformActionsProc			mPerformActionsProc;

				void*						mUserData;
		};

	// Procs
	public:
		typedef	CSceneItemPlayer*	(*CreateProc)(const CSceneItem& sceneItem,
											const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
											const Procs& procs);

	// Classes
	private:
		class Internals;

	// Methods
	public:
													// Lifecycle methods
													CSceneItemPlayer(CSceneItem& sceneItem, const Procs& procs);
													CSceneItemPlayer(const CString& name,
															CSceneItem::Options sceneItemOptions,
															bool isInitiallyVisible, const Procs& procs);
													CSceneItemPlayer(const Procs& procs);
		virtual										~CSceneItemPlayer();

													// Instance methods
				const	CString&					getName() const;
						CSceneItem::Options			getSceneItemOptions() const;
				const	OR<CSceneItem>&				getSceneItem() const;

		virtual			bool						getIsVisible() const;
		virtual			void						setIsVisible(bool isVisible);

				const	S2DOffsetF32&				getScreenPositionOffset() const;
						void						setScreenPositionOffset(const S2DOffsetF32& screenPositionOffset);

		virtual			CActions					getAllActions() const;
						void						perform(const CActions& actions,
															const S2DPointF32& point = S2DPointF32());

		virtual			void						load(CGPU& gpu);
		virtual			void						allPeersHaveLoaded();
		virtual			void						unload();

		virtual	const	OV<UniversalTimeInterval>&	getStartTimeInterval() const;

		virtual			void						reset();
		virtual			void						update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval,
															bool isRunning);
		virtual			void						render(CGPU& gpu, const CGPURenderObject::RenderInfo& renderInfo)
															const {}

		virtual			bool						handlesTouchOrMouseAtPoint(const S2DPointF32& point) const;
		virtual			void						touchBeganOrMouseDownAtPoint(const S2DPointF32& point,
															UInt32 tapOrClickCount, const void* reference);
		virtual			void						touchOrMouseMovedFromPoint(const S2DPointF32& point1,
															const S2DPointF32& point2, const void* reference);
		virtual			void						touchEndedOrMouseUpAtPoint(const S2DPointF32& point,
															const void* reference);
		virtual			void						touchOrMouseCancelled(const void* reference);

		virtual			void						shakeBegan();
		virtual			void						shakeEnded();
		virtual			void						shakeCancelled();

		virtual			void						setProperty(const CString& property, const SValue& value);
		virtual			bool						handleCommand(CGPU& gpu, const CString& command,
															const CDictionary& commandInfo, const S2DPointF32& point);

													// Subclass methods
						void						setPeerProperty(const CString& sceneName, const CString& name,
															const CString& property, const SValue& value) const;
						void						setPeerProperty(const CString& name, const CString& property,
															const SValue& value) const;
						void						sendPeerCommand(const CString& name, const CString& command) const;

	protected:
													// Subclass methods
						I<CSceneItemPlayer>			add(CSceneItem& sceneItem, CGPU& gpu);
						I<CSceneItemPlayer>			add(const I<CSceneItemPlayer>& sceneItemPlayer, CGPU& gpu);
						OR<I<CSceneItemPlayer> >	getSceneItemPlayer(const CSceneItem& sceneItem);
						void						remove(const I<CSceneItemPlayer>& sceneItemPlayer);

		virtual			void						noteSceneItemUpdated(const CString& propertyName) {}

	// Properties
	public:
		static	CString		mIsVisiblePropertyName;		// bool or 1

		static	CString		mCommandNameLoad;
		static	CString		mCommandNameUnload;
		static	CString		mCommandNameReset;

	private:
				Internals*	mInternals;
};
