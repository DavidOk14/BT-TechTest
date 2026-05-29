#pragma once

#ifdef _DEBUG  // Enable validation layers in debug mode only
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

// C++ Includes
#include <vector>
#include <iostream>
#include <optional>
#include <map>
#include <set>

// SDL3/Vulkan Inlcudes
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isCompleted()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanRenderer
{
public:
    VulkanRenderer(SDL_Window* window); // Constructor With Window Parameter To Wrap Render To It
    ~VulkanRenderer();  // Destructor For Closing The Renderer

    VkInstance createInstance(); // Vulkan instance creation

    VkDevice getDevice() { return _device; }
    VkSurfaceKHR getWindowSurface() { return windowSurface; }
    VkSurfaceKHR getSurface() { return _surface; }
    VkQueue getGraphicsQueue() { return _graphicsQueue; }
    VkQueue getPresentQueue() { return _presentQueue; }

private:
    void Cleanup(); // Cleanup Vulkan resources

    int w, h;

    SDL_Window* refGameWindow; // Window object to be used as reference for main game

    std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

    // Instance Creation Variables
    VkInstance instance = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain;
    VkRenderPass renderPass;
    VkPipeline pipeline;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkFramebuffer framebuffer;

    // Debug Mode Variables    
    std::vector<const char*> layers;
    bool supportedValidationLayers(std::vector<const char*> extensions, std::vector<const char*>& layers);
    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void createSurface();
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    VkDebugUtilsMessengerEXT debugMessenger = NULL;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    // Device Variables
    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    int rateDeviceSuitability(VkPhysicalDevice device);
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    void createLogicalDevice();
    VkDevice _device = VK_NULL_HANDLE;
    VkQueue _graphicsQueue = VK_NULL_HANDLE;

    // Window Surface Variables
    VkSurfaceKHR windowSurface = VK_NULL_HANDLE;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkQueue _presentQueue = VK_NULL_HANDLE;

};