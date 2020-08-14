//----------------------------------------------------------------------------------------------------------------------
//	CAudioInfo.h			©2012 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#include "CDictionary.h"

//----------------------------------------------------------------------------------------------------------------------
// MARK: Looping

// TODO: Look into making loop count optional
const	UInt32	kAudioInfoLoopCountForever = 0;

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Options

enum EAudioInfoOptions {
	kAudioInfoOptionsNone					= 0,
	kAudioInfoOptionsStopBeforePlay			= 1 << 0,
	kAudioInfoOptionsUseDefinedLoopInFile	= 1 << 1,
	kAudioInfoOptionsLoadFileIntoMemory		= 1 << 2,
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - Property info

enum {
	kSceneItemPropertyTypeAudioInfo	= MAKE_OSTYPE('A', 'u', 'I', 'n'),

	kAudioInfoPropertyTypeGain		= MAKE_OSTYPE('A', 'u', 'I', 'G'),
	kAudioInfoPropertyTypeLoopCount	= MAKE_OSTYPE('A', 'u', 'I', 'L'),
	kAudioInfoPropertyTypeOptions	= MAKE_OSTYPE('A', 'u', 'I', 'O'),
};

//----------------------------------------------------------------------------------------------------------------------
// MARK: - CAudioInfo

class CAudioInfoInternals;
class CAudioInfo {
	// Methods
	public:
									// Lifecycle methods
									CAudioInfo();
									CAudioInfo(const CDictionary& info);
									CAudioInfo(const CAudioInfo& other);
									~CAudioInfo();

									// Instance methods
				TArray<CDictionary>	getProperties() const;
				CDictionary			getInfo() const;
				
				Float32				getGain() const;
				void				setGain(Float32 gain);

		const	CString&			getResourceFilename() const;
				void				setResourceFilename(const CString& resourceFilename);

				UInt32				getLoopCount() const;
				void				setLoopCount(UInt32 loopCount);

				EAudioInfoOptions	getOptions() const;
				void				setOptions(EAudioInfoOptions options);

	// Properties
	private:
		CAudioInfoInternals*	mInternals;
};
