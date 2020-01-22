//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayer.h"

#include "CAudioSession.h"
#include "CLogServices.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

//const	Float32	kCSceneItemPlayerTapOrClickMaxPixelDelta = 5.0;

//static	SceneItemPlayerActionArrayHandlerProc	sSceneItemPlayerActionArrayHandlerProc = nil;
//static	void*									sSceneItemPlayerActionArrayHandlerUserData = nil;

CString	CSceneItemPlayer::mIsVisiblePropertyName("visible");

CString	CSceneItemPlayer::mCommandNameLoad("load");
CString	CSceneItemPlayer::mCommandNameUnload("unload");
CString	CSceneItemPlayer::mCommandNameReset("reset");

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayerInternals

class CSceneItemPlayerInternals {
	public:
		CSceneItemPlayerInternals(const CSceneItem& sceneItem,
				const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo) :
			mIsVisible(false), mIsLoaded(false), mSceneItem(sceneItem),
					mSceneItemPlayerProcsInfo(sceneItemPlayerProcsInfo)
			{}

			bool						mIsVisible;
			bool						mIsLoaded;
	const	CSceneItem&					mSceneItem;
			SSceneItemPlayerProcsInfo	mSceneItemPlayerProcsInfo;
};

////----------------------------------------------------------------------------------------------------------------------
////----------------------------------------------------------------------------------------------------------------------
//// MARK: - CSceneItemPlayerCreateInfo
//
//class CSceneItemPlayerCreateInfo {
//	public:
//		CSceneItemPlayerCreateInfo(const CString& itemType, SceneItemPlayerCreateProc createProc) :
//			mItemType(itemType), mCreateProc(createProc)
//			{}
//		~CSceneItemPlayerCreateInfo() {}
//
//		const	CString&					mItemType;
//				SceneItemPlayerCreateProc	mCreateProc;
//};

//static	TPtrArray<CSceneItemPlayerCreateInfo*>*	sSceneItemPlayerInfosArray = nil;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayer

// MARK: Lifecycle methods

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer::CSceneItemPlayer()
////----------------------------------------------------------------------------------------------------------------------
//{
//	mInternals = new CSceneItemPlayerInternals(nil, false);
//}
//
//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer::CSceneItemPlayer(const CSceneItem& sceneItem,
		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new CSceneItemPlayerInternals(sceneItem, sceneItemPlayerProcsInfo);

	// Reset
	reset();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer::~CSceneItemPlayer()
//----------------------------------------------------------------------------------------------------------------------
{
	DisposeOf(mInternals);
}

// MARK: CEventHandler methods

////----------------------------------------------------------------------------------------------------------------------
//UError CSceneItemPlayer::handleEvent(CEvent& event)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return kEventEventNotHandled;
//}
//
// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
const CSceneItem& CSceneItemPlayer::getSceneItem() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneItem;
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayer::getIsVisible() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mIsVisible;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::setIsVisible(bool isVisible)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsVisible = isVisible;
}

//----------------------------------------------------------------------------------------------------------------------
CActionArray CSceneItemPlayer::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return CActionArray();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::perform(const CActionArray& actionArray, const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Call proc
	mInternals->mSceneItemPlayerProcsInfo.mActionArrayPerformProc(actionArray, point,
			mInternals->mSceneItemPlayerProcsInfo.mUserData);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::load()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsLoaded = true;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::finishLoading()
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::allPeersHaveLoaded()
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::unload()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsLoaded = false;
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayer::getIsFullyLoaded() const
//----------------------------------------------------------------------------------------------------------------------
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
UniversalTimeInterval CSceneItemPlayer::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	return 0.0;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	// Update visible
	mInternals->mIsVisible = mInternals->mSceneItem.getIsVisible();
	if (mInternals->mSceneItem.getOptions() & kSceneItemOptionsHideIfNoAudioInput)
		mInternals->mIsVisible &= CAudioSession::audioInputIsAvailable();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::render(CGPU& gpu, const S2DPoint32& offset)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayer::handlesTouchOrMouseAtPoint(const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::touchBeganOrMouseDownAtPoint(const S2DPoint32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::touchOrMouseMovedFromPoint(const S2DPoint32& point1, const S2DPoint32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::touchEndedOrMouseUpAtPoint(const S2DPoint32& point, const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::touchOrMouseCancelled(const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::shakeBegan()
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::shakeEnded()
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::shakeCancelled()
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::setProperty(const CString& property, const SDictionaryValue& value)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check property name
	if (property == mIsVisiblePropertyName)
		// IsVisible
		setIsVisible(value.getBool());
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayer::handleCommand(const CString& command, const CDictionary& commandInfo, const S2DPoint32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check command
	if (command == mCommandNameLoad) {
		// Load
		load();

		return true;
	} else if (command == mCommandNameUnload) {
		// Unload
		unload();

		return true;
	} else if (command == mCommandNameReset) {
		// Reset
		reset();

		return true;
	} else
		// Unknown
		return false;
}

// MARK: Subclass methods

////----------------------------------------------------------------------------------------------------------------------
//CImageX CSceneItemPlayer::getCurrentViewportImage(bool performRedraw) const
////----------------------------------------------------------------------------------------------------------------------
//{
//	return mInternals->mSceneItemPlayerProcsInfo->mGetCurrentViewportImageProc(*this, performRedraw,
//			mInternals->mSceneItemPlayerProcsInfo->mUserData);
//}

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* CSceneItemPlayer::getSceneItemPlayerForSceneItemName(const CString& sceneItemName)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return mInternals->mSceneItemPlayerProcsInfo.mGetPeerForSceneItemNameProc(*this, sceneItemName,
//			mInternals->mSceneItemPlayerProcsInfo.mUserData);
//}

// MARK: Class methods

////----------------------------------------------------------------------------------------------------------------------
//void CSceneItemPlayer::setActionArrayHandler(
//		SceneItemPlayerActionArrayHandlerProc sceneItemPlayerActionArrayHandlerProc, void* userData)
////----------------------------------------------------------------------------------------------------------------------
//{
//	sSceneItemPlayerActionArrayHandlerProc = sceneItemPlayerActionArrayHandlerProc;
//	sSceneItemPlayerActionArrayHandlerUserData = userData;
//}

////----------------------------------------------------------------------------------------------------------------------
//void CSceneItemPlayer::registerSceneItemPlayerCreateProc(SceneItemPlayerCreateProc sceneItemPlayerCreateProc,
//		const CString& itemType)
////----------------------------------------------------------------------------------------------------------------------
//{
//	if (sSceneItemPlayerInfosArray == nil)
//		sSceneItemPlayerInfosArray = new TPtrArray<CSceneItemPlayerCreateInfo*>();
//
//	(*sSceneItemPlayerInfosArray) += new CSceneItemPlayerCreateInfo(itemType, sceneItemPlayerCreateProc);
//}

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* CSceneItemPlayer::createSceneItemPlayerForSceneItem(const CSceneItem& sceneItem,
//		const SSceneAppResourceManagementInfo& sceneAppResourceManagementInfo,
//		const SSceneItemPlayerProcsInfo& sceneItemPlayerProcsInfo, bool makeCopy)
////----------------------------------------------------------------------------------------------------------------------
//{
//	for (CArrayItemIndex i = 0; i < sSceneItemPlayerInfosArray->getCount(); i++) {
//		CSceneItemPlayerCreateInfo*	info = (*sSceneItemPlayerInfosArray)[i];
//		if (info->mItemType == sceneItem.getType())
//			return info->mCreateProc(sceneItem, sceneAppResourceManagementInfo, sceneItemPlayerProcsInfo, makeCopy);
//	}
//
//#if defined(DEBUG)
//	CLogServices::logMessage(CString("CSceneItemPlayer unable to create player for itemType ") + sceneItem.getType());
//#endif
//
//	return nil;
//}

// MARK: Subclass methods

////----------------------------------------------------------------------------------------------------------------------
//const SSceneAppResourceManagementInfo& CSceneItemPlayer::getSceneAppResourceManagementInfo() const
////----------------------------------------------------------------------------------------------------------------------
//{
//	return mInternals->mSceneAppResourceManagementInfo;
//}

////----------------------------------------------------------------------------------------------------------------------
//bool CSceneItemPlayer::isTapOrMouseClick(const S2DPoint32& startPoint, const S2DPoint32& endPoint)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return (fabs(endPoint.mX - startPoint.mX) <= kCSceneItemPlayerTapOrClickMaxPixelDelta) &&
//			(fabs(endPoint.mY - startPoint.mY) <= kCSceneItemPlayerTapOrClickMaxPixelDelta);
//}

////----------------------------------------------------------------------------------------------------------------------
//CSceneItemPlayer* CSceneItemPlayer::createAndAddSceneItemPlayerForSceneItem(
//		const CSceneItem& sceneItem, bool makeCopy, bool autoLoad)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return mInternals->mSceneItemPlayerProcsInfo.mCreateSceneItemPlayerForSceneItemProc(sceneItem, makeCopy, autoLoad,
//			mInternals->mSceneItemPlayerProcsInfo.mUserData);
//}

////----------------------------------------------------------------------------------------------------------------------
//void CSceneItemPlayer::addSceneItemPlayer(CSceneItemPlayer& sceneItemPlayer, bool autoLoad)
////----------------------------------------------------------------------------------------------------------------------
//{
//	mInternals->mSceneItemPlayerProcsInfo.mAddSceneItemPlayerProc(sceneItemPlayer, autoLoad,
//			mInternals->mSceneItemPlayerProcsInfo.mUserData);
//}
