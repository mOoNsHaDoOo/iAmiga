//
//  PKCustomKeyboard.h
//  PunjabiKeyboard
//
//  Created by Kulpreet Chilana on 7/19/12.
//  Copyright (c) 2012 Kulpreet Chilana. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import "SDL.h"

@interface PMCustomKeyboard : UIViewController <UIInputViewAudioFeedback>

@property (strong) id<UITextInput> textView;

- (IBAction)keyDown:(id)sender;
- (IBAction)keyUp:(id)sender;

@end
