//----------------------------------------------------------------------------------------------------------------------
//	CAction.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAction.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CActionInternals

class CActionInternals : public TReferenceCountable<CActionInternals> {
	public:
		CActionInternals(const CString& name, const CDictionary& info, CAction::Options options) :
			TReferenceCountable(), mName(name), mInfo(info), mOptions(options) {}

		CString				mName;
		CDictionary			mInfo;
		CAction::Options	mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAction

// MARK: Properties

// Names
CString	CAction::mNameOpenURL(OSSTR("openURL"));
CString	CAction::mPauseBackgroundAudio(OSSTR("pauseBackgroundAudio"));
CString	CAction::mResumeBackgroundAudio(OSSTR("resumeBackgroundAudio"));
CString	CAction::mNameSceneCover(OSSTR("sceneCover"));
CString	CAction::mNameSceneCut(OSSTR("sceneCut"));
CString	CAction::mNameScenePush(OSSTR("scenePush"));
CString	CAction::mNameSceneUncover(OSSTR("sceneUncover"));
CString	CAction::mNameSetItemNameValue(OSSTR("setItemNameValue"));
CString	CAction::mNameSendItemCommand(OSSTR("sendItemCommand"));
CString	CAction::mNameSendAppCommand(OSSTR("sendAppCommand"));

// Info keys and value types
CString	CAction::mInfoCommandKey(OSSTR("command"));
CString	CAction::mInfoItemNameKey(OSSTR("itemName"));
CString	CAction::mInfoLoadSceneIndexFromKey(OSSTR("loadSceneIndexFrom"));
CString	CAction::mInfoPropertyNameKey(OSSTR("propertyName"));
CString	CAction::mInfoPropertyValueKey(OSSTR("propertyValue"));
CString	CAction::mInfoSceneIndexKey(OSSTR("sceneIndex"));
CString	CAction::mInfoSceneNameKey(OSSTR("sceneName"));
CString	CAction::mInfoURLKey(OSSTR("URL"));
CString	CAction::mInfoUseWebViewKey(OSSTR("useWebView"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction(const CString& name, const CDictionary& info, Options options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CActionInternals(name, info, options);
}

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	Options	options = kOptionsNone;
	if (info.contains(CString(OSSTR("dontPreload"))))
		options = (Options) (options | kOptionsDontPreload);

	mInternals =
			new CActionInternals(info.getString(CString(OSSTR("actionName"))),
					info.getDictionary(CString(OSSTR("actionInfo"))), options);
}

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction(const CAction& other)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = other.mInternals->addReference();
}

//----------------------------------------------------------------------------------------------------------------------
CAction::~CAction()
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals->removeReference();
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CDictionary CAction::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	CDictionary	info;

	info.set(CString(OSSTR("actionName")), mInternals->mName);
	info.set(CString(OSSTR("actionInfo")), mInternals->mInfo);
	if (mInternals->mOptions & kOptionsDontPreload)
		info.set(CString(OSSTR("dontPreload")), (UInt32) 1);

	return info;
}

//----------------------------------------------------------------------------------------------------------------------
const CString& CAction::getName() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mName;
}

////----------------------------------------------------------------------------------------------------------------------
//void CAction::setName(const CString& actionName)
////----------------------------------------------------------------------------------------------------------------------
//{
//	mInternals->mName = actionName;
//}
//
//----------------------------------------------------------------------------------------------------------------------
const CDictionary& CAction::getActionInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mInfo;
}

////----------------------------------------------------------------------------------------------------------------------
//void CAction::setActionInfo(const CDictionary& actionInfo)
////----------------------------------------------------------------------------------------------------------------------
//{
//	mInternals->mInfo = actionInfo;
//}
//
//----------------------------------------------------------------------------------------------------------------------
CAction::Options CAction::getOptions() const
//----------------------------------------------------------------------------------------------------------------------
{
	return mInternals->mOptions;
}

////----------------------------------------------------------------------------------------------------------------------
//void CAction::setOptions(EActionOptions options)
////----------------------------------------------------------------------------------------------------------------------
//{
//	mInternals->mOptions = options;
//}
//
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CActions

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CActions::CActions() : TNArray<CAction>()
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
CActions::CActions(const CAction& action) : TNArray<CAction>(action)
//----------------------------------------------------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------------------------------------------------
CActions::CActions(const CDictionary& info) : TNArray<CAction>()
//----------------------------------------------------------------------------------------------------------------------
{
	// Iterate action infos
	TArray<CDictionary>	actionInfos = info.getArrayOfDictionaries(CString(OSSTR("actions")));
	for (CArray::ItemIndex i = 0; i < actionInfos.getCount(); i++)
		// Add action
		*this += CAction(actionInfos[i]);
}

//----------------------------------------------------------------------------------------------------------------------
CActions::CActions(const CActions& other) : TNArray<CAction>(other)
//----------------------------------------------------------------------------------------------------------------------
{
}

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
CDictionary CActions::getInfo() const
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	CDictionary	info;

	// Collect action infos
	TNArray<CDictionary>	actionInfos;
	for (TIteratorD<CAction> iterator = getIterator(); iterator.hasValue(); iterator.advance())
		actionInfos += iterator.getValue().getInfo();
	info.set(CString(OSSTR("actions")), actionInfos);

	return info;
}
