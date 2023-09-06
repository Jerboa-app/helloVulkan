#include <Shader/shader.h>

namespace Renderer
{

    Shader::~Shader()
    {
        vkDestroyShaderModule(device, fragmentModule, nullptr);
        vkDestroyShaderModule(device, vertexModule, nullptr);
    }

    std::vector<char> Shader::readSPIRV(const std::string & filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open SPIR-V file at "+filename);
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    void Shader::createShaderModules(const VkDevice & device)
    {
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = vertexSource.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(vertexSource.data());

        if (vkCreateShaderModule(device, &createInfo, nullptr, &vertexModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create vertex shader module");
        }

        createInfo.codeSize = fragmentSource.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(fragmentSource.data());

        if (vkCreateShaderModule(device, &createInfo, nullptr, &fragmentModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create fragment shader module");
        }
    }

    std::vector<VkPipelineShaderStageCreateInfo> Shader::shaderStage()
    {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertexModule;
        // the entry point
        vertShaderStageInfo.pName = "main";
        // can set constant on-the-fly
        // vertShaderStageInfo.pSpecializationInfo

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragmentModule;
        // the entry point
        fragShaderStageInfo.pName = "main";

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = 
        {
            vertShaderStageInfo,
            fragShaderStageInfo
        };

        return shaderStages;
    }
}