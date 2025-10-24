//----------------------------------------------------------------------------------------------------------------------
//	CSceneItemPlayer.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CSceneItemPlayer.h"

#include "CAudioSession.h"
#include "CLogServices.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItemPlayer::Internals

class CSceneItemPlayer::Internals {
	public:
						Internals(CSceneItemPlayer& sceneItemPlayer, CSceneItem& sceneItem,
								const Procs& procs) :
							mSceneItemPlayer(sceneItemPlayer), mProcs(procs),
									mSceneItem(sceneItem), mName(sceneItem.getName()),
									mSceneItemOptions(sceneItem.getOptions()),
									mIsInitiallyVisible(sceneItem.getIsInitiallyVisible()),
									mIsVisible(sceneItem.getIsInitiallyVisible()),
									mIsLoaded(false)
							{
								// Set proc
								mSceneItem->setNoteUpdatedProc((CSceneItem::NoteUpdatedProc) noteSceneItemUpdated, this);
							}
						Internals(CSceneItemPlayer& sceneItemPlayer, const CString& name,
								CSceneItem::Options sceneItemOptions, bool isInitiallyVisible, const Procs& procs) :
							mSceneItemPlayer(sceneItemPlayer), mProcs(procs),
									mName(name), mSceneItemOptions(sceneItemOptions),
									mIsInitiallyVisible(isInitiallyVisible),
									mIsVisible(isInitiallyVisible),
									mIsLoaded(false)
							{}
						~Internals()
							{
								// Check if have CSceneItem
								if (mSceneItem.hasReference())
									// Clear proc
									mSceneItem->setNoteUpdatedProc(nil, nil);
							}

		static	void	noteSceneItemUpdated(const CString& propertyName, Internals* internals)
							{ internals->mSceneItemPlayer.noteSceneItemUpdated(propertyName); }

		CSceneItemPlayer&	mSceneItemPlayer;
		Procs				mProcs;

		OR<CSceneItem>		mSceneItem;
		CString				mName;
		CSceneItem::Options	mSceneItemOptions;
		bool				mIsInitiallyVisible;

		bool				mIsVisible;
		S2DOffsetF32		mScreenPositionOffset;

		bool				mIsLoaded;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItemPlayer

// MARK: Properties

CString	CSceneItemPlayer::mIsVisiblePropertyName(OSSTR("visible"));

CString	CSceneItemPlayer::mCommandNameLoad(OSSTR("load"));
CString	CSceneItemPlayer::mCommandNameUnload(OSSTR("unload"));
CString	CSceneItemPlayer::mCommandNameReset(OSSTR("reset"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer::CSceneItemPlayer(CSceneItem& sceneItem, const Procs& procs)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(*this, sceneItem, procs);

	// Reset
	reset();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer::CSceneItemPlayer(const CString& name, CSceneItem::Options sceneItemOptions, bool isInitiallyVisible,
		const Procs& procs)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(*this, name, sceneItemOptions, isInitiallyVisible, procs);

	// Reset
	reset();
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItemPlayer::CSceneItemPlayer(const Procs& procs)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	mInternals = new Internals(*this, CString::mEmpty, CSceneItem::kOptionsNone, true, procs);

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
const CString& CSceneItemPlayer::getName() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName;
}

//----------------------------------------------------------------------------------------------------------------------
CSceneItem::Options CSceneItemPlayer::getSceneItemOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mSceneItemOptions;
}

//----------------------------------------------------------------------------------------------------------------------
const OR<CSceneItem>& CSceneItemPlayer::getSceneItem() const
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
const S2DOffsetF32&CSceneItemPlayer::getScreenPositionOffset() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mScreenPositionOffset;
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::setScreenPositionOffset(const S2DOffsetF32& screenPositionOffset)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mScreenPositionOffset = screenPositionOffset;
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
	mInternals->mProcs.performActions(actions, point);
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
	mInternals->mIsVisible = mInternals->mIsInitiallyVisible;
	if (mInternals->mSceneItemOptions & CSceneItem::kOptionsHideIfNoAudioInput)
		mInternals->mIsVisible &= CAudioSession::mShared.inputIsAvailable();
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::update(CGPU& gpu, UniversalTimeInterval deltaTimeInterval, bool isRunning)
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
void CSceneItemPlayer::setProperty(const CString& property, const SValue& value)
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

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::setPeerProperty(const CString& sceneName, const CString& name, const CString& property,
		const SValue& value) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CDictionary	info;
	info.set(CAction::mInfoSceneNameKey, sceneName);
	info.set(CAction::mInfoItemNameKey, name);
	info.set(CAction::mInfoPropertyNameKey, property);
	info.set(CAction::mInfoPropertyValueKey, value);

	// Call proc
	mInternals->mProcs.performActions(CActions(CAction(CAction::mNameSetItemNameValue, info)));
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::setPeerProperty(const CString& name, const CString& property, const SValue& value) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CDictionary	info;
	info.set(CAction::mInfoItemNameKey, name);
	info.set(CAction::mInfoPropertyNameKey, property);
	info.set(CAction::mInfoPropertyValueKey, value);

	// Call proc
	mInternals->mProcs.performActions(CActions(CAction(CAction::mNameSetItemNameValue, info)));
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::sendPeerCommand(const CString& name, const CString& command) const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CDictionary	info;
	info.set(CAction::mInfoItemNameKey, name);
	info.set(CAction::mInfoCommandKey, command);

	// Call proc
	mInternals->mProcs.performActions(CActions(CAction(CAction::mNameSendItemCommand, info)));
}

//----------------------------------------------------------------------------------------------------------------------
I<CSceneItemPlayer> CSceneItemPlayer::add(CSceneItem& sceneItem, CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mProcs.addSceneItem(sceneItem, gpu);
}

//----------------------------------------------------------------------------------------------------------------------
I<CSceneItemPlayer> CSceneItemPlayer::add(const I<CSceneItemPlayer>& sceneItemPlayer, CGPU& gpu)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mProcs.addSceneItemPlayer(sceneItemPlayer, gpu);
}

//----------------------------------------------------------------------------------------------------------------------
OR<I<CSceneItemPlayer> > CSceneItemPlayer::getSceneItemPlayer(const CSceneItem& sceneItem)
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mProcs.getSceneItemPlayer(sceneItem);
}

//----------------------------------------------------------------------------------------------------------------------
void CSceneItemPlayer::remove(const I<CSceneItemPlayer>& sceneItemPlayer)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->mProcs.removeSceneItemPlayer(sceneItemPlayer);
}
