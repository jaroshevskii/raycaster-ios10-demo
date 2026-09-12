ARCHS = armv7 armv7s
TARGET = iphone:clang:10.3:8.0
include $(THEOS)/makefiles/common.mk

APPLICATION_NAME = RaycasterDemo
RaycasterDemo_FILES = main.m RaycasterDemoAppDelegate.m RaycasterDemoViewController.m RaycasterView.m RaycasterCore.c
RaycasterDemo_FRAMEWORKS = UIKit Foundation CoreGraphics QuartzCore
RaycasterDemo_CFLAGS = -fobjc-arc -fno-builtin
RaycasterDemo_USE_MODULES = 0
RaycasterDemo_LDFLAGS = -Wl,-U,_memset

include $(THEOS_MAKE_PATH)/application.mk