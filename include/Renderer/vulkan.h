#ifndef VULKAN
#define VULKAN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
#include <array>
#include <cstring>

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

struct Vertex 
{
    glm::vec2 pos;
    glm::vec3 colour;

    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        // can also be VK_VERTEX_INPUT_RATE_INSTANCE to move each instance
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getArrtibuteDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        // vertex binding, 0
        attributeDescriptions[0].binding = 0;
        // layout(location = 0)
        attributeDescriptions[0].location = 0;
        // we have a vec2, so R32, G32, as single float
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        // auto calulate offset via macro
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, colour);

        return attributeDescriptions;
    }
};


const std::vector<Vertex> vertices = 
{
    {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

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

            void setExtent(uint32_t w, uint32_t h) { width = w; height = h; recreateSwapChain(); }

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

            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;

            VkCommandPool commandPool;
            std::vector<VkCommandBuffer> commandBuffers;

            std::vector<VkSemaphore> imageAvailableSemaphores, renderFinsihedSemaphores;
            std::vector<VkFence> framesFinished;

            unsigned currentFrame = 0;

            VkSwapchainKHR swapChain;
            std::vector<VkImage> swapChainImages;
            VkFormat swapChainImageFormat;
            VkExtent2D swapChainExtent;

            uint32_t width, height;

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

            void createSwapChain();
            void recreateSwapChain();
            void cleanupSwapChain();
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
            VkSurfaceFormatKHR chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats);
            VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes);
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities);
            void createImageViews();

            void createRenderPass();

            void createGraphicsPipeline();

            void createFramebuffers();

            void createVertexBuffer();

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

            uint32_t findMemoryType(uint32_t typeFiller, VkMemoryPropertyFlags properties);

            void createBuffer
            (
                VkDeviceSize size, 
                VkBufferUsageFlags usage, 
                VkMemoryPropertyFlags properties,
                VkBuffer & buffer, VkDeviceMemory & bufferMemory
            );

            void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
    };

}

#endif /* VULKAN */
