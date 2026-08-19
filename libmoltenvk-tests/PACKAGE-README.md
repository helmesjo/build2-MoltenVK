# libmoltenvk-tests - Vulkan implementation on top of Apple's Metal (tests)

This is a `build2` package containing the test suite for the
[`MoltenVK`](https://github.com/KhronosGroup/MoltenVK) library. Each
subdirectory is a separate `exe{driver}` exercising one way of getting a
`VkInstance` out of MoltenVK through the Khronos loader, all sharing the
device-and-compute-pipeline exercise in `shared.hpp`. This is dev/CI
tooling for testing an uninstalled `libmoltenvk` out of tree. See
`libmoltenvk`'s `PACKAGE-README.md` for how a real application should
consume it.

`icd/`: the ICD path. Link `libvulkan-loader%lib{vulkan}`, adhoc-import
`libmoltenvk%json{MoltenVK_icd}` so the dylib is updated, set
`VK_DRIVER_FILES` for the duration of the test via Testscript `env`, and
create an instance with portability enumeration enabled.

`direct-driver-loading/`: `VK_LUNARG_direct_driver_loading`. Link
`libvulkan-loader%lib{vulkan}`, adhoc-import `libmoltenvk%libs{MoltenVK}`
so the dylib is updated but never linked, `dlopen` it directly at
runtime, and hand the loader `vk_icdGetInstanceProcAddr` through
`VkInstanceCreateInfo`'s `pNext` chain. No json, no environment variable.
Requires ICD interface version 7, which a patched
`libmoltenvk/src/MoltenVK/Vulkan/vulkan.mm` reports, see
[KhronosGroup/MoltenVK#2663](https://github.com/KhronosGroup/MoltenVK/issues/2663).

`bundle/`: the `.app` bundle path. Assembles a minimal bundle structure
(`Driver.app/Contents/MacOS/driver`, no `Info.plist`) around the driver,
with `libmoltenvk%json{MoltenVK_icd-bundle}` and a copy of the dylib
placed in `Contents/Resources/vulkan/icd.d/`, and runs it with a
completely empty environment. Proves the loader's `CFBundleGetMainBundle`
bundle detection is purely path-structure-based: no `Info.plist`, no
codesigning, no `open`/LaunchServices needed for this to work.


## Importable targets

This package exports no targets.


## Configuration variables

This package provides no configuration variables.
