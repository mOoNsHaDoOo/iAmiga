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
#import "EMUFileInfo.h"

@interface EMUBrowser : NSObject


/**
 * Returns an array of EMUFileInfo instances for all available adf files.
 */
- (NSArray *)getAdfFileInfos;

/**
 * Returns an array of EMUFileInfo instances for all available files that match the specified extensions (for ex @[adf, ADF]).
 */
- (NSArray *)getFileInfosForExtensions:(NSArray *)extensions;

/**
 * Returns a EMUFileInfo instance matching the specified file name (for ex xyz.adf), or nil if no match is found.
 */
- (EMUFileInfo *)getFileInfoForFileName:(NSString *)fileName;

/**
 * Returns an array of EMUFileInfo instances for all available files whose names match the specified file names (@[xyz.adf, blah.rom].
 */
- (NSArray *)getFileInfosForFileNames:(NSArray *)fileName;

/**
 * Methods for search bar
 */
-(NSArray *)getFileInfosWithFileNameFilter:(NSString *)fileNameFilter;
-(NSArray *)getFileInfosInDirectory:(NSString *)directory fileNameFilter:(NSString *)fileNameFilter;


@end
