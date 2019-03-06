/*
 Frodo, Commodore 64 emulator for the iPhone
 Copyright (C) 2007, 2008 Stuart Carnie
 See gpl.txt for license information.
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#import <UIKit/UIKit.h>
#import "AdfImporter.h"

@class EMUFileInfo;

@protocol SelectRomDelegate
- (void)didSelectROM:(EMUFileInfo*)fileInfo withContext:(id)context;
@end

@interface EMUROMBrowserViewController : UITableViewController<UISearchBarDelegate>

+ (NSString *)getFileImportedNotificationName;

@property (nonatomic, assign) id<SelectRomDelegate>	delegate;
@property (nonatomic, retain) id context;
@property (nonatomic, retain) NSArray *extensions; // extensions of files to show
@property (retain, nonatomic) IBOutlet UISearchBar *searchBar;
@property (nonatomic, retain) NSMutableArray *results;
@property (nonatomic, retain) NSMutableArray *sectionRoms;
@property (nonatomic, retain) NSIndexPath *selectedIndexPath;
@property (nonatomic) int selectedRow;
@property (nonatomic) bool segueSelect;

@end
