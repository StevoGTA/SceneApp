//----------------------------------------------------------------------------------------------------------------------
//	CAction.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDictionary.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Action Options

enum EActionOptions {
	kActionOptionsNone			= 0,
	kActionOptionsDontPreload	= 1 << 0,
//	kActionOptionsUnloadCurrent	= 1 << 1,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAction

class CActionInternals;
class CAction {
	// Methods
	public:
								// Lifecycle methods
								CAction(const CString& name, const CDictionary& info,
										EActionOptions options = kActionOptionsNone);
								CAction(const CDictionary& info);
								CAction(const CAction& other);
								~CAction();

								// Instance methods
				CDictionary		getInfo() const;

		const	CString&		getName() const;
//				void			setName(const CString& name);

		const	CDictionary&	getActionInfo() const;
//				void			setActionInfo(const CDictionary& info);

				EActionOptions	getOptions() const;
//				void			setOptions(EActionOptions options);

	// Properties
	public:
									// Names
		static	CString				mNameOpenURL;
		static	CString				mNamePlayMovie;
		static	CString				mNameSceneCover;
		static	CString				mNameSceneCut;
		static	CString				mNameScenePush;
		static	CString				mNameSceneUncover;
		static	CString				mNameSetItemNameValue;
		static	CString				mNameSendItemCommand;
		static	CString				mNameSendAppCommand;

									// Info keys and value types
		static	CString				mInfoCommandKey;			// CString
		static	CString				mInfoControlModeKey;		// UInt32
		static	CString				mInfoFilenameKey;			// CString
		static	CString				mInfoItemNameKey;			// CString
		static	CString				mInfoLoadSceneIndexFromKey;	// CString
		static	CString				mInfoPropertyNameKey;		// CString
		static	CString				mInfoPropertyValueKey;		// SDictionaryValue
		static	CString				mInfoSceneIndexKey;			// UInt32
		static	CString				mInfoSceneNameKey;			// CString
		static	CString				mInfoURLKey;				// CString
		static	CString				mInfoUseWebView;			// anything

	private:
				CActionInternals*	mInternals;
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CActions

class CActions : public TNArray<CAction> {
	// Methods
	public:
					// Lifecycle methods
					CActions();
					CActions(const CAction& action);
					CActions(const CDictionary& info);
					CActions(const CActions& other);

					// Instance methods
		CDictionary	getInfo() const;
};
