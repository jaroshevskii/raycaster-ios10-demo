# raycaster-ios10-demo

Textured raycasting demo (Wolfenstein 3D style) for iOS 10, following [Lode's Computer Graphics Tutorial – Raycasting 2 (textures)](https://lodev.org/cgtutor/raycasting2.html). Built with [Theos](https://theos.dev) for a 32-bit iPad 4 (A6X, `armv7s`) running iOS 10.3.4.

Programmatic UI, Auto Layout, full iPad rotation support, launch images, on-device controls, no third-party code signing required.

## Demo

Rendered on iPad 4 (iOS 10.3.4) at 60 fps:

![RaycasterDemo demo](demo.gif)

[Watch the full video](demo.mp4)

![RaycasterDemo screenshot](screenshot.png)

## Controls

- **◀ ▼ ▶ ▲** on-screen buttons — turn left / move back / turn right / move forward (hold to keep moving)
- **Drag on the view** — smooth camera rotation
- The core raycasting algorithm is exactly the one from the tutorial (DDA, perpendicular wall distance to avoid fisheye, wall textures loaded from the tutorial's PNG assets in `Resources/Textures/`, textured floor & ceiling casting, Y-side darkening)

## How it works

| File | Role |
|------|------|
| `RaycasterCore.c` | Pure C port of the tutorial's textured raycasting: map, wall texture lookup (`wallX`, `texX`, `texY`), DDA loop, per-wall-stripe rendering, textured floor/ceiling casting (per-row `rowDistance`, mirrored sky/floor), movement + collision |
| `RaycasterView.m` | Renders the RGBA buffer via `CGBitmapContext` + `CADisplayLink` at 60 fps, loads the 8 wall PNGs from the bundle into texture slots, applies input each tick |
| `RaycasterDemoViewController.m` | Control pad (4 buttons) + pan gesture for rotation, Auto Layout |
| `Resources/` | `Info.plist`, launch images, `PkgInfo` |
| `Resources/Textures/` | The tutorial's wall textures (`bluestone`, `colorstone`, `eagle`, `greystone`, `mossy`, `purplestone`, `redbrick`, `wood`) plus sprite art (`barrel`, `pillar`, `greenlight`) for the next tutorial steps |

## Requirements

- macOS with Xcode command line tools
- [Theos](https://theos.dev/docs/installation) with an iOS 10.3 SDK in `$THEOS/sdks/`
- 32-bit iOS 10 device (iPad 4 / iPhone 5 / iPhone 5c) — or jailbroken device for unsigned install

## Build

```sh
export THEOS=$HOME/theos
make                                  # .theos/obj/debug/RaycasterDemo.app
```

Build notes (SDK 10.3 + modern clang): `ARCHS = armv7 armv7s` (arm64 won't link with the patched SDK tebs), `USE_MODULES = 0`, `-Wl,-U,_memset`.

## Package as IPA

```sh
rm -rf Payload; mkdir -p Payload
ditto --noextattr --norsrc .theos/obj/debug/RaycasterDemo.app Payload/RaycasterDemo.app
COPYFILE_DISABLE=1 zip -rXq RaycasterDemo.ipa Payload
rm -rf Payload
```

## Install

Jailbroken device (e.g. [Socket](https://socket-jb.app)) + [AppSync Unified](https://cydia.akemi.ai/):

```sh
ideviceinstaller install RaycasterDemo.ipa   # over USB, no Apple ID
```

Non-jailbroken: sideload with Sideloadly / Impactor using an Apple ID.

## Screenshot

The classic tutorial map (24x24) with the tutorial's real wall textures (bluestone, colorstone, eagle, greystone, mossy, purplestone, redbrick, wood) instead of generated patterns.

## Credits

Raycasting algorithm, map and textures from [lodev.org](https://lodev.org/cgtutor/raycasting2.html) by Lode Vandevenne.