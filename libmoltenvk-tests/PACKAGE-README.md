# libmoltenvk-tests - Vulkan implementation on top of Apple's Metal (tests)

This is a `build2` package containing the test suite for the
[`MoltenVK`](https://github.com/KhronosGroup/MoltenVK) library.
It exercises the Khronos ICD path: link `libvulkan-loader%lib{vulkan}`,
adhoc-import `libmoltenvk%json{MoltenVK_icd}` so the dylib is updated,
set `VK_DRIVER_FILES` for the duration of the test via Testscript `env`,
and create an instance with portability enumeration enabled. This is a
dev/CI convenience for testing an uninstalled `libmoltenvk` out of tree.
See `libmoltenvk`'s `PACKAGE-README.md` for how a real application should
consume the ICD (search path or `VK_ADD_DRIVER_FILES`, no import).


## Importable targets

This package exports no targets.


## Configuration variables

This package provides no configuration variables.
