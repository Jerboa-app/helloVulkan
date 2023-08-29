#ifndef VULKAN_H
#define VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Renderer/renderer.h>

#include <stdexcept>
#include <vector>
#include <string>

#include <iostream>

const std::vector<const char *> validationLayers = 
{
    "VK_LAYER_KHRONOS_validation"
};

#ifdef VALIDATION
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

namespace Renderer
{

    class VulkanRenderer : public AbstractRenderer
    {

        public:

            VulkanRenderer();

            ~VulkanRenderer();

        private:

            VkInstance instance;

            VkDebugUtilsMessengerEXT debugMessenger;

            std::vector<VkLayerProperties> availableLayers;
            std::vector<const char *> extensions;

            bool checkValidationLayerSupport();
            void supportedValidationLayers(bool print = false);

            void getRequiredExtensions();

            void setupDebugMessenger();
            void destroyDebugMessenger();

            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
            (
                VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT              messageTypes,
                const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
                void                                       * pUserData
            )
            {
                /*
                    messageSeverity can be

                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:    Diagnostic message
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:       Informational message like the creation of a resource
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:    Message about behavior that is not necessarily an error, but very likely a bug in your application
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:      Message about behavior that is invalid and may cause crashes

                    messageType can be

                        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:        Some event has happened that is unrelated to the specification or performance
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:     Something has happened that violates the specification or indicates a possible mistake
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:    Potential non-optimal use of Vulkan

                */
                std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

                // abort triggering call
                return VK_FALSE;
            }

            // proxy to create a debug messenger

            VkResult createDebugUtilsMessengerEXT
            (
                VkInstance instance, 
                const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                const VkAllocationCallbacks* pAllocator, 
                VkDebugUtilsMessengerEXT* pDebugMessenger
            )
            {
                auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
                if (func != nullptr) {
                    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
                } else {
                    return VK_ERROR_EXTENSION_NOT_PRESENT;
                }
            }

            // proxy to destroy a debug messenger

            static void destroyDebugUtilsMessengerEXT
            (
                VkInstance instance, 
                VkDebugUtilsMessengerEXT debugMessenger, 
                const VkAllocationCallbacks* pAllocator
            ) 
            {
                auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
                if (func != nullptr) {
                    func(instance, debugMessenger, pAllocator);
                }
            }
    };

}

#endif /* VULKAN_H */
