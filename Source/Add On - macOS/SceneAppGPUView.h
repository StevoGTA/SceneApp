//----------------------------------------------------------------------------------------------------------------------
//	SceneAppGPUView.h			©2019 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#pragma once

#import "CGPU.h"
#import "TimeAndDate.h"

#import <AppKit/AppKit.h>

NS_ASSUME_NONNULL_BEGIN

//----------------------------------------------------------------------------------------------------------------------
// MARK: SceneAppGPUView
@protocol SceneAppGPUView

// MARK: Properties
@property (nonatomic, readonly)	CGPU&	gpu;

@property (nonatomic, strong)	void	(^mouseDownProc)(NSEvent* event);
@property (nonatomic, strong)	void	(^mouseDraggedProc)(NSEvent* event);
@property (nonatomic, strong)	void	(^mouseUpProc)(NSEvent* event);

@property (nonatomic, strong)	void	(^periodicProc)(UniversalTime outputTime);

// MARK: Instance methods
- (void) installPeriodic;
- (void) removePeriodic;

@end

NS_ASSUME_NONNULL_END
