#include <Renderer/vulkan.h>

namespace Renderer
{

    VulkanRenderer::VulkanRenderer()
    {
        // optional application information
        // can be used for optimisation  by driver 
        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello VK Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No engine";
        appInfo.engineVersion = VK_API_VERSION_1_0;
        appInfo.pNext = nullptr;
        
        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.pNext = nullptr;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        getRequiredExtensions();

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }

        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers are not supported");
        }

        setupDebugMessenger();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        if (enableValidationLayers)
        {
            destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroyInstance(instance, nullptr);
    }

    bool VulkanRenderer::checkValidationLayerSupport()
    {
        supportedValidationLayers();

        for (const char * layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto & layerProperties : availableLayers)
            {
                if (std::string(layerName) == std::string(layerProperties.layerName))
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void VulkanRenderer::getRequiredExtensions()
    {
        // glfw 
        uint32_t glfwExtensionCount = 0;
        const char ** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        extensions = std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
    }

    void VulkanRenderer::supportedValidationLayers(bool print)
    {
        uint32_t layerCount;
        // get all the validation layers allowed
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        availableLayers = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        if (print)
        {
            for (const auto & layerProperties : availableLayers)
            {
                std::cout << layerProperties.layerName << "\n";
            }
        }
    }

    void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo) {
        createInfo = {};
        createInfo.sType =              VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback =    debugCallback;
        createInfo.pUserData =          nullptr; // Optional
        createInfo.pNext =              nullptr;
        createInfo.flags =              0;
    }

    void VulkanRenderer::setupDebugMessenger()
    {
        if (!enableValidationLayers) { return; }

        VkDebugUtilsMessengerCreateInfoEXT createInfo;

        populateDebugMessengerCreateInfo(createInfo);

        if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void destroyDebugMessenger();

}