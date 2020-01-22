//----------------------------------------------------------------------------------------------------------------------
//	CSceneItem.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDictionary.h"
#include "TimeAndDate.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Start time values

const	UniversalTimeInterval	kSceneItemStartTimeStartAtLoad = -1.0;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Options

enum ESceneItemOptions {
	kSceneItemOptionsNone				= 0,
	
	// Anchor to Screen (instead of default anchor to background)
	kSceneItemOptionsAnchorToScreen		= 1 << 0,
	
	// Don't load with scene (instead of default to load when scene loads)
	kSceneItemOptionsDontLoadWithScene	= 1 << 1,
	
	// Hide if audio input is unavailabale
	kSceneItemOptionsHideIfNoAudioInput	= 1 << 2,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Property info

// Query the properties method which will return a TArray<CDictionary>.  In each CDictionary are the following:
//		Key: mPropertyNameKey		Value: CString of object property name
//		Key: mPropertyTitleKey		Value: CString of display title
//		Key: mPropertyTypeKey		Value: OSType of property type (see below)
enum {
	kSceneItemPropertyTypeName				= MAKE_OSTYPE('N', 'a', 'm', 'e'),
	kSceneItemPropertyTypeFilename			= MAKE_OSTYPE('F', 'N', 'a', 'm'),
	kSceneItemPropertyTypeText				= MAKE_OSTYPE('T', 'e', 'x', 't'),
	kSceneItemPropertyTypeBoolean			= MAKE_OSTYPE('B', 'o', 'o', 'l'),
	kSceneItemPropertyTypeOptions			= MAKE_OSTYPE('O', 'p', 't', 's'),
	kSceneItemPropertyTypeScreenRect		= MAKE_OSTYPE('R', 'e', 'c', 't'),
	kSceneItemPropertyTypeScreenRectArray	= MAKE_OSTYPE('A', 'R', 'c', 't'),
	kSceneItemPropertyTypeScreenPoint		= MAKE_OSTYPE('S', 'c', 'P', 't'),
	kSceneItemPropertyTypeActionArray		= MAKE_OSTYPE('A', 'c', 't', 'n'),
	kSceneItemPropertyTypeStartTimeInterval	= MAKE_OSTYPE('S', 't', 'T', 'm'),
	kSceneItemPropertyTypeColor				= MAKE_OSTYPE('C', 'o', 'l', 'r'),
	kSceneItemPropertyTypePixelWidth		= MAKE_OSTYPE('P', 'x', 'l', 'W'),
	kSceneItemPropertyTypeFontName			= MAKE_OSTYPE('F', 'n', 't', 'N'),
	kSceneItemPropertyTypeFontSize			= MAKE_OSTYPE('F', 'n', 't', 'S'),
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CSceneItem

class CSceneItemInternals;
class CSceneItem {
	// Methods
	public:
											// Lifecycle methods
											CSceneItem();
											CSceneItem(const CDictionary& info);
											CSceneItem(const CSceneItem& other);
		virtual								~CSceneItem();
						
											// Instance methods
		virtual			CSceneItem*			copy() const = 0;

						bool				getIsVisible() const;
						void				setIsVisible(bool isVisible);

						CString&			getName() const;
						void				setName(const CString& name);

						ESceneItemOptions	getOptions() const;
						void				setOptions(ESceneItemOptions options);

		virtual	const	CString&			getType() const = 0;
		virtual			CString				getDescription();
		virtual			TArray<CDictionary>	getProperties() const;
		virtual			CDictionary			getInfo() const;

	// Properties
	public:
		static	CString					mItemInfoKey;

		static	CString					mPropertyNameKey;
		static	CString					mPropertyTitleKey;
		static	CString					mPropertyTypeKey;

	private:
				CSceneItemInternals*	mInternals;
};
