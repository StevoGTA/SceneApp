//----------------------------------------------------------------------------------------------------------------------
//	CAction.cpp			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "CAction.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local data

static	CString	sNameKey(OSSTR("actionName"));
static	CString	sInfoKey(OSSTR("actionInfo"));
static	CString	sDontPreloadKey(OSSTR("dontPreload"));		// Can be anything
static	CString	sUnloadCurrentKey(OSSTR("unloadCurrent"));	// Can be anything

static	CString	sActionsKey(OSSTR("actions"));

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CActionInternals

class CActionInternals : public TReferenceCountable<CActionInternals> {
	public:
		CActionInternals(const CString& name, const CDictionary& info, EActionOptions options) :
			TReferenceCountable(), mName(name), mInfo(info), mOptions(kActionOptionsNone) {}

		CString			mName;
		CDictionary		mInfo;
		EActionOptions	mOptions;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAction

// MARK: Properties

// Names
CString	CAction::mNameOpenURL(OSSTR("openURL"));
CString	CAction::mNamePlayMovie(OSSTR("playMovie"));
CString	CAction::mNameSceneCover(OSSTR("sceneCover"));
CString	CAction::mNameSceneCut(OSSTR("sceneCut"));
CString	CAction::mNameScenePush(OSSTR("scenePush"));
CString	CAction::mNameSceneUncover(OSSTR("sceneUncover"));
CString	CAction::mNameSetItemNameValue(OSSTR("setItemNameValue"));
CString	CAction::mNameSendItemCommand(OSSTR("sendItemCommand"));
CString	CAction::mNameSendAppCommand(OSSTR("sendAppCommand"));

// Info keys and value types
CString	CAction::mInfoCommandKey(OSSTR("command"));
CString	CAction::mInfoControlModeKey(OSSTR("controlMode"));
CString	CAction::mInfoFilenameKey(OSSTR("filename"));
CString	CAction::mInfoItemNameKey(OSSTR("itemName"));
CString	CAction::mInfoLoadSceneIndexFromKey(OSSTR("loadSceneIndexFrom"));
CString	CAction::mInfoPropertyNameKey(OSSTR("propertyName"));
CString	CAction::mInfoPropertyValueKey(OSSTR("propertyValue"));
CString	CAction::mInfoSceneIndexKey(OSSTR("sceneIndex"));
CString	CAction::mInfoSceneNameKey(OSSTR("sceneName"));
CString	CAction::mInfoURLKey(OSSTR("URL"));
CString	CAction::mInfoUseWebView(OSSTR("useWebView"));

// MARK: Lifecycle methods

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction(const CString& name, const CDictionary& info, EActionOptions options)
//----------------------------------------------------------------------------------------------------------------------
{
	mInternals = new CActionInternals(name, info, options);
}

//----------------------------------------------------------------------------------------------------------------------
CAction::CAction(const CDictionary& info)
//----------------------------------------------------------------------------------------------------------------------
{
	EActionOptions	options = kActionOptionsNone;
	if (info.contains(sDontPreloadKey))
		options = (EActionOptions) (options | kActionOptionsDontPreload);

	mInternals = new CActionInternals(info.getString(sNameKey), info.getDictionary(sInfoKey), options);
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

	info.set(sNameKey, mInternals->mName);
	info.set(sInfoKey, mInternals->mInfo);
	if (mInternals->mOptions & kActionOptionsDontPreload)
		info.set(sDontPreloadKey, (UInt32) 1);
//	if (mInternals->mOptions & kActionOptionsUnloadCurrent)
//		info.set(sUnloadCurrentKey, (UInt32) 1);

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
EActionOptions CAction::getOptions() const
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
CActions::CActions(const CAction& action) : TNArray<CAction>()
//----------------------------------------------------------------------------------------------------------------------
{
	*this += action;
}

//----------------------------------------------------------------------------------------------------------------------
CActions::CActions(const CDictionary& info) : TNArray<CAction>()
//----------------------------------------------------------------------------------------------------------------------
{
	TArray<CDictionary>	actionInfos = info.getArrayOfDictionaries(sActionsKey);
	for (CArray::ItemIndex i = 0; i < actionInfos.getCount(); i++)
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
	info.set(sActionsKey, actionInfos);

	return info;
}
