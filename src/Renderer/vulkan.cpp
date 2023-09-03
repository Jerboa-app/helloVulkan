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

        pickPhysicalDevice();

        createLogicalDevice();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        vkDestroyDevice(device, nullptr);

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

    void VulkanRenderer::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            throw std::runtime_error("No GPUs with Vulkan support");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        VkPhysicalDeviceProperties deviceProperties;

        for (const auto& physicalDevice : devices)
        {
            vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

            std::cout << "Found physicalDevice: " 
                      << deviceProperties.deviceName << " v"
                      << deviceProperties.driverVersion << "\n";

            if (isSuitableDevice(physicalDevice))
            {
                this->physicalDevice = physicalDevice;
                break;
            }
        }
    }

    bool VulkanRenderer::isSuitableDevice(VkPhysicalDevice physicalDevice)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        return indices.isComplete();
    }

    QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice physicalDevice)
    {
        QueueFamilyIndices indices;

        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(count);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());

        unsigned i = 0;
        for (const auto & queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }
            if (indices.isComplete())
            {
                break;
            }
            i++;
        }

        return indices;
    }

    void VulkanRenderer::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        VkDeviceQueueCreateInfo queueCreateInfo {};

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        // priority between 0 and 1
        float priority = 1.0f;
        queueCreateInfo.pQueuePriorities = &priority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;

        // compat with older vulkan https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Logical_device_and_queues
        createInfo.enabledExtensionCount = 0;
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device");
        }
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