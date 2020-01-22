//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayer.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CAction.h"
#include "CSceneApp.h"
#include "CSceneItem.h"

//#include "CEvent.h"
//#include "CImageX.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: SSceneItemPlayerProcsInfo

class CSceneItemPlayer;
typedef	void	(*SceneItemPlayerActionArrayPerformProc)(const CActionArray& actionArray,
									const S2DPoint32& point, void* userData);
//typedef OV<CSceneItemPlayer*>	(*SceneItemPlayerCreateSceneItemPlayerForSceneItemProc)(const CSceneItem& sceneItem,
//										bool autoLoad, void* userData);
//typedef void					(*SceneItemPlayerAddSceneItemPlayerProc)(CSceneItemPlayer& sceneItemPlayer,
//										bool autoLoad, void* userData);
//typedef OR<CSceneItemPlayer>	(*SceneItemPlayerGetPeerForSceneItemNameProc)(CSceneItemPlayer& sceneItemPlayer,
//										const CString& sceneItemName, void* userData);
//typedef CImageX				(*SceneItemPlayerGetCurrentViewportImageProc)(const CSceneItemPlayer& sceneItemPlayer,
//									bool performRedraw, void* userData);

struct SSceneItemPlayerProcsInfo {
	// Lifecycle methods
	SSceneItemPlayerProcsInfo(
			SceneItemPlayerActionArrayPerformProc actionArrayPerformProc,
//			SceneItemPlayerCreateSceneItemPlayerForSceneItemProc createSceneItemPlayerForSceneItemProc,
//			SceneItemPlayerAddSceneItemPlayerProc addSceneItemPlayerProc,
//			SceneItemPlayerGetPeerForSceneItemNameProc getPeerForSceneItemNameProc,
			void* userData) :
		mActionArrayPerformProc(actionArrayPerformProc),
//		mCreateSceneItemPlayerForSceneItemProc(createSceneItemPlayerForSceneItemProc),
//				mAddSceneItemPlayerProc(addSceneItemPlayerProc),
//				mGetPeerForSceneItemNameProc(getPeerForSceneItemNameProc),
				mUserData(userData)
		{}

	// Properties
	SceneItemPlayerActionArrayPerformProc	mActionArrayPerformProc;
//	SceneItemPlayerCreateSceneItemPlayerForSceneItemProc	mCreateSceneItemPlayerForSceneItemProc;
//	SceneItemPlayerAddSceneItemPlayerProc					mAddSceneItemPlayerProc;
//	SceneItemPlayerGetPeerForSceneItemNameProc				mGetPeerForSceneItemNameProc;
//	SceneItemPlayerGetCurrentViewportImageProc				mGetCurrentViewportImageProc;
	void*													mUserData;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Procs

typedef	CSceneItemPlayer*	(*SceneItemPlayerCreateProc)(const CSceneItem& sceneItem,
									const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
									const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayer

class CSceneItemPlayerInternals;
//class CSceneItemPlayer : public CEventHandler {
class CSceneItemPlayer {
	// Methods
	public:
												// Lifecycle methods
												CSceneItemPlayer(const CSceneItem& sceneItem,
														const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);
		virtual									~CSceneItemPlayer();
						
//												// CEventHandler methods
//		virtual	UError							handleEvent(CEvent& event);
						
												// Instance methods
				const	CSceneItem&				getSceneItem() const;
				
		virtual			bool					getIsVisible() const;
		virtual			void					setIsVisible(bool isVisible);

		virtual			S2DRect32				getCurrentScreenRect() const = 0;

		virtual			CActionArray			getAllActions() const;
						void					perform(const CActionArray& actionArray,
														const S2DPoint32& point = S2DPoint32());

		virtual			void					load();
		virtual			void					finishLoading();
		virtual			void					allPeersHaveLoaded();
		virtual			void					unload();
		virtual			bool					getIsFullyLoaded() const;

		virtual			UniversalTimeInterval	getStartTimeInterval() const;

		virtual			void					reset();
		virtual			void					update(UniversalTimeInterval deltaTimeInterval, bool isRunning);
		virtual			void					render(CGPU& gpu, const S2DPoint32& offset);

		virtual			bool					handlesTouchOrMouseAtPoint(const S2DPoint32& point);
		virtual			void					touchBeganOrMouseDownAtPoint(const S2DPoint32& point,
														UInt32 tapOrClickCount, const void* reference);
		virtual			void					touchOrMouseMovedFromPoint(const S2DPoint32& point1,
														const S2DPoint32& point2, const void* reference);
		virtual			void					touchEndedOrMouseUpAtPoint(const S2DPoint32& point,
														const void* reference);
		virtual			void					touchOrMouseCancelled(const void* reference);

		virtual			void					shakeBegan();
		virtual			void					shakeEnded();
		virtual			void					shakeCancelled();

		virtual			void					setProperty(const CString& property, const SDictionaryValue& value);
		virtual			bool					handleCommand(const CString& command, const CDictionary& commandInfo,
														const S2DPoint32& point);

//				CImageX					getCurrentViewportImage(bool performRedraw = false) const;

//				CSceneItemPlayer*		getSceneItemPlayerForSceneItemName(const CString& sceneItemName);

										// Class methods
// TODO: Rework all these
//		static	void					setActionArrayHandler(
//												SceneItemPlayerActionArrayHandlerProc
//														sceneItemPlayerActionArrayHandlerProc,
//												void* userData);
//
//		static	void					registerSceneItemPlayerCreateProc(const CString& itemType,
//												SceneItemPlayerCreateProc sceneItemPlayerCreateProc);
//		static	OV<CSceneItemPlayer*>	createSceneItemPlayerForSceneItem(const CSceneItem& sceneItem,
//												const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//												const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo);

	protected:
										// Subclass methods
//				const	SSceneAppResourceManagementInfo&	getSceneAppResourceManagementInfo() const;
//				bool					isTapOrMouseClick(const S2DPoint32& startPoint, const S2DPoint32& endPoint);
//				CSceneItemPlayer*		createAndAddSceneItemPlayerForSceneItem(const CSceneItem& sceneItem,
//												bool makeCopy = false, bool autoLoad = true);
//				void					addSceneItemPlayer(CSceneItemPlayer& sceneItemPlayer, bool autoLoad = true);

	// Properties
	public:
		static	CString						mIsVisiblePropertyName;		// CNumber

		static	CString						mCommandNameLoad;
		static	CString						mCommandNameUnload;
		static	CString						mCommandNameReset;

	private:
				CSceneItemPlayerInternals*	mInternals;
};


//----------------------------------------------------------------------------------------------------------------------
// MARK: - Macros for registering a Scene Item Player

//#define REGISTER_SCENE_ITEM_PLAYER(fileClass, createProc, itemType)								\
//	class	fileClass##Initializor {															\
//		public:																					\
//			fileClass##Initializor()															\
//				{ CSceneItemPlayer::registerSceneItemPlayerCreateProc(createProc, itemType); }	\
//	};																							\
//			UInt32					e##fileClass##Reference = 0;								\
//	static	fileClass##Initializor	_##fileClass##Initializor
//
//#if TARGET_OS_IPHONE
//#define REFERENCE_SCENE_ITEM_PLAYER(fileClass)													\
//	extern	UInt32	e##fileClass##Reference;													\
//	static	UInt32	s##fileClass##Reference = e##fileClass##Reference;
//#endif
//
//#if TARGET_OS_LINUX
//#define REFERENCE_SCENE_ITEM_PLAYER(fileClass)													\
//	extern	UInt32	e##fileClass##Reference;													\
//	static	UInt32	s##fileClass##Reference = e##fileClass##Reference;
//#endif
//
//#if TARGET_OS_MACOS
//#define REFERENCE_SCENE_ITEM_PLAYER(fileClass)													\
//	extern	UInt32	e##fileClass##Reference;													\
//	static	UInt32	s##fileClass##Reference = e##fileClass##Reference;
//#endif
