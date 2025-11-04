//----------------------------------------------------------------------------------------------------------------------
//	SceneAppViewController.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#import "AKTGPUView.h"
#import "CSceneAppPlayer.h"

NS_ASSUME_NONNULL_BEGIN

//----------------------------------------------------------------------------------------------------------------------
// MARK: SceneAppViewController

@interface SceneAppViewController : NSViewController

// MARK: Types
typedef CSceneAppPlayer*	_Nonnull	(^CreateSceneAppPlayerProc)(CGPU& gpu, const CSceneAppPlayer::Procs& procs,
												const SSceneAppResourceLoading& sceneAppResourceLoading);

// MARK: Properties
@property (nonatomic, readonly)			CSceneAppPlayer&	sceneAppPlayer;

@property (nonatomic, strong, nullable)	void				(^openURLProc)(NSURL* url);
@property (nonatomic, strong, nullable)	void				(^handleCommandProc)(const CString& command,
																	const CDictionary& commandInfo);

// MARK: Instance methods
- (instancetype) initWithView:(NSView<AKTGPUView>*) view;

- (void) loadScenesFrom:(const CScenePackage::Info&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder;
- (void) loadScenesFrom:(const CScenePackage::Info&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder
		sceneAppPlayerCreationProc:(nullable CreateSceneAppPlayerProc) sceneAppPlayerCreationProc;

@end

NS_ASSUME_NONNULL_END
