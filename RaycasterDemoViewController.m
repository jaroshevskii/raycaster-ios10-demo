#import "RaycasterDemoViewController.h"
#import "RaycasterView.h"
#import <objc/runtime.h>

@implementation RaycasterDemoViewController {
    RaycasterView *_renderView;
    UIButton *_fwd;
    UIButton *_back;
    UIButton *_left;
    UIButton *_right;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor blackColor];

    _renderView = [[RaycasterView alloc] initWithFrame:CGRectZero];
    _renderView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:_renderView];

    UIPanGestureRecognizer *pan = [[UIPanGestureRecognizer alloc]
                                   initWithTarget:self action:@selector(onPan:)];
    [_renderView addGestureRecognizer:pan];

    UIStackView *row = [[UIStackView alloc] init];
    row.translatesAutoresizingMaskIntoConstraints = NO;
    row.axis = UILayoutConstraintAxisHorizontal;
    row.distribution = UIStackViewDistributionFillEqually;
    row.spacing = 12;
    [self.view addSubview:row];

    _left = [self makeButton:@"◀"];
    _back = [self makeButton:@"▼"];
    _right = [self makeButton:@"▶"];
    _fwd = [self makeButton:@"▲"];
    [row addArrangedSubview:_left];
    [row addArrangedSubview:_back];
    [row addArrangedSubview:_right];
    [row addArrangedSubview:_fwd];

    [self bindButton:_left flagBlock:^(RaycasterView *v) { v.turnLeft = YES; }];
    [self bindButton:_right flagBlock:^(RaycasterView *v) { v.turnRight = YES; }];
    [self bindButton:_fwd flagBlock:^(RaycasterView *v) { v.moveForward = YES; }];
    [self bindButton:_back flagBlock:^(RaycasterView *v) { v.moveBackward = YES; }];

    [NSLayoutConstraint activateConstraints:@[
        [_renderView.topAnchor constraintEqualToAnchor:self.view.topAnchor],
        [_renderView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor],
        [_renderView.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor],
        [_renderView.bottomAnchor constraintEqualToAnchor:row.topAnchor constant:-8],

        [row.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [row.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [row.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:-20],
        [row.heightAnchor constraintEqualToConstant:56],
    ]];
}

- (UIButton *)makeButton:(NSString *)title {
    UIButton *b = [UIButton buttonWithType:UIButtonTypeCustom];
    [b setTitle:title forState:UIControlStateNormal];
    b.titleLabel.font = [UIFont systemFontOfSize:30];
    [b setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    b.backgroundColor = [UIColor colorWithWhite:0.15 alpha:0.9];
    b.layer.cornerRadius = 8;
    return b;
}

- (void)bindButton:(UIButton *)button flagBlock:(void(^)(RaycasterView *v))setFlag {
    [button addTarget:self action:@selector(pressDown:) forControlEvents:UIControlEventTouchDown];
    [button addTarget:self action:@selector(pressUp:) forControlEvents:UIControlEventTouchUpInside];
    [button addTarget:self action:@selector(pressUp:) forControlEvents:UIControlEventTouchUpOutside];
    [button addTarget:self action:@selector(pressUp:) forControlEvents:UIControlEventTouchCancel];
    objc_setAssociatedObject(button, "flagBlock", setFlag, OBJC_ASSOCIATION_COPY_NONATOMIC);
}

- (void)pressDown:(UIButton *)sender {
    void (^block)(RaycasterView *) = objc_getAssociatedObject(sender, "flagBlock");
    if (block) block(_renderView);
}

- (void)pressUp:(UIButton *)sender {
    if (sender == _fwd) _renderView.moveForward = NO;
    if (sender == _back) _renderView.moveBackward = NO;
    if (sender == _left) _renderView.turnLeft = NO;
    if (sender == _right) _renderView.turnRight = NO;
}

- (void)onPan:(UIPanGestureRecognizer *)g {
    CGPoint t = [g translationInView:_renderView];
    [g setTranslation:CGPointZero inView:_renderView];
    [_renderView applyExtraTurn:-t.x * 0.01f];
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskAll;
}

@end