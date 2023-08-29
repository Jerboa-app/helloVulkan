#ifndef VULKAN_H
#define VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Renderer/renderer.h>

#include <stdexcept>

namespace Renderer
{

    class VulkanRenderer : public AbstractRenderer
    {

        public:

            VulkanRenderer();

        private:

            VkInstance instance;
    };

}

#endif /* VULKAN_H */
