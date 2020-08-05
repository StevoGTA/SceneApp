//----------------------------------------------------------------------------------------------------------------------
//	SceneAppViewController.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#import "CSceneAppPlayer.h"
#import "UKTGPUView.h"

NS_ASSUME_NONNULL_BEGIN

//----------------------------------------------------------------------------------------------------------------------
// MARK: SceneAppViewController

@interface SceneAppViewController : UIViewController

// MARK: Properties
@property (nonatomic, readonly)			CSceneAppPlayer&	sceneAppPlayer;

@property (nonatomic, strong, nullable)	void				(^openURLProc)(NSURL* url);
@property (nonatomic, strong, nullable)	void				(^handleCommandProc)(const CString& command,
																	const CDictionary& commandInfo);

// MARK: Class methods
+ (TArray<SScenePackageInfo>) scenePackageInfosIn:(const CFolder&) sceneAppContentFolder;

// MARK: Instance methods
- (instancetype) initWithView:(UIView<UKTGPUView>*) view sceneAppContentFolder:(const CFolder&) sceneAppContentFolder;
- (instancetype) initWithView:(UIView<UKTGPUView>*) view sceneAppContentFolder:(const CFolder&) sceneAppContentFolder
		sceneAppPlayerCreationProc:
				(nullable CSceneAppPlayer* (^)(CGPU& gpu, const SSceneAppPlayerProcsInfo& sceneAppPlayerProcsInfo))
						sceneAppPlayerCreationProc;

- (void) loadScenesFrom:(const SScenePackageInfo&) scenePackageInfo;

@end

NS_ASSUME_NONNULL_END
