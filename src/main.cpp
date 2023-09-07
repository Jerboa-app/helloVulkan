#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

#include <Renderer/vulkan.h>

#include <memory>

class HelloTriangleApplication 
{
public:

    void run() 
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    void resize(uint32_t w, uint32_t h) { renderer->setExtent(w, h); }

private:

    GLFWwindow * window;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    std::unique_ptr<Renderer::VulkanRenderer> renderer;

    void initWindow()
    {
        glfwInit();
        // no opengl
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizedCallback);
    }

    void initVulkan() 
    {
        renderer = std::move(std::make_unique<Renderer::VulkanRenderer>(window));
    }

    void mainLoop() 
    {
        while(!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            renderer->drawFrame();
        }

        renderer->finish();
    }

    void cleanup() 
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    static void frameBufferResizedCallback(GLFWwindow * window, int width, int height)
    {
        auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        width = static_cast<uint32_t>(w);
        height = static_cast<uint32_t>(h);
        app->resize(width, height);

    }


};

int main() 
{
    HelloTriangleApplication app;

    try 
    {
        app.run();
    } catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}