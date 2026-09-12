#import <UIKit/UIKit.h>

@interface RaycasterView : UIView
@property (nonatomic, assign) BOOL moveForward;
@property (nonatomic, assign) BOOL moveBackward;
@property (nonatomic, assign) BOOL turnLeft;
@property (nonatomic, assign) BOOL turnRight;
- (void)applyExtraTurn:(CGFloat)angle;
- (void)start;
@end