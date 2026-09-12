#import "RaycasterDemoAppDelegate.h"
#import "RaycasterDemoViewController.h"

@implementation RaycasterDemoAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.window.rootViewController = [[RaycasterDemoViewController alloc] init];
    [self.window makeKeyAndVisible];
    return YES;
}

@end