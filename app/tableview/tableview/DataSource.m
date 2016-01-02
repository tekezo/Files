#import "DataSource.h"

@interface DataSource ()
@property NSMutableArray* dataSource;
@end

@implementation DataSource

- (instancetype)init {
  self = [super init];

  if (self) {
    self.dataSource = [NSMutableArray new];

    for (int i = 0; i < 5; ++i) {
      NSMutableDictionary* dict = [NSMutableDictionary new];
      dict[@"index"] = @(i);
      dict[@"level"] = @(0);
      [self.dataSource addObject:dict];

      if (i > 0) {
        dict[@"children"] = [NSMutableArray new];
        for (int j = 0; j < i; ++j) {
          NSMutableDictionary* d = [NSMutableDictionary new];
          d[@"index"] = @(j);
          d[@"level"] = @(1);
          [dict[@"children"] addObject:d];
        }
      }
    }
  }

  return self;
}

- (NSInteger)outlineView:(NSOutlineView*)outlineView numberOfChildrenOfItem:(id)item {
  if (item == nil) {
    return [self.dataSource count];
  } else {
    return [item[@"children"] count];
  }
}

- (BOOL)outlineView:(NSOutlineView*)outlineView isItemExpandable:(id)item {
  if (item == nil) {
    return YES;
  } else {
    return [item[@"children"] count] > 0;
  }
}

- (id)outlineView:(NSOutlineView*)outlineView child:(NSInteger)index ofItem:(id)item {
  if (item == nil) {
    return self.dataSource[index];
  } else {
    return item[@"children"][index];
  }
}

@end
