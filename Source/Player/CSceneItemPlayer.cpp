//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayer.h"

#include "CAudioSession.h"
#include "CLogServices.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

//const	Float32	kCSceneItemPlayerTapOrClickMaxPixelDelta = 5.0;

CString	CSceneItemPlayer::mIsVisiblePropertyName(OSSTR("visible"));

CString	CSceneItemPlayer::mCommandNameLoad(OSSTR("load"));
CString	CSceneItemPlayer::mCommandNameUnload(OSSTR("unload"));
CString	CSceneItemPlayer::mCommandNameReset(OSSTR("reset"));

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

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayer

// MARK: Lifecycle methods

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
	Delete(mInternals);
}

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
CActions CSceneItemPlayer::getAllActions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return CActions();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::perform(const CActions& actions, const S2DPointF32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Call proc
	mInternals->mSceneItemPlayerProcsInfo.performActions(actions, point);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::load(CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mIsLoaded = true;
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
const OV<UniversalTimeInterval>& CSceneItemPlayer::getStartTimeInterval() const
//----------------------------------------------------------------------------------------------------------------------
{
	static	OV<UniversalTimeInterval>	sStartTimeIntervalUndefined;

	return sStartTimeIntervalUndefined;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::reset()
//----------------------------------------------------------------------------------------------------------------------
{
	// Update visible
	mInternals->mIsVisible = mInternals->mSceneItem.getIsVisible();
	if (mInternals->mSceneItem.getOptions() & kSceneItemOptionsHideIfNoAudioInput)
		mInternals->mIsVisible &= CAudioSession::shared.inputIsAvailable();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::update(UniversalTimeInterval deltaTimeInterval, bool isRunning)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
bool CSceneItemPlayer::handlesTouchOrMouseAtPoint(const S2DPointF32& point) const
//----------------------------------------------------------------------------------------------------------------------
{
	return false;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::touchBeganOrMouseDownAtPoint(const S2DPointF32& point, UInt32 tapOrClickCount,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::touchOrMouseMovedFromPoint(const S2DPointF32& point1, const S2DPointF32& point2,
		const void* reference)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::touchEndedOrMouseUpAtPoint(const S2DPointF32& point, const void* reference)
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
bool CSceneItemPlayer::handleCommand(CGPU& gpu, const CString& command, const CDictionary& commandInfo,
		const S2DPointF32& point)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check command
	if (command == mCommandNameLoad) {
		// Load
		load(gpu);

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
//bool CSceneItemPlayer::isTapOrMouseClick(const S2DPointF32& startPoint, const S2DPointF32& endPoint)
////----------------------------------------------------------------------------------------------------------------------
//{
//	return (fabs(endPoint.mX - startPoint.mX) <= kCSceneItemPlayerTapOrClickMaxPixelDelta) &&
//			(fabs(endPoint.mY - startPoint.mY) <= kCSceneItemPlayerTapOrClickMaxPixelDelta);
//}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::setPeerProperty(const CString& sceneName, const CString& name, const CString& property,
		const SDictionaryValue& value) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CDictionary	info;
	info.set(CAction::mInfoSceneNameKey, sceneName);
	info.set(CAction::mInfoItemNameKey, name);
	info.set(CAction::mInfoPropertyNameKey, property);
	info.set(CAction::mInfoPropertyValueKey, value);

	// Call proc
	mInternals->mSceneItemPlayerProcsInfo.performActions(CActions(CAction(CAction::mNameSetItemNameValue, info)));
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::setPeerProperty(const CString& name, const CString& property, const SDictionaryValue& value)
		const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CDictionary	info;
	info.set(CAction::mInfoItemNameKey, name);
	info.set(CAction::mInfoPropertyNameKey, property);
	info.set(CAction::mInfoPropertyValueKey, value);

	// Call proc
	mInternals->mSceneItemPlayerProcsInfo.performActions(CActions(CAction(CAction::mNameSetItemNameValue, info)));
}
