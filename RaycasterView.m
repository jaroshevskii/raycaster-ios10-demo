#import "RaycasterView.h"
#import "RaycasterCore.h"
#import <QuartzCore/QuartzCore.h>

#define RBUF_WIDTH 320
#define RBUF_HEIGHT 200
#define RTEX 64

static void SetTextureFromImage(int slot, UIImage *image) {
    CGImageRef cg = [image CGImage];
    if (!cg) return;
    unsigned char *buf = malloc(RTEX * RTEX * 4);
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
    CGContextRef ctx = CGBitmapContextCreate(buf, RTEX, RTEX, 8, RTEX * 4,
                                             space, kCGImageAlphaPremultipliedLast);
    if (ctx) {
        CGContextClearRect(ctx, CGRectMake(0, 0, RTEX, RTEX));
        CGContextDrawImage(ctx, CGRectMake(0, 0, RTEX, RTEX), cg);
        rc_set_texture(slot, buf, RTEX, RTEX);
        CGContextRelease(ctx);
    }
    CGColorSpaceRelease(space);
    free(buf);
}

@implementation RaycasterView {
    CGContextRef _ctx;
    unsigned char *_pixels;
    CADisplayLink *_link;
    CFTimeInterval _last;
    CGFloat _extraTurn;
    BOOL _dirty;
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor blackColor];
        self.layer.contentsGravity = kCAGravityResizeAspect;
        self.contentMode = UIViewContentModeRedraw;

        _pixels = calloc(RBUF_WIDTH * RBUF_HEIGHT * 4, 1);
        CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
        _ctx = CGBitmapContextCreate(_pixels, RBUF_WIDTH, RBUF_HEIGHT, 8,
                                     RBUF_WIDTH * 4, space, kCGImageAlphaPremultipliedLast);
        CGColorSpaceRelease(space);

        rc_init();
        NSArray<NSString *> *names = @[@"bluestone", @"colorstone", @"eagle",
                                       @"greystone", @"mossy", @"purplestone",
                                       @"redbrick", @"wood"];
        for (NSUInteger i = 0; i < names.count; i++) {
            NSString *path = [[NSBundle mainBundle] pathForResource:names[i]
                                                             ofType:@"png"
                                                        inDirectory:@"Textures"];
            if (!path) continue;
            SetTextureFromImage((int)i, [UIImage imageWithContentsOfFile:path]);
        }
        _dirty = YES;
    }
    return self;
}

- (void)start {
    if (_link) return;
    _last = CACurrentMediaTime();
    _link = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
    [_link addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

- (void)stop {
    [_link invalidate];
    _link = nil;
}

- (void)applyExtraTurn:(CGFloat)angle {
    _extraTurn += angle;
}

- (void)tick:(CADisplayLink *)link {
    CFTimeInterval now = CACurrentMediaTime();
    double dt = now - _last;
    _last = now;
    if (dt <= 0.0 || dt > 0.1) dt = 1.0 / 60.0;

    double moveSpeed = dt * 4.0;
    double rotSpeed = dt * 2.6;
    CGFloat turn = _extraTurn;
    _extraTurn = 0.0;

    BOOL anyInput = self.moveForward || self.moveBackward || self.turnLeft || self.turnRight || turn != 0.0f;
    if (anyInput) _dirty = YES;

    rc_tick(self.moveForward, self.moveBackward, self.turnLeft, self.turnRight,
            moveSpeed, rotSpeed, turn);

    if (_dirty) {
        rc_render(_pixels, RBUF_WIDTH, RBUF_HEIGHT);
        CGImageRef img = CGBitmapContextCreateImage(_ctx);
        if (img) {
            self.layer.contents = (__bridge id)img;
            CGImageRelease(img);
        }
        _dirty = NO;
    }
}

- (void)willMoveToWindow:(UIWindow *)newWindow {
    [super willMoveToWindow:newWindow];
    if (newWindow) [self start];
    else [self stop];
}

- (void)dealloc {
    [self stop];
    if (_ctx) CGContextRelease(_ctx);
    if (_pixels) free(_pixels);
}

@end