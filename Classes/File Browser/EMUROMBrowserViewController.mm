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

#import "EMUROMBrowserViewController.h"
#import "EMUBrowser.h"
#import "EMUFileInfo.h"
#import "EMUFileGroup.h"
#import "ScrollToRowHandler.h"

static bool g_inited = false;
static NSMutableArray *_romArray = nil;

@implementation EMUROMBrowserViewController {
    @private
    AdfImporter *_adfImporter;
    NSArray *_indexTitles;
    NSArray *_roms;
    ScrollToRowHandler *_scrollToRowHandler;
}

+ (NSString *)getFileImportedNotificationName {
    return @"FileImportedNotification";
}

- (void)viewDidLoad {
    [super viewDidLoad];
	self.title = @"Browser";
    _adfImporter = [[AdfImporter alloc] init];
    _indexTitles = [@[@"A", @"B", @"C", @"D", @"E", @"F", @"G", @"H", @"I",
                      @"J", @"K", @"L", @"M", @"N", @"O", @"P", @"Q", @"R", @"S", @"T", @"U", @"V",
                      @"W", @"X", @"Y", @"Z", @"#"] retain];
    _scrollToRowHandler = [[ScrollToRowHandler alloc] initWithTableView:self.tableView identity:[_extensions description]];
    

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onAdfChanged)
                                                 name:[EMUROMBrowserViewController getFileImportedNotificationName]
                                               object:nil];
    [self reloadAdfs];
    [_scrollToRowHandler scrollToRow];
}

- (void)reloadAdfs {
    NSMutableArray *sections = [[NSMutableArray alloc] init];
    
    for (int i = 0; i < 26; i++) {
        unichar c = i+65;
        EMUFileGroup *g = [[EMUFileGroup alloc] initWithSectionName:[NSString stringWithFormat:@"%c", c]];
        [sections addObject:g];
    }
    [sections addObject:[[EMUFileGroup alloc] initWithSectionName:@"#"]];
    
    EMUBrowser *browser = [[EMUBrowser alloc] init];
    
    NSArray *files;
    
    if (!g_inited) {
        _romArray = [[NSMutableArray alloc] init];
        files = [browser getFileInfos];
    } else
        files = _romArray;
    
    for (EMUFileInfo* f in files) {
        EMUFileGroup *g;
        
        if (!g_inited)
            [_romArray addObject: f];
        
        unichar c = [[f fileName] characterAtIndex:0];
        c = toupper(c) - 65;
        if (c < [sections count] && c >= 0)
            g = (EMUFileGroup*)[sections objectAtIndex:c];
        else
            g = (EMUFileGroup*)[sections objectAtIndex:26];
        
        [g.files addObject:f];
    }
    [browser release];
    _sectionRoms = sections;
    g_inited = 1;
}

-(void)searchAdfs
{
    NSPredicate *srcResults;
    
    if ([_searchBar.text length] == 0) {
        _results = nil;
        return;
    }
    
    srcResults = [NSPredicate predicateWithFormat: @"SELF.fileName contains [search] %@", _searchBar.text];
    if (srcResults)
        _results = [[_romArray filteredArrayUsingPredicate: srcResults] mutableCopy];
}

-(void)searchBar:(UISearchBar *)searchBar textDidChange: (NSString *) term
{
    [self searchAdfs];
    [self.tableView reloadData];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	return _roms.count;
}

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView {
    return _indexTitles;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
	EMUFileGroup *g = (EMUFileGroup*)[_roms objectAtIndex:section];
	return g.sectionName;
}

- (NSInteger)tableView:(UITableView *)tableView sectionForSectionIndexTitle:(NSString *)title atIndex:(NSInteger)index {
    if (_results == nil) {
        unichar c = [title characterAtIndex:0];
        if (c > 64 && c < 91)
            return c - 65;
        
        return 26;
    }
    
    return 0;
}

- (void)onAdfChanged {
    [_scrollToRowHandler clearRow];
    [self reloadAdfs];
    [self.tableView reloadData];
}

- (NSInteger)tableView:(UITableView *)table numberOfRowsInSection:(NSInteger)section {
    EMUFileGroup *g = (EMUFileGroup*)[_roms objectAtIndex:section];
    return g.files.count;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    EMUFileGroup *g = (EMUFileGroup*)[_roms objectAtIndex:indexPath.section];
    EMUFileInfo *fi = [g.files objectAtIndex:indexPath.row];
    [self.navigationController popViewControllerAnimated:YES];
    [self.delegate didSelectROM:fi withContext:self.context];
    [_scrollToRowHandler setRow:indexPath];
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

- (NSArray *)tableView:(UITableView *)tableView editActionsForRowAtIndexPath:(NSIndexPath *)indexPath {
    NSMutableArray *editActions = [NSMutableArray arrayWithCapacity:2];
    
    EMUFileInfo *fileInfo = [self getFileInfoForIndexPath:indexPath];
    
    UITableViewRowAction *shareAction = [UITableViewRowAction rowActionWithStyle:UITableViewRowActionStyleNormal title:@"Share" handler:^(UITableViewRowAction *action, NSIndexPath *indexPath) {
        NSURL *url = [NSURL fileURLWithPath:fileInfo.path];
        NSString *string = @"iAmiga File Sharing";
        UIActivityViewController *activityViewController =
            [[[UIActivityViewController alloc] initWithActivityItems:@[string, url] applicationActivities:nil] autorelease];
        if ([activityViewController respondsToSelector:@selector(popoverPresentationController)]) {
            // iOS8: setting the sourceView is required for iPad
            UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
            activityViewController.popoverPresentationController.sourceView = [cell.subviews firstObject]; // firstObject == cell actions
        }
        [self presentViewController:activityViewController animated:YES completion:^{ }];
    }];
    shareAction.backgroundColor = [UIColor blueColor];
    [editActions addObject:shareAction];
    
    BOOL okToDelete = [_adfImporter isDownloadedAdf:fileInfo.path];

    if (okToDelete) {
        UITableViewRowAction *deleteAction = [UITableViewRowAction rowActionWithStyle:UITableViewRowActionStyleDestructive title:@"Delete" handler:^(UITableViewRowAction *action, NSIndexPath *indexPath) {
            BOOL deleted = [[NSFileManager defaultManager] removeItemAtPath:fileInfo.path error:NULL];
            if (deleted) {
                [_scrollToRowHandler clearRow];
                [self reloadAdfs];
                [tableView beginUpdates];
                [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
                [tableView endUpdates];
            }
        }];
        [editActions addObject:deleteAction];
    }
    return editActions;
}

#define CELL_ID @"DiskCell"

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CELL_ID];
    if (!cell) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CELL_ID] autorelease];
    }
	
    cell.autoresizingMask = UIViewAutoresizingFlexibleWidth;
    EMUFileInfo *fileInfo = [self getFileInfoForIndexPath:indexPath];
    cell.textLabel.text = [fileInfo fileName];
	
    return cell;
}

- (EMUFileInfo *)getFileInfoForIndexPath:(NSIndexPath *)indexPath {
    EMUFileGroup *group = [_roms objectAtIndex:indexPath.section];
    return [group.files objectAtIndex:indexPath.row];
}

- (void)dealloc {

    self.context = nil;
    self.extensions = nil;
    [_roms release];
    [_indexTitles release];
    [_adfImporter release];
    [_scrollToRowHandler release];

    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [_searchBar release];
    [super dealloc];
}

@end
