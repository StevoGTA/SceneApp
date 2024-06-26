//----------------------------------------------------------------------------------------------------------------------
//	CSceneItem.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDictionary.h"
#include "TimeAndDate.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: CSceneItem

class CSceneItem {
	// Options
	public:
		enum Options {
			kOptionsNone				= 0,

			// Anchor to Screen (instead of default anchor to background)
			kOptionsAnchorToScreen		= 1 << 0,

			// Don't load with scene (instead of default to load when scene loads)
			kOptionsDontLoadWithScene	= 1 << 1,

			// Hide if audio input is unavailabale
			kOptionsHideIfNoAudioInput	= 1 << 2,
		};

	// PropertyType
	public:
		// Query the properties method which will return a TArray<CDictionary>.  In each CDictionary are the following:
		//		Key: mPropertyNameKey		Value: CString of object property name
		//		Key: mPropertyTitleKey		Value: CString of display title
		//		Key: mPropertyTypeKey		Value: OSType of property type (see below)
		enum PropertyType {
			kPropertyTypeName				= MAKE_OSTYPE('N', 'a', 'm', 'e'),
			kPropertyTypeFilename			= MAKE_OSTYPE('F', 'N', 'a', 'm'),
			kPropertyTypeText				= MAKE_OSTYPE('T', 'e', 'x', 't'),
			kPropertyTypeBoolean			= MAKE_OSTYPE('B', 'o', 'o', 'l'),
			kPropertyTypeOptions			= MAKE_OSTYPE('O', 'p', 't', 's'),
			kPropertyTypeScreenRect			= MAKE_OSTYPE('R', 'e', 'c', 't'),
			kPropertyTypeScreenRectArray	= MAKE_OSTYPE('A', 'R', 'c', 't'),
			kPropertyTypeScreenPoint		= MAKE_OSTYPE('S', 'c', 'P', 't'),
			kPropertyTypeActions			= MAKE_OSTYPE('A', 'c', 't', 'n'),
			kPropertyTypeStartTimeInterval	= MAKE_OSTYPE('S', 't', 'T', 'm'),
			kPropertyTypeColor				= MAKE_OSTYPE('C', 'o', 'l', 'r'),
			kPropertyTypePixelWidth			= MAKE_OSTYPE('P', 'x', 'l', 'W'),
			kPropertyTypeFontName			= MAKE_OSTYPE('F', 'n', 't', 'N'),
			kPropertyTypeFontSize			= MAKE_OSTYPE('F', 'n', 't', 'S'),
		};

	// Classes
	private:
		class Internals;

	// Methods
	public:
												// Lifecycle methods
												CSceneItem();
												CSceneItem(const CDictionary& info);
												CSceneItem(const CSceneItem& other);
		virtual									~CSceneItem();
						
												// Instance methods
		virtual			CSceneItem*				copy() const = 0;

						bool					getIsVisible() const;
						void					setIsVisible(bool isVisible);

				const	CString&				getName() const;
						void					setName(const CString& name);

						Options					getOptions() const;
						void					setOptions(Options options);

		virtual	const	CString&				getType() const = 0;
		virtual			CString					getDescription();
		virtual			TMArray<CDictionary>	getProperties() const;
		virtual			CDictionary				getInfo() const;

	// Properties
	public:
		static	CString		mItemInfoKey;

		static	CString		mPropertyNameKey;
		static	CString		mPropertyTitleKey;
		static	CString		mPropertyTypeKey;

	private:
				Internals*	mInternals;
};
