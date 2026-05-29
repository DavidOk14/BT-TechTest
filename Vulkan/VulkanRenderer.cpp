#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer(SDL_Window* window) : refGameWindow(window)
{
    // Create Vulkan Information
    createInstance();
    // Start Debugging Messenger for Validation Layer Calls
    setupDebugMessenger();
    // Create Window Surface
    createSurface();
    // Select Physical Hardware Device To Use
    pickPhysicalDevice();
    createLogicalDevice();

    // Print The Selected Device & Score
    std::cout << "Selected Physical Device Score: " << rateDeviceSuitability(_physicalDevice) << std::endl;
    std::cout << "Selected Physical Device Name: " << deviceProperties.deviceName << ", Type: " << 
        deviceProperties.deviceType << ", ID: " << deviceProperties.deviceID << std::endl;
}

VulkanRenderer::~VulkanRenderer()
{
    // Upon closure of Vulkan Renderer/Application Run Cleanup/Destroy Functions
    Cleanup();
}

// Callback for Vulkan Validation Layer Messages
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "Vulkan Validation Layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkInstance VulkanRenderer::createInstance()
{
    // Vulkan Application Info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = SDL_GetWindowTitle(refGameWindow);
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // Vulkan Instance Create Info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Fetch SDL/Vulkan Instance Extensions
    Uint32 count_instance_extensions;
    const char* const* instance_extensions = SDL_Vulkan_GetInstanceExtensions(&count_instance_extensions);

    // Error Handle Vulkan Instance Extensions
    if (instance_extensions == NULL) 
    {
        std::cerr << "Error: Failed to get Vulkan instance extensions from SDL3! SDL Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        // List Found Vulkan Extensions
        std::cout << "Vulkan Extensions:" << std::endl;
        for (uint32_t i = 0; i < count_instance_extensions; ++i)
        {
            std::cout << "\t" << instance_extensions[i] << std::endl;
        }
        std::cout << std::endl;
    }

    // Vector of Vulkan/SDL Extensions
    std::vector<const char*> checkValidExtensions(instance_extensions, instance_extensions + count_instance_extensions);

    // Validation Layers for debugging
    if (enableValidationLayers)
    {
        checkValidExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        if (!supportedValidationLayers(checkValidExtensions, validationLayers))
            throw std::runtime_error("Validation Layers have been requested however none were available.");

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        // If Debugging Mode is disabled do not enable Vulkan Validation Layers
        createInfo.enabledLayerCount = NULL;
        createInfo.ppEnabledLayerNames = NULL;
    }

    // Add Extensions to Vulkan Instance Info
    createInfo.enabledExtensionCount = static_cast<uint32_t>(checkValidExtensions.size());
    createInfo.ppEnabledExtensionNames = checkValidExtensions.data();

    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);

    // Check for errors with instance creation
    if (result != VK_SUCCESS) 
        std::cerr << "Failed to create Vulkan instance!" << std::endl;

    // Notify to Console Vulkan was properly initialized
    std::cout << "Vulkan Initialized Successfully!\n" << std::endl;

    return instance;
}

bool VulkanRenderer::supportedValidationLayers(std::vector<const char*> extensions, std::vector<const char*>& layers)
{
    // Get The Number Of Supported Extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    // Retrieve Extension Properties
    std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

    // Print The Supported Extensions
    std::cout << "The device supports the following extensions:\n";
    for (VkExtensionProperties supportedExtension : supportedExtensions)
    {
        std::cout << "\t" << supportedExtension.extensionName << std::endl;
    }
    std::cout << std::endl;

    // Find All Supported Extensions
    bool found = false;
    for (const char* extension : extensions)
    {
        found = false;
        for (VkExtensionProperties supportedExtension : supportedExtensions)
        {
            if (strcmp(extension, supportedExtension.extensionName) == 0)
            {
                found = true;
                std::cout << "Extension: " << extension << " is supported!" << std::endl;
            }
        }
        if (!found)
        {
            std::cout << "Extension: " << extension << " is not supported." << std::endl;
            return false;
        }
    }
    std::cout << std::endl;

    // Get The Number Of Supported Layers
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    // Retrieve Layer Properties
    std::vector<VkLayerProperties> supportedLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, supportedLayers.data());

    // Print The Supported Layers
    std::cout << "The device supports the following layers:\n";
    for (VkLayerProperties supportedLayer : supportedLayers)
    {
        std::cout << "\t" << supportedLayer.layerName << std::endl;
    }
    std::cout << std::endl;

    // Find All Supported Layers
    found = false;
    for (const char* layer : layers)
    {
        found = false;
        for (VkLayerProperties supportedLayer : supportedLayers)
        {
            if (strcmp(layer, supportedLayer.layerName) == 0)
            {
                found = true;
                std::cout << "Layer: " << layer << " is supported!" << std::endl;
            }
        }
        if (!found)
        {
            std::cout << "Layer: " << layer << " is not supported." << std::endl;
            return false;
        }
    }

    std::cout << std::endl;
    return true;
}

// Setup Vulkan Validation Layer Debug Messenger for Console Printing
void VulkanRenderer::setupDebugMessenger()
{
    // If enableValidationLayers is disabled (application is running in release mode) do not execute function
    if (!enableValidationLayers) 
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    // Check for success of DebugUtilMessenger creation (throw runtime error on failure)
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("Failed to set up debug messenger!");
}

// Pick which Graphical Based Processor to use
void VulkanRenderer::pickPhysicalDevice()
{
    // Retrieve number of Physical Devices
    unsigned int deviceCount = 0;
    int score = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // If no Physical Devices are found that have Vulkan Support throw runtime error!
    if (deviceCount == 0)
        throw std::runtime_error("Failed to retrieve any GPUs with Vulkan Support");

    // Set list of Physical Devices to a Vector
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, VkPhysicalDevice> candidates;

    // Review score of each Physical Device in list Devices then store in unordered map
    for (const auto& device : devices) 
    {
        score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    // Set Physical Device to the highest scoring device
    _physicalDevice = candidates.rbegin()->second;

    // If a suitable Physical Device was not found
    if ((_physicalDevice == VK_NULL_HANDLE) || (!isDeviceSuitable(_physicalDevice)))
        throw std::runtime_error("Failed to retrieve a suitable Physical Device with Vulkan Support");
}

// Compare device with queueFamilies to test suitability
bool VulkanRenderer::isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    return indices.isCompleted();
}

// Function to find Queue Families
QueueFamilyIndices VulkanRenderer::FindQueueFamilies(VkPhysicalDevice device)
{
    // Initialization of return value
    QueueFamilyIndices indices;

    // Retrieve number of Queue Families/Data for Vector
    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::cout << "Vulkan Queue Family Count: " << queueFamilyCount << std::endl;

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // Assign i to value of Queue Families with Desired Flags
    int i = 0;

    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

        if (presentSupport)
            indices.presentFamily = i;

        if (indices.isCompleted())
            break;

        i++;
    }

    return indices;
}

// Function to rate the current Physical Device
int VulkanRenderer::rateDeviceSuitability(VkPhysicalDevice device) 
{
    // Iniitalize score value and fetch Device Properties/Features
    int score = 0;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
        score += 1000;

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader)
        return 0;

    // Return value of device score
    return score;
}

// Create Logical Device Utilizing Queue Family Data
void VulkanRenderer::createLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Vulkan failed to create a logical device.");
    }

    vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
}

// Store Neccessary Info For DebugMessenger
void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
{
    // Setup Info for the DebugUtilMessenger to use for validation layer printing to console
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

// Function Used To Fetch Proper Vulkan Variables for DebugUtilsMessenger Initialization
VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanRenderer::createSurface()
{
    // Check for issues while creating Window Surface
    if (!SDL_Vulkan_CreateSurface(refGameWindow, instance, nullptr, &_surface))
    {
        throw std::runtime_error("Could not create Vulkan surface! SDL error: " + std::string(SDL_GetError()));
        std::cout << std::endl;;
    }
    else
        std::cout << "Successfuly created SDL Surface for Vulkan!\n" << std::endl;
}

// Free DebugUtilsMessenger From Memory
void VulkanRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

// Called In Destructor To Free Memory While Vulkan Is Not In Use
void VulkanRenderer::Cleanup()
{
    if(_device)
        vkDestroyDevice(_device, nullptr);

    std::cout << "Destroying Vulkan Device..." << std::endl;

    if (_surface)
        vkDestroySurfaceKHR(instance, _surface, nullptr);

    std::cout << "Destroying Vulkan Surface..." << std::endl;

    if (enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    std::cout << "Destroying Vulkan Debug Messenger..." << std::endl;

    if(instance)
        vkDestroyInstance(instance, nullptr);

    std::cout << "Destroying Vulkan Instance..." << std::endl;
}