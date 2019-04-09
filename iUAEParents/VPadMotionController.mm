//
//  VPadMotionController.mm
//  iUAE
//
//  Created by MrStargazer on 27.03.16.
//
//
//  iUAE is free software: you may copy, redistribute
//  and/or modify it under the terms of the GNU General Public License as
//  published by the Free Software Foundation, either version 2 of the
//  License, or (at your option) any later version.
//
//  This file is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//

#import "VPadMotionController.h"
#import "touchstick.h"
#import "CGVector.h"
#import "CocoaUtility.h"
#import "SDL_events.h"
#import "JoypadKey.h"
#import "Settings.h"
#import "MultiPeerConnectivityController.h"

extern CJoyStick g_touchStick;

@implementation VPadMotionController {
}

static CMAttitude *refAttitude = nil;
static CMMotionManager *motionController = nil;
static Settings *settings;
static BOOL active = NO;
const float motionUpdateInterval = 1.0/30.0;
static CJoyStick *TheJoyStick;
static MultiPeerConnectivityController *mpcController;
static int buttontoreleasehorizontal;
static int buttontoreleasevertical;

+ (BOOL) isActive{
    return active;
}

// activates motion controlling
+ (void) setActive{
    active = YES;
    TheJoyStick = &g_touchStick;
    [self stopUpdating];
    motionController.deviceMotionUpdateInterval = motionUpdateInterval;
    
    // register for motion updates
    [motionController startDeviceMotionUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:^(CMDeviceMotion *motionData, NSError *error){
        [self receiveMotionData:motionData.attitude];
        if(error){
            
            NSLog(@"%@", error);
        }
    }];
    
    mpcController = [MultiPeerConnectivityController getinstance];
}

// receivces frequently motion data and translate it into joystick directions
+ (void) receiveMotionData : (CMAttitude*)currentAttitude{
    // process data
    
    TouchStickDPadState dpadState;
    float deadZone = settings.gyroSensitivity;
    BOOL toggleUpDown = settings.gyroToggleUpDown;
    
    if (refAttitude != nil){
        [currentAttitude multiplyByInverseOfAttitude: refAttitude];
    }
    
    if (currentAttitude.pitch < -deadZone && currentAttitude.roll < -deadZone){
        dpadState = toggleUpDown ? DPadUpRight : DPadDownRight;
    }
    else if (currentAttitude.pitch < -deadZone && currentAttitude.roll > deadZone){
        dpadState = toggleUpDown ? DPadDownRight : DPadUpRight;
    }
    else if (currentAttitude.pitch > deadZone && currentAttitude.roll < -deadZone){
        dpadState = toggleUpDown ? DPadUpLeft : DPadDownLeft;
    }
    else if (currentAttitude.pitch > deadZone && currentAttitude.roll > deadZone){
        dpadState = toggleUpDown ? DPadDownLeft : DPadUpLeft;
    }
    else if (currentAttitude.roll > deadZone && ABS(currentAttitude.pitch) < deadZone)
    {

        dpadState = toggleUpDown ? DPadDown : DPadUp;
    }
    else if (currentAttitude.roll < -deadZone && ABS(currentAttitude.pitch) < deadZone){
        dpadState = toggleUpDown ? DPadUp : DPadDown;
    }
    else if (currentAttitude.pitch > deadZone && ABS(currentAttitude.roll) < deadZone){
        dpadState = DPadLeft;
    }
    else if (currentAttitude.pitch < -deadZone && ABS(currentAttitude.roll) < deadZone){
        dpadState = DPadRight;
    }
    else {
        dpadState = DPadCenter;
    }
    
    int buttonvertical = (int)[mpcController dpadstatetojoypadkey:@"vertical" hatstate:dpadState];
    int buttonhorizontal = (int)[mpcController dpadstatetojoypadkey:@"horizontal" hatstate:dpadState];
    
    [mpcController handleinputdirections:dpadState buttontoreleasevertical:buttontoreleasevertical buttontoreleasehorizontal: buttontoreleasehorizontal deviceid:kVirtualPad];
    
    buttontoreleasevertical = buttonvertical;
    buttontoreleasehorizontal = buttonhorizontal;
    
}

// disables motion controlling
+ (void) disable{
    [self stopUpdating];
    active = NO;
}

// interface to apples motion api (CoreMotion)
+ (CMMotionManager*)motionManager {
    
    if (motionController != nil){
        return motionController;
    }
 
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        motionController = [[CMMotionManager alloc] init];
        settings = [[Settings alloc] init];
        motionController.deviceMotionUpdateInterval = 1.0/60.0;
        [motionController startDeviceMotionUpdates];
        
    });
    
    return motionController;
}

// returns the reference position set with method calibrate
+ (CMAttitude*)referenceAttitude{
    return refAttitude;
}

// starts receiving motion updates
+ (void)startUpdating {
    if (!self.motionManager.isDeviceMotionActive){
        [self.motionManager startDeviceMotionUpdates];
    }
}

// stops motion controller receiving motion updates
+ (void)stopUpdating {
    if ([self motionManager].isDeviceMotionActive){
        [[self motionManager] stopDeviceMotionUpdates];
    }
}

// sets der current position of the device as reference for movement
+ (void)calibrate {
    refAttitude = self.motionManager.deviceMotion.attitude.copy;
}

+ (CMAttitude*)getMotion {
    CMAttitude *currentAttitude = self.motionManager.deviceMotion.attitude;

    if (refAttitude != nil){
      [currentAttitude multiplyByInverseOfAttitude: refAttitude];
    }

    return currentAttitude;
}

- (void)dealloc {
    // Should never be called, but just here for clarity really.
    [super dealloc];
}

@end
