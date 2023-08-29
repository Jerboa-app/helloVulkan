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

            std::vector<VkLayerProperties> availableLayers;

            bool checkValidationLayerSupport();
            void supportedValidationLayers(bool print = false);
    };

}

#endif /* VULKAN_H */
