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
Currently a known, tracked skip against MoltenVK 1.4.2
(`vk_icdNegotiateLoaderICDInterfaceVersion` caps at interface version 5,
the loader requires 7 for this extension), see
[KhronosGroup/MoltenVK#2663](https://github.com/KhronosGroup/MoltenVK/issues/2663)
and the comment in `direct-driver-loading/driver.cpp`.


## Importable targets

This package exports no targets.


## Configuration variables

This package provides no configuration variables.
