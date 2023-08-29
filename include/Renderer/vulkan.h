#ifndef VULKAN_H
#define VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Renderer/renderer.h>

#include <stdexcept>
#include <vector>
#include <string>

#include <iostream>

const std::vector<std::string> validationLayers = 
{
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
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

            std::vector<VkLayerProperties> availableLayers;

            bool checkValidationLayerSupport();
            void supportedValidationLayers();
    };

}

#endif /* VULKAN_H */
