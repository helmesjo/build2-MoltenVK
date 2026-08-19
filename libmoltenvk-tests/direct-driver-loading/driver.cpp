#include "../shared.hpp"

#include <dlfcn.h>

// Direct driver loading (VK_LUNARG_direct_driver_loading): dlopen
// MoltenVK directly (RTLD_LOCAL, so its vk* symbols never enter the
// global namespace and never collide with the linked loader's), pull
// vk_icdGetInstanceProcAddr, and hand it to the loader via the
// VkInstanceCreateInfo pNext chain. No json, no environment variable.
// MVK_DYLIB_PATH is libmoltenvk.dylib_path from libs{MoltenVK} (see
// ./buildfile).
//
typedef PFN_vkVoidFunction (VKAPI_PTR *pfn_vk_icdGetInstanceProcAddr) (VkInstance instance, const char* pName);

int
main ()
{
  void* handle = dlopen (MVK_DYLIB_PATH, RTLD_NOW | RTLD_LOCAL);
  assert (handle != nullptr);

  auto get_proc_addr = (pfn_vk_icdGetInstanceProcAddr) dlsym (handle, "vk_icdGetInstanceProcAddr");
  assert (get_proc_addr != nullptr);

  VkDirectDriverLoadingInfoLUNARG ddli {};
  ddli.sType = VK_STRUCTURE_TYPE_DIRECT_DRIVER_LOADING_INFO_LUNARG;
  ddli.pfnGetInstanceProcAddr = (PFN_vkGetInstanceProcAddrLUNARG) get_proc_addr;

  VkDirectDriverLoadingListLUNARG ddll {};
  ddll.sType = VK_STRUCTURE_TYPE_DIRECT_DRIVER_LOADING_LIST_LUNARG;
  ddll.mode = VK_DIRECT_DRIVER_LOADING_MODE_EXCLUSIVE_LUNARG;
  ddll.driverCount = 1;
  ddll.pDrivers = &ddli;

  const char* exts[] =
  {
    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    VK_LUNARG_DIRECT_DRIVER_LOADING_EXTENSION_NAME,
  };

  VkApplicationInfo app_info {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo ici {};
  ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  ici.pNext = &ddll;
  ici.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  ici.pApplicationInfo = &app_info;
  ici.enabledExtensionCount = 2;
  ici.ppEnabledExtensionNames = exts;

  // Requires ICD interface version 7 (KhronosGroup/MoltenVK#2663): see
  // ../../libmoltenvk/src/MoltenVK/Vulkan/vulkan.mm.patch for the fix to
  // vk_icdNegotiateLoaderICDInterfaceVersion's negotiated version.
  //
  VkInstance instance;
  assert (vkCreateInstance (&ici, nullptr, &instance) == VK_SUCCESS);

  exercise_device (instance);

  vkDestroyInstance (instance, nullptr);

  assert (dlclose (handle) == 0);
}
