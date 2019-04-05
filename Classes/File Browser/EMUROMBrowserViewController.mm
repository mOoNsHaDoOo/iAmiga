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

static NSMutableDictionary *_files = nil;
static NSMutableDictionary *_lastSeachTerms = nil;

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
    _scrollToRowHandler = [[ScrollToRowHandler alloc] initWithTableView:self.tableView identity:[_extension description]];
    
    if (_files == nil)
        _files = [NSMutableDictionary new];
    
    if (_lastSeachTerms == nil)
        _lastSeachTerms = [NSMutableDictionary new];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onAdfChanged)
                                                 name:[EMUROMBrowserViewController getFileImportedNotificationName]
                                               object:nil];
    [self loadFiles: false];
    NSString *lastSearchTerm = [_lastSeachTerms objectForKey: _extension];
    if (lastSearchTerm != nil) {
        _searchBar.text = lastSearchTerm;
        [self searchFiles];
        [self.tableView reloadData];
    }
    _searchBar.placeholder = [NSString stringWithFormat: @"search %@", _extension];
    [_scrollToRowHandler scrollToRow];
}

- (void)loadFiles: (bool) forceReload {
    NSMutableArray *sections = [[NSMutableArray alloc] init];
    
    for (int i = 0; i < 26; i++) {
        unichar c = i+65;
        EMUFileGroup *g = [[EMUFileGroup alloc] initWithSectionName:[NSString stringWithFormat:@"%c", c]];
        [sections addObject:g];
    }
    [sections addObject:[[EMUFileGroup alloc] initWithSectionName:@"#"]];
    
    EMUBrowser *browser = [[EMUBrowser alloc] init];
    NSArray *files;
    
    files = [_files objectForKey: _extension];
    if (files == nil || forceReload) {
        files = [browser getFileInfosWithFileNameFilter: _extension];
        _files[_extension] = files;
    }
    
    _activeFilesCollection = files;
    for (EMUFileInfo* f in files) {
        EMUFileGroup *g;
        
        unichar c = [[f fileName] characterAtIndex:0];
        c = toupper(c) - 65;
        if (c < [sections count] && c >= 0)
            g = (EMUFileGroup*)[sections objectAtIndex:c];
        else
            g = (EMUFileGroup*)[sections objectAtIndex:26];
        
        [g.files addObject:f];
    }
    [browser release];
    _sectionFiles = sections;
}

-(void)searchFiles
{
    NSPredicate *srcResults;
    
    if ([_searchBar.text length] == 0) {
        _results = nil;
        return;
    }
    
    srcResults = [NSPredicate predicateWithFormat: @"SELF.fileName contains [search] %@", _searchBar.text];
    if (srcResults)
        _results = [[_activeFilesCollection filteredArrayUsingPredicate: srcResults] mutableCopy];
}

-(void)searchBar:(UISearchBar *)searchBar textDidChange: (NSString *) term
{
    _lastSeachTerms[_extension] = _searchBar.text;
    [self searchFiles];
    [self.tableView reloadData];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    if (_results == nil)
        return _sectionFiles.count;
    else {
        return 1;
    }
}

- (NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView {
    if (_results == nil)
        return [_indexTitles copy];
    else
        return @[@"S"];
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    if (_results == nil) {
        EMUFileGroup *g = (EMUFileGroup*)[_sectionFiles objectAtIndex:section];
        return g.sectionName;
    } else {
        return @"Found files";
    }
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
    [self loadFiles: true];
    [self.tableView reloadData];
}

- (NSInteger)tableView:(UITableView *)table numberOfRowsInSection:(NSInteger)section {
    if (_results == nil) {
        EMUFileGroup *g = (EMUFileGroup*)[_sectionFiles objectAtIndex:section];
        return g.files.count;
    }
    
    return _results.count;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    if (indexPath == _selectedIndexPath || _segueSelect)
        return;
    
    UITableViewCell *cell = [tableView cellForRowAtIndexPath: _selectedIndexPath];
    
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell = [tableView cellForRowAtIndexPath:indexPath];
    //cell.accessoryType = UITableViewCellAccessoryCheckmark;
    _selectedIndexPath = indexPath;
    
    EMUFileInfo *fi = [self getFileInfoForIndexPath: indexPath];
    [self.navigationController popViewControllerAnimated:YES];
    if (self.delegate)
        [self.delegate didSelectROM:fi withContext: _context];
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    EMUFileInfo *fileInfo = [self getFileInfoForIndexPath:indexPath];
    return [_adfImporter isDownloadedAdf:fileInfo.path];
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        EMUFileInfo *fileInfo = [self getFileInfoForIndexPath:indexPath];
        BOOL deleteOk = [[NSFileManager defaultManager] removeItemAtPath:fileInfo.path error:NULL];
        if (deleteOk) {
            [self loadFiles: true];
            [tableView beginUpdates];
            [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
            [tableView endUpdates];
        }
    }
}

#define CELL_ID @"DiskCell"

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CELL_ID];
    if (cell == nil)
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CELL_ID] autorelease];
    
    cell.autoresizingMask = UIViewAutoresizingFlexibleWidth;
    if ([indexPath compare: _selectedIndexPath] == NSOrderedSame)
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
    else
        cell.accessoryType = UITableViewCellAccessoryNone;
    
    EMUFileInfo *fileInfo = [self getFileInfoForIndexPath:indexPath];
    cell.textLabel.text = [fileInfo fileName];
    
    return cell;
}

- (EMUFileInfo *)getFileInfoForIndexPath:(NSIndexPath *)indexPath {
    if (_results == nil) {
        EMUFileGroup *group = [_sectionFiles objectAtIndex:indexPath.section];
        return [group.files objectAtIndex:indexPath.row];
    } else {
        return [_results objectAtIndex:indexPath.row];
    }
}


- (void)dealloc {

    self.context = nil;
    self.extension= nil;
    [_roms release];
    [_indexTitles release];
    [_adfImporter release];
    [_scrollToRowHandler release];

    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [_searchBar release];
    [super dealloc];
}

@end
