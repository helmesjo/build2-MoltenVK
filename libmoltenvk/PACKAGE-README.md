# libmoltenvk - Vulkan implementation on top of Apple's Metal

This is a `build2` package for the [`MoltenVK`](https://github.com/KhronosGroup/MoltenVK)
C++ library. It implements a subset of the Vulkan 1.4 graphics and compute API
on top of Apple's Metal framework. This package currently targets macOS only.


## Usage

This package is the macOS Vulkan ICD (the driver). The Khronos stack links
the loader, not this library. Do not import both `libvulkan-loader%lib{vulkan}`
and `libmoltenvk%lib{MoltenVK}` into the same image. Both export `vk*`.

### ICD mode (recommended)

The application links only the Khronos loader. It does not depend on or
import this package at all. MoltenVK is a runtime driver that the loader
`dlopen`s after reading an ICD json, exactly like any other Vulkan ICD
(a system package, Homebrew, or the LunarG SDK). A library that only
compiles against the loader must never depend on `libmoltenvk` either,
since that would pin one specific ICD onto every application that links it.

```
depends: libvulkan-loader ^1.4.359
```

```
import vk = libvulkan-loader%lib{vulkan}
exe{app}: cxx{main} $vk
```

At `vkCreateInstance`, enable `VK_KHR_portability_enumeration` and
`VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR` or the loader skips
this driver. That is Khronos policy, not specific to this package.

How the loader finds this build of MoltenVK, using the same knobs a
non-`build2` user already has:

- **Search path.** Directories the loader always scans (no environment
  variable). On macOS that includes the app bundle
  `Contents/Resources/vulkan/icd.d/`, then `etc/vulkan/icd.d/` and
  `share/vulkan/icd.d/` under `/usr/local` and `/usr`. `bpkg install`ing
  this package (or installing it via the system package manager) to one
  of those prefixes is enough, with no application-side change at all.
- **Environment.** For a prefix the loader doesn't scan, such as Homebrew
  (`/opt/homebrew`), a custom prefix, or an uninstalled out-tree, set
  `VK_ADD_DRIVER_FILES` (additive: adds to the driver list, unlike
  `VK_DRIVER_FILES`, which replaces it) to the json path. Same variable
  the LunarG SDK and a CMake MoltenVK build already use.
- **`.app` bundle.** Copy `json{MoltenVK_icd-bundle}` and
  `libMoltenVK.dylib` into `Contents/Resources/vulkan/icd.d/`.

`library_path` in the json is relative to the json file. Relocatable
installation is supported.

GLFW should use the linked loader. Do not call `glfwInitVulkanLoader` in
ICD mode.

Out-of-tree testing of *this* repo (no system MoltenVK installed) is the
one case that legitimately needs the json path before install.
`libmoltenvk-tests` is the reference for how: a plain same-configuration
import of `json{MoltenVK_icd}` plus Testscript `env` to scope
`VK_DRIVER_FILES` to a single test invocation. That pattern is dev/CI
tooling for this repo, not something application packages should copy.

### Direct-link mode

To start using `libmoltenvk` without the loader, add the following `depends`
value to your `manifest`, adjusting the version constraint as appropriate:

```
depends: libmoltenvk ^1.4.2
```

Then import the library in your `buildfile`:

```
import libs = libmoltenvk%lib{MoltenVK}
```

The target name is mixed-case `lib{MoltenVK}` (not `lib{moltenvk}`) to match
the upstream binary name (`libMoltenVK.dylib`).

Linking against it transitively imports `libvulkan-headers` and the Apple
frameworks required by the public headers (`Foundation`, `Metal`,
`CoreGraphics`). GLFW then needs `glfwInitVulkanLoader(vkGetInstanceProcAddr)`
or it will `dlopen` `libvulkan.1.dylib`.

The ICD json is not used in this mode.


## Importable targets

This package provides the following importable targets:

```
lib{MoltenVK}
json{MoltenVK_icd}
json{MoltenVK_icd-bundle}
```

`lib{MoltenVK}` is the compiled library, for direct-link mode only.

`json{MoltenVK_icd}` is the out-of-tree ICD manifest. It exists for this
project's own out-of-tree testing (see `libmoltenvk-tests`) and is not a
general consumer API. Ordinary ICD-mode consumers need neither `depends`
nor `import` on this package at all.

`json{MoltenVK_icd-bundle}` is a static ICD manifest (`library_path`
relative to its own directory) for packagers copying MoltenVK into a
`.app` bundle's `Contents/Resources/vulkan/icd.d/`.


## Configuration variables

This package provides the following configuration variables:

```
[bool]   config.libmoltenvk.use_metal_private_api ?= false
[string] config.libmoltenvk.log_level             ?= 'info'
```

`config.libmoltenvk.use_metal_private_api` mirrors upstream's
`MVK_USE_METAL_PRIVATE_API`. When true, MoltenVK may use private Metal
interfaces to implement some Vulkan features. Default is false (App Store
safe).

`config.libmoltenvk.log_level` mirrors upstream's `MVK_CONFIG_LOG_LEVEL`.
Valid values are `debug`, `info`, `warn`, `error`, and `off`.

SPIRV-Tools integration is always excluded in this package
(`MVK_EXCLUDE_SPIRV_TOOLS=1`). Enabling it requires SPIRV-Tools and
SPIRV-Headers packages that are not available yet.
