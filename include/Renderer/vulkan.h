#ifndef VULKAN
#define VULKAN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Renderer/renderer.h>
#include <Shader/shader.h>

#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>
#include <optional>
#include <set>
#include <map>
#include <limits>
#include <algorithm>

const int MAX_CONCURRENT_FRAMES = 2;

const std::vector<const char *> validationLayers = 
{
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> deviceExtensions = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef VALIDATION
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

namespace Renderer
{

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanRenderer : public AbstractRenderer
    {

        public:

            VulkanRenderer(GLFWwindow * window);

            ~VulkanRenderer();

            void drawFrame();

            void finish(){ vkDeviceWaitIdle(device); }

        private:

            VkInstance instance;

            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
            VkDevice device;

            VkQueue graphicsQueue, presentQueue;

            VkSurfaceKHR surface;

            VkViewport viewport;
            VkRect2D scissor;

            VkRenderPass renderPass;
            VkPipelineLayout pipelineLayout;
            VkPipeline pipeline;

            VkCommandPool commandPool;
            std::vector<VkCommandBuffer> commandBuffers;

            std::vector<VkSemaphore> imageAvailableSemaphores, renderFinsihedSemaphores;
            std::vector<VkFence> framesFinished;

            unsigned currentFrame = 0;

            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            VkFormat swapChainImageFormat;
            VkExtent2D swapChainExtent;

            std::vector<VkImageView> swapChainImageViews;
            std::vector<VkFramebuffer> swapChainFramebuffers;

            VkDebugUtilsMessengerEXT debugMessenger;

            std::vector<VkLayerProperties> availableLayers;
            std::vector<const char *> extensions;

            void createSurface(GLFWwindow * window);

            void pickPhysicalDevice();
            bool isSuitableDevice(VkPhysicalDevice physicalDevice);
            void createLogicalDevice();

            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);

            bool checkValidationLayerSupport();
            void supportedValidationLayers(bool print = false);

            bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
            void getRequiredExtensions();

            void createSwapChain(GLFWwindow * window);
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
            VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats);
            VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes);
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities, GLFWwindow * window);
            void createImageViews();

            void createRenderPass();

            void createGraphicsPipeline();

            void createFramebuffers();

            void createCommandPool();
            void createCommandBuffers();
            void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

            void createSyncObjects();

            void setupDebugMessenger();

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

            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo);
    };

}

#endif /* VULKAN */
