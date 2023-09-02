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
        
        VkInstanceCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // glfw 
        uint32_t glfwExtensionCount = 0;
        const char ** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }

        if (enableValidationLayers)
        {
            checkValidationLayerSupport();
        }
    }

    VulkanRenderer::~VulkanRenderer()
    {
        vkDestroyInstance(instance, nullptr);
    }

    bool VulkanRenderer::checkValidationLayerSupport()
    {
        
        supportedValidationLayers();

        for (std::string layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto & layerProperties : availableLayers)
            {
                if (layerName == std::string(layerProperties.layerName))
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

    void VulkanRenderer::supportedValidationLayers()
    {
        uint32_t layerCount;
        // get all the validation layers allowed
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        availableLayers = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const auto & layerProperties : availableLayers)
        {
            std::cout << layerProperties.layerName << "\n";
        }
    }

}