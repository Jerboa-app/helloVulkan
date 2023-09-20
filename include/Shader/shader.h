#ifndef SHADER
#define SHADER

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>

namespace Renderer
{

    class Shader
    {

        /*

            Shaderc macro definitions
                // Like -DMY_DEFINE=1
                options.AddMacroDefinition("MY_DEFINE", "1");

        */

    public:

        Shader()
        : device(VK_NULL_HANDLE)
        {}

        Shader(const VkDevice & d, std::string programName)
        : device(d)
        {
            
            shaderc::CompileOptions options;

            auto preprocessedVert = preprocessShader
            (
                "shader_src", 
                shaderc_glsl_vertex_shader, 
                vert, 
                options
            );
            std::cout << "Compiled a vertex shader resulting in preprocessed text:"
                      << std::endl
                      << preprocessedVert << std::endl;

            vertexSource = compileSPIRV
            (
                "shader_src", 
                shaderc_glsl_vertex_shader, 
                preprocessedVert, 
                options,
                true
            );

            std::cout << "Compiled to a binary module with " << vertexSource.size()
                      << " words." << std::endl;

            auto preprocessedFrag = preprocessShader
            (
                "shader_src", 
                shaderc_glsl_fragment_shader, 
                frag, 
                options
            );
            std::cout << "Compiled a fragment shader resulting in preprocessed text:"
                      << std::endl
                      << preprocessedFrag << std::endl;

            fragmentSource = compileSPIRV
            (
                "shader_src", 
                shaderc_glsl_fragment_shader, 
                preprocessedFrag, 
                options,
                true
            );

            std::cout << "Compiled to a binary module with " << fragmentSource.size()
                      << " words." << std::endl;


            createShaderModules(device);

        }

        ~Shader();

        std::vector<VkPipelineShaderStageCreateInfo> shaderStage();

    private:

        const VkDevice & device;

        VkShaderModule vertexModule, fragmentModule;

        std::vector<uint32_t> vertexSource, fragmentSource;

        std::vector<char> readSPIRV(const std::string & filename);
        void createShaderModules(const VkDevice & device);

        // Returns GLSL shader source text after preprocessing.
        std::string preprocessShader
        (
            const std::string& source_name,
            shaderc_shader_kind kind,
            const std::string& source,
            shaderc::CompileOptions options
        );

        // Compiles a shader to SPIR-V assembly. Returns the assembly text
        // as a string.
        std::string compileToAssembly
        (
            const std::string& source_name,
            shaderc_shader_kind kind,
            const std::string& source,
            shaderc::CompileOptions options,
            bool optimize = false
        );


        // Compiles a shader to a SPIR-V binary. Returns the binary as
        // a vector of 32-bit words.
        std::vector<uint32_t> compileSPIRV
        (
            const std::string& source_name,
            shaderc_shader_kind kind,
            const std::string& source,
            shaderc::CompileOptions options,
            bool optimize = false
        );

        const char * frag = "#version 450\n"
            "layout(location = 0) in vec3 fragColour;\n"
            "layout(location = 0) out vec4 outColour;\n"
            "void main()\n"
            "{\n"
            "    outColour = vec4(fragColour, 1.0);\n"
            "}\n";

        const char * vert = "#version 450\n"
            "layout(binding = 0) uniform UniformBufferObject\n"
            "{\n"
            "    mat4 model;\n"
            "    mat4 view;\n"
            "    mat4 proj;\n"
            "} ubo;\n"
            "layout(location = 0) in vec2 a_position;\n"
            "layout(location = 1) in vec3 a_colour;\n"
            "layout(location = 0) out vec3 fragColour;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(a_position, 0.0, 1.0);\n"
            "    fragColour = a_colour;\n"
            "}";
    };
}

#endif /* SHADER */
