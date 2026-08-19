#include "../shared.hpp"

// .app bundle mode: the loader discovers MoltenVK via
// Contents/Resources/vulkan/icd.d/ next to this executable, on its own,
// as soon as this binary runs from inside a .app bundle structure. No
// json import, no dlopen, no environment variable, no code at all beyond
// the ordinary portability-enumeration flag every ICD consumer needs.
//
int
main ()
{
  const char* exts[] = {VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME};

  VkApplicationInfo app_info {};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo ici {};
  ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  ici.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  ici.pApplicationInfo = &app_info;
  ici.enabledExtensionCount = 1;
  ici.ppEnabledExtensionNames = exts;

  VkInstance instance;
  assert (vkCreateInstance (&ici, nullptr, &instance) == VK_SUCCESS);

  exercise_device (instance);

  vkDestroyInstance (instance, nullptr);
}
