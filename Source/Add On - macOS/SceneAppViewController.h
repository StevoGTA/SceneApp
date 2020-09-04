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

// MARK: Properties
@property (nonatomic, readonly)			CSceneAppPlayer&	sceneAppPlayer;

@property (nonatomic, strong, nullable)	void				(^openURLProc)(NSURL* url);
@property (nonatomic, strong, nullable)	void				(^handleCommandProc)(const CString& command,
																	const CDictionary& commandInfo);

// MARK: Class methods
+ (TArray<SScenePackageInfo>) scenePackageInfosIn:(const CFolder&) folder;

// MARK: Instance methods
- (instancetype) initWithView:(NSView<AKTGPUView>*) view;

- (void) loadScenesFrom:(const SScenePackageInfo&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder;
- (void) loadScenesFrom:(const SScenePackageInfo&) scenePackageInfo
		sceneAppContentFolder:(const CFolder&) sceneAppContentFolder
		sceneAppPlayerCreationProc:
				(nullable CSceneAppPlayer* (^)(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo))
						sceneAppPlayerCreationProc;

@end

NS_ASSUME_NONNULL_END
