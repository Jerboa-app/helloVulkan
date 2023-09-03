#ifndef SHADER
#define SHADER

#include <vulkan/vulkan.h>

#include <fstream>
#include <string>
#include <vector>

namespace Renderer
{
    class Shader
    {

    public:

        Shader()
        : device(VK_NULL_HANDLE)
        {}

        Shader(const VkDevice & d, std::string programName)
        : device(d)
        {
            vertexSource = readSPIRV(programName+"-vert.spv");
            fragmentSource = readSPIRV(programName+"-frag.spv");
        }

        ~Shader();

        std::vector<VkPipelineShaderStageCreateInfo> shaderStage();

    private:

        const VkDevice & device;

        VkShaderModule vertexModule, fragmentModule;

        std::vector<char> vertexSource, fragmentSource;

        std::vector<char> readSPIRV(const std::string & filename);
        void createShaderModules(VkDevice device);
    };
}

#endif /* SHADER */
