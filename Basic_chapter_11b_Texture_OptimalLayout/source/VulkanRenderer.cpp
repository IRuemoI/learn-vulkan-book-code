#include "VulkanRenderer.h"
#include "MeshData.h"
#include "VulkanApplication.h"
#include "Wrappers.h"

VulkanRenderer::VulkanRenderer(VulkanApplication *app, VulkanDevice *deviceObject) {
    // 注意需要使用0来填充内存
    memset(&Depth, 0, sizeof(Depth));
    memset(&connection, 0, sizeof(HINSTANCE));// hInstance - Windows Instance

    application = app;
    deviceObj = deviceObject;

    // 创建交换链
    swapChainObj = new VulkanSwapChain(this);
    // 创建绘制对象
    VulkanDrawable *drawableObj = new VulkanDrawable(this);
    // 添加绘制对象
    drawableList.push_back(drawableObj);
}

VulkanRenderer::~VulkanRenderer() {
    delete swapChainObj;
    swapChainObj = nullptr;
    for (auto d: drawableList) {
        delete d;
    }
    drawableList.clear();
}

void VulkanRenderer::initialize() {

    // 我们需要用到指令缓存，因此创建指令缓冲池
    createCommandPool();

    // 创建交换链的颜色图和深度图
    buildSwapChainAndDepthImage();

    // 构建顶点缓冲
    createVertexBuffer();

    const bool includeDepth = true;
    // 创建渲染通道
    createRenderPass(includeDepth);

    // 使用渲染通道创建帧缓冲
    createFrameBuffer(includeDepth);

    // 创建顶点和片元着色器
    createShaders();

    const char *filename = "../LearningVulkan.ktx";
    bool renderOptimalTexture = true;
    if (renderOptimalTexture) {
        createTextureOptimal(filename, &texture, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT);
    } else {
        createTextureLinear(filename, &texture, VK_IMAGE_USAGE_SAMPLED_BIT);
    }
    // 在可绘制对象中设置创建的纹理
    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->setTextures(&texture);
    }

    // 创建描述符集布局
    createDescriptors();

    // 创建流水线状态管理对象
    createPipelineStateManagement();
}

void VulkanRenderer::prepare() {
    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->prepare();
    }
}

void VulkanRenderer::update() {
    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->update();
    }
}

bool VulkanRenderer::render() {
    MSG msg;// message
    PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
    if (msg.message == WM_QUIT) {
        return false;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    RedrawWindow(window, nullptr, nullptr, RDW_INTERNALPAINT);
    return true;
}

#ifdef _WIN32

// MS-Windows的窗口回调函数
LRESULT CALLBACK VulkanRenderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    VulkanApplication *appObj = VulkanApplication::GetInstance();
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_PAINT:
            for (VulkanDrawable *drawableObj: appObj->rendererObj->drawableList) {
                drawableObj->render();
            }

            return 0;

        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED) {
                appObj->rendererObj->width = lParam & 0xffff;
                appObj->rendererObj->height = (lParam & 0xffff0000) >> 16;
                appObj->rendererObj->getSwapChain()->setSwapChainExtent(appObj->rendererObj->width, appObj->rendererObj->height);
                appObj->resize();
            }
            break;

        default:
            break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

void VulkanRenderer::createPresentationWindow(const int &windowWidth, const int &windowHeight) {
#ifdef _WIN32
    width = windowWidth;
    height = windowHeight;
    assert(width > 0 || height > 0);

    WNDCLASSEX winInfo;

    sprintf(name, "Texture demo - Optimal Layout");
    memset(&winInfo, 0, sizeof(WNDCLASSEX));
    // Initialize the window class structure:
    winInfo.cbSize = sizeof(WNDCLASSEX);
    winInfo.style = CS_HREDRAW | CS_VREDRAW;
    winInfo.lpfnWndProc = WndProc;
    winInfo.cbClsExtra = 0;
    winInfo.cbWndExtra = 0;
    winInfo.hInstance = connection;// hInstance
    winInfo.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    winInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
    winInfo.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    winInfo.lpszMenuName = nullptr;
    winInfo.lpszClassName = name;
    winInfo.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);

    // Register window class:
    if (!RegisterClassEx(&winInfo)) {
        // It didn't work, so try to give a useful error:
        printf("试图启动应用程序时出现意外错误!\n");
        fflush(stdout);
        exit(1);
    }

    // Create window with the registered class:
    RECT wr = {0, 0, width, height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    window = CreateWindowEx(0,
                            name,// class name
                            name,// app name
                            WS_OVERLAPPEDWINDOW |// window style
                                    WS_VISIBLE |
                                    WS_SYSMENU,
                            100, 100,// x/y coords
                            wr.right - wr.left,// width
                            wr.bottom - wr.top,// height
                            nullptr,// handle to parent
                            nullptr,// handle to menu
                            connection,// hInstance
                            nullptr);// no extra parameters

    if (!window) {
        // It didn't work, so try to give a useful error:
        printf("无法创建要在其中绘图的窗口!\n");
        fflush(stdout);
        exit(1);
    }

    SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR) &application);
#else
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    connection = xcb_connect(nullptr, &scr);
    if (connection == nullptr) {
        std::cout << "Cannot find a compatible Vulkan ICD.\n";
        exit(-1);
    }

    setup = xcb_get_setup(connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);

    screen = iter.data;
#endif// _WIN32
}

void VulkanRenderer::destroyPresentationWindow() {
    DestroyWindow(window);
}
#else
void VulkanRenderer::createPresentationWindow() {
    assert(width > 0);
    assert(height > 0);

    uint32_t value_mask, value_list[32];

    window = xcb_generate_id(connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, width, height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, value_mask, value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
    reply = xcb_intern_atom_reply(connection, cookie2, 0);

    xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1, &(*reply).atom);
    free(reply);

    xcb_map_window(connection, window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive runs
    const uint32_t coords[] = {100, 100};
    xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(connection);

    xcb_generic_event_t *e;
    while ((e = xcb_wait_for_event(connection))) {
        if ((e->response_type & ~0x80) == XCB_EXPOSE)
            break;
    }
}

void VulkanRenderer::destroyWindow() {
    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);
}

#endif// _WIN32

void VulkanRenderer::createCommandPool() {
    VulkanDevice *deviceObj = application->deviceObj;
    /* 依赖于initializeSwapChainExtension() */
    VkResult res;

    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = nullptr;
    cmdPoolInfo.queueFamilyIndex = deviceObj->graphicsQueueWithPresentIndex;
    cmdPoolInfo.flags = 0;

    res = vkCreateCommandPool(deviceObj->device, &cmdPoolInfo, nullptr, &cmdPool);
    assert(res == VK_SUCCESS);
}

void VulkanRenderer::createDepthImage() {
    VkResult result;
    bool pass;

    VkImageCreateInfo imageInfo = {};

    // 如果深度格式未定义，那么使用16位深度值
    if (Depth.format == VK_FORMAT_UNDEFINED) {
        Depth.format = VK_FORMAT_D16_UNORM;
    }

    const VkFormat depthFormat = Depth.format;

    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = nullptr;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = depthFormat;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = NUM_SAMPLES;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.queueFamilyIndexCount = 0;
    imageInfo.pQueueFamilyIndices = nullptr;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.flags = 0;

    // 用户创建图像信息和图像对象
    result = vkCreateImage(deviceObj->device, &imageInfo, nullptr, &Depth.image);
    assert(result == VK_SUCCESS);

    // 获取图像内存需求
    VkMemoryRequirements memRqrmnt;
    vkGetImageMemoryRequirements(deviceObj->device, Depth.image, &memRqrmnt);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = 0;
    memAlloc.memoryTypeIndex = 0;
    memAlloc.allocationSize = memRqrmnt.size;
    // 借助内存属性确定所需的内存类型
    pass = deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, 0, /* No requirements */ &memAlloc.memoryTypeIndex);
    assert(pass);

    // 为深度图像对象分配内存
    result = vkAllocateMemory(deviceObj->device, &memAlloc, nullptr, &Depth.mem);
    assert(result == VK_SUCCESS);

    // 绑定刚刚分配的内存
    result = vkBindImageMemory(deviceObj->device, Depth.image, Depth.mem, 0);
    assert(result == VK_SUCCESS);


    VkImageViewCreateInfo imgViewInfo = {};
    imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewInfo.pNext = nullptr;
    imgViewInfo.image = VK_NULL_HANDLE;
    imgViewInfo.format = depthFormat;
    imgViewInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY};
    imgViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imgViewInfo.subresourceRange.baseMipLevel = 0;
    imgViewInfo.subresourceRange.levelCount = 1;
    imgViewInfo.subresourceRange.baseArrayLayer = 0;
    imgViewInfo.subresourceRange.layerCount = 1;
    imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewInfo.flags = 0;

    if (depthFormat == VK_FORMAT_D16_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
        depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        imgViewInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    // 使用指令缓存来创建深度图，包含指令缓存的分配、记录指令的起始和结束范围、指令提交行为这些步骤
    CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdDepthImage);
    CommandBufferMgr::beginCommandBuffer(cmdDepthImage);
    {
        VkImageSubresourceRange subresourceRange = {};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.layerCount = 1;

        // 设置图像布局为：深度蒙版优化
        convertImageLayout(Depth.image,
                       imgViewInfo.subresourceRange.aspectMask,
                       VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange, cmdDepthImage);
    }
    CommandBufferMgr::endCommandBuffer(cmdDepthImage);
    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdDepthImage);

    // 创建图像视图并允许应用程序使用图像
    imgViewInfo.image = Depth.image;
    result = vkCreateImageView(deviceObj->device, &imgViewInfo, nullptr, &Depth.view);
    assert(result == VK_SUCCESS);
}

void VulkanRenderer::createTextureOptimal(const char *filename, TextureData *texture, VkImageUsageFlags imageUsageFlags, VkFormat format) {
    // 载入图像
    gli::texture2D image2D(gli::load(filename));
    assert(!image2D.empty());

    // 获取图像的大小
    texture->textureWidth = uint32_t(image2D[0].dimensions().x);
    texture->textureHeight = uint32_t(image2D[0].dimensions().y);

    // 设置mip-map登记
    texture->mipMapLevels = uint32_t(image2D.levels());

    // Create a staging buffer resource states using.
    // Indicate it be the source of the transfer command.
    // .usage	= VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = image2D.size();
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult error;
    // Create a buffer resource (host-visible) -
    VkBuffer buffer;
    error = vkCreateBuffer(deviceObj->device, &bufferCreateInfo, nullptr, &buffer);
    assert(!error);

    // 获取暂存缓冲的缓冲区内存需求
    VkMemoryRequirements memRqrmnt;
    VkDeviceMemory devMemory;
    vkGetBufferMemoryRequirements(deviceObj->device, buffer, &memRqrmnt);

    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = nullptr;
    memAllocInfo.allocationSize = 0;
    memAllocInfo.memoryTypeIndex = 0;
    memAllocInfo.allocationSize = memRqrmnt.size;

    // Determine the type of memory required for the host-visible buffer  -
    deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memAllocInfo.memoryTypeIndex);

    // Allocate the memory for host-visible buffer objects -
    error = vkAllocateMemory(deviceObj->device, &memAllocInfo, nullptr, &devMemory);
    assert(!error);

    // Bind the host-visible buffer with allocated device memory -
    error = vkBindBufferMemory(deviceObj->device, buffer, devMemory, 0);
    assert(!error);

    // Populate the raw image data into the device memory -
    uint8_t *data;
    error = vkMapMemory(deviceObj->device, devMemory, 0, memRqrmnt.size, 0, (void **) &data);
    assert(!error);

    memcpy(data, image2D.data(), image2D.size());
    vkUnmapMemory(deviceObj->device, devMemory);

    // Create image info with optimal tiling support (.tiling = VK_IMAGE_TILING_OPTIMAL) -
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.mipLevels = texture->mipMapLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.extent = {texture->textureWidth, texture->textureHeight, 1};
    imageCreateInfo.usage = imageUsageFlags;

    // Set image object with VK_IMAGE_USAGE_TRANSFER_DST_BIT if
    // not set already. This allows to copy the source VkBuffer
    // object (with VK_IMAGE_USAGE_TRANSFER_DST_BIT) contents
    // into this image object memory(destination).
    if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
        imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    error = vkCreateImage(deviceObj->device, &imageCreateInfo, nullptr, &texture->image);
    assert(!error);

    // 获取图像内存需求
    vkGetImageMemoryRequirements(deviceObj->device, texture->image, &memRqrmnt);

    // Set the allocation size equal to the buffer allocation
    memAllocInfo.allocationSize = memRqrmnt.size;

    // 借助内存属性确定所需的内存类型
    deviceObj->memoryTypeFromProperties(memRqrmnt.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

    // Allocate the physical memory on the GPU
    error = vkAllocateMemory(deviceObj->device, &memAllocInfo, nullptr, &texture->mem);
    assert(!error);

    // Bound the physical memory with the created image object
    error = vkBindImageMemory(deviceObj->device, texture->image, texture->mem, 0);
    assert(!error);

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = texture->mipMapLevels;
    subresourceRange.layerCount = 1;

    // Use a separate command buffer for texture loading
    // Start command buffer recording
    CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdTexture);
    CommandBufferMgr::beginCommandBuffer(cmdTexture);

    // set the image layout to be
    // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    // since it is destination for copying buffer
    // into image using vkCmdCopyBufferToImage -
    convertImageLayout(texture->image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange, cmdTexture);

    // List contains the buffer image copy formipLevel -
    std::vector<VkBufferImageCopy> bufferImgCopyList;


    uint32_t bufferOffset = 0;
    // Iterater through each mip level and set buffer image copy -
    for (uint32_t i = 0; i < texture->mipMapLevels; i++) {
        VkBufferImageCopy bufImgCopyItem = {};
        bufImgCopyItem.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bufImgCopyItem.imageSubresource.mipLevel = i;
        bufImgCopyItem.imageSubresource.layerCount = 1;
        bufImgCopyItem.imageSubresource.baseArrayLayer = 0;
        bufImgCopyItem.imageExtent.width = uint32_t(image2D[i].dimensions().x);
        bufImgCopyItem.imageExtent.height = uint32_t(image2D[i].dimensions().y);
        bufImgCopyItem.imageExtent.depth = 1;
        bufImgCopyItem.bufferOffset = bufferOffset;

        bufferImgCopyList.push_back(bufImgCopyItem);

        // adjust buffer offset
        bufferOffset += uint32_t(image2D[i].size());
    }

    // Copy the staging buffer memory data contain
    // the stage raw data(with mip levels) into image object
    vkCmdCopyBufferToImage(cmdTexture, buffer, texture->image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           uint32_t(bufferImgCopyList.size()), bufferImgCopyList.data());

    // Advised to change the image layout to shader read
    // after staged buffer copied into image memory -
    texture->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    convertImageLayout(texture->image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   texture->imageLayout, subresourceRange, cmdTexture);


    // Submit command buffer containing copy and image layout commands-
    CommandBufferMgr::endCommandBuffer(cmdTexture);

    // Create a fence object to ensure that the command buffer is executed,
    // coping our staged raw data from the buffers to image memory with
    // respective image layout and attributes into consideration -
    VkFence fence;
    VkFenceCreateInfo fenceCI = {};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCI.flags = 0;

    error = vkCreateFence(deviceObj->device, &fenceCI, nullptr, &fence);
    assert(!error);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdTexture;

    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdTexture, &submitInfo, fence);

    error = vkWaitForFences(deviceObj->device, 1, &fence, VK_TRUE, 10000000000);
    assert(!error);

    vkDestroyFence(deviceObj->device, fence, nullptr);

    // 销毁分配的资源
    vkFreeMemory(deviceObj->device, devMemory, nullptr);
    vkDestroyBuffer(deviceObj->device, buffer, nullptr);

    ///////////////////////////////////////////////////////////////////////////////////////

    // Create sampler
    VkSamplerCreateInfo samplerCI = {};
    samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCI.pNext = nullptr;
    samplerCI.magFilter = VK_FILTER_LINEAR;
    samplerCI.minFilter = VK_FILTER_LINEAR;
    samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.mipLodBias = 0.0f;
    if (deviceObj->deviceFeatures.samplerAnisotropy == VK_TRUE) {
        samplerCI.anisotropyEnable = VK_TRUE;
        samplerCI.maxAnisotropy = 8;
    } else {
        samplerCI.anisotropyEnable = VK_FALSE;
        samplerCI.maxAnisotropy = 1;
    }
    samplerCI.compareOp = VK_COMPARE_OP_NEVER;
    samplerCI.minLod = 0.0f;
    samplerCI.maxLod = (float) texture->mipMapLevels;
    samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    samplerCI.unnormalizedCoordinates = VK_FALSE;

    error = vkCreateSampler(deviceObj->device, &samplerCI, nullptr, &texture->sampler);
    assert(!error);

    // 创建图像视图，允许着色器访问纹理信息
    VkImageViewCreateInfo viewCI = {};
    viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCI.pNext = nullptr;
    viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCI.format = format;
    viewCI.components.r = VK_COMPONENT_SWIZZLE_R;
    viewCI.components.g = VK_COMPONENT_SWIZZLE_G;
    viewCI.components.b = VK_COMPONENT_SWIZZLE_B;
    viewCI.components.a = VK_COMPONENT_SWIZZLE_A;
    viewCI.subresourceRange = subresourceRange;
    viewCI.subresourceRange.levelCount = texture->mipMapLevels;// Optimal tiling supports mip map levels very well set it.
    viewCI.image = texture->image;

    error = vkCreateImageView(deviceObj->device, &viewCI, nullptr, &texture->view);
    assert(!error);

    // 填充可用于设置描述符集的描述符图像信息
    texture->descsImgInfo.imageView = texture->view;
    texture->descsImgInfo.sampler = texture->sampler;
    texture->descsImgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
}

void VulkanRenderer::createTextureLinear(const char *filename, TextureData *texture, VkImageUsageFlags imageUsageFlags, VkFormat format) {
    // 载入图像
    gli::texture2D image2D(gli::load(filename));
    assert(!image2D.empty());

    // 获取图像的大小
    texture->textureWidth = uint32_t(image2D[0].dimensions().x);
    texture->textureHeight = uint32_t(image2D[0].dimensions().y);

    // 设置mip-map登记
    texture->mipMapLevels = uint32_t(image2D.levels());

    // 使用VkImageCreateInfo创建图像资源状态信息
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.extent.width = uint32_t(image2D[0].dimensions().x);
    imageCreateInfo.extent.height = uint32_t(image2D[0].dimensions().y);
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = texture->mipMapLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.usage = imageUsageFlags;
    imageCreateInfo.flags = 0;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
    imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;

    VkResult error;
    // 使用创建图像信息并创建图像对象
    error = vkCreateImage(deviceObj->device, &imageCreateInfo, nullptr, &texture->image);
    assert(!error);

    // 获取缓冲区内存需求
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(deviceObj->device, texture->image, &memoryRequirements);

    // 创建内存分配元数据信息
    VkMemoryAllocateInfo &memAlloc = texture->memoryAlloc;
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = nullptr;
    memAlloc.allocationSize = memoryRequirements.size;
    memAlloc.memoryTypeIndex = 0;

    // 借助内存属性确定所需的内存类型
    bool pass = deviceObj->memoryTypeFromProperties(memoryRequirements.memoryTypeBits,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &texture->memoryAlloc.memoryTypeIndex);
    assert(pass);

    // 为缓冲区对象分配内存
    error = vkAllocateMemory(deviceObj->device, &texture->memoryAlloc, nullptr, &(texture->mem));
    assert(!error);

    // 为图像绑定设备内存
    error = vkBindImageMemory(deviceObj->device, texture->image, texture->mem, 0);
    assert(!error);

    VkImageSubresource subresource = {};
    subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresource.mipLevel = 0;
    subresource.arrayLayer = 0;

    VkSubresourceLayout layout;
    uint8_t *data;

    vkGetImageSubresourceLayout(deviceObj->device, texture->image, &subresource, &layout);

    // 将GPU内存映射到本地主机
    error = vkMapMemory(deviceObj->device, texture->mem, 0, texture->memoryAlloc.allocationSize, 0, (void **) &data);
    assert(!error);

    // 在映射缓冲区中加载图像纹理数据
    uint8_t *dataTemp = (uint8_t *) image2D.data();
    for (int y = 0; y < image2D[0].dimensions().y; y++) {
        size_t imageSize = image2D[0].dimensions().y * 4;
        memcpy(data, dataTemp, imageSize);
        dataTemp += imageSize;

        // 一行行的读取数据
        data += layout.rowPitch;
    }

    // 取消映射主机内存以将更改推入设备内存
    vkUnmapMemory(deviceObj->device, texture->mem);

    // 命令缓冲区分配和记录开始
    CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdTexture);
    CommandBufferMgr::beginCommandBuffer(cmdTexture);

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = texture->mipMapLevels;
    subresourceRange.layerCount = 1;

    texture->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    convertImageLayout(texture->image, VK_IMAGE_ASPECT_COLOR_BIT,
                   VK_IMAGE_LAYOUT_PREINITIALIZED, texture->imageLayout,
                   subresourceRange, cmdTexture);

    // 停止指令缓存的录制
    CommandBufferMgr::endCommandBuffer(cmdTexture);

    // 确保GPU在主机再次接管前完成提交的作业
    VkFence fence;
    VkFenceCreateInfo fenceCI = {};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCI.flags = 0;

    vkCreateFence(deviceObj->device, &fenceCI, nullptr, &fence);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdTexture;

    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdTexture, &submitInfo, fence);
    vkWaitForFences(deviceObj->device, 1, &fence, VK_TRUE, 10000000000);
    vkDestroyFence(deviceObj->device, fence, nullptr);

    // Specify a particular kind of texture using samplers
    VkSamplerCreateInfo samplerCI = {};
    samplerCI.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCI.pNext = nullptr;
    samplerCI.magFilter = VK_FILTER_LINEAR;
    samplerCI.minFilter = VK_FILTER_LINEAR;
    samplerCI.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCI.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCI.mipLodBias = 0.0f;
    if (deviceObj->deviceFeatures.samplerAnisotropy == VK_TRUE) {
        samplerCI.anisotropyEnable = VK_TRUE;
        samplerCI.maxAnisotropy = 8;
    } else {
        samplerCI.anisotropyEnable = VK_FALSE;
        samplerCI.maxAnisotropy = 1;
    }
    samplerCI.compareOp = VK_COMPARE_OP_NEVER;
    samplerCI.minLod = 0.0f;
    samplerCI.maxLod = 0.0f;// Set to texture->mipLevels if Optimal tiling, generally linear does not support mip-maping
    samplerCI.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    samplerCI.unnormalizedCoordinates = VK_FALSE;

    // Create the sampler
    error = vkCreateSampler(deviceObj->device, &samplerCI, nullptr, &texture->sampler);
    assert(!error);

    // 创建图像视图，允许着色器访问纹理信息
    VkImageViewCreateInfo viewCI = {};
    viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCI.pNext = nullptr;
    viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCI.format = format;
    viewCI.components.r = VK_COMPONENT_SWIZZLE_R;
    viewCI.components.g = VK_COMPONENT_SWIZZLE_G;
    viewCI.components.b = VK_COMPONENT_SWIZZLE_B;
    viewCI.components.a = VK_COMPONENT_SWIZZLE_A;
    viewCI.subresourceRange = subresourceRange;
    viewCI.subresourceRange.levelCount = 1;
    viewCI.flags = 0;
    viewCI.image = texture->image;

    error = vkCreateImageView(deviceObj->device, &viewCI, nullptr, &texture->view);
    assert(!error);

    texture->descsImgInfo.sampler = texture->sampler;
    texture->descsImgInfo.imageView = texture->view;
    texture->descsImgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    // 在可绘制对象中设置创建的纹理
    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->setTextures(texture);
    }
}

void VulkanRenderer::createRenderPass(bool isDepthSupported, bool clear) {
    // VulkanSwapChain::createSwapChain()来获得彩色表面图像;
    // VulkanRenderer::createDepthBuffer()来获得深度缓冲图像

    VkResult result;
    // 将颜色附件和深度附件关联到渲染通道实例
    VkAttachmentDescription attachments[2];
    attachments[0].format = swapChainObj->scPublicVars.format;
    attachments[0].samples = NUM_SAMPLES;
    attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachments[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;

    // 如果深度附件可用，在这里定义深度缓存附件的属性
    if (isDepthSupported) {
        attachments[1].format = Depth.format;
        attachments[1].samples = NUM_SAMPLES;
        attachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
    }

    // 定义颜色缓冲附件绑定点和布局信息
    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // 定义深度缓冲区附件绑定点和布局信息
    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // 设置附件信息：颜色、深度、解析、保留等
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = isDepthSupported ? &depthReference : nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    // 设置渲染通道中的子通道和附件信息
    VkRenderPassCreateInfo rpInfo = {};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpInfo.pNext = nullptr;
    rpInfo.attachmentCount = isDepthSupported ? 2 : 1;
    rpInfo.pAttachments = attachments;
    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses = &subpass;
    rpInfo.dependencyCount = 0;
    rpInfo.pDependencies = nullptr;

    // 创建渲染通道对象
    result = vkCreateRenderPass(deviceObj->device, &rpInfo, nullptr, &renderPass);
    assert(result == VK_SUCCESS);
}

void VulkanRenderer::createFrameBuffer(bool includeDepth) {
    // 依赖于createDepthBuffer(), createRenderPass() and createSwapChain()
    VkResult result;
    VkImageView attachments[2];
    attachments[1] = Depth.view;

    VkFramebufferCreateInfo fbInfo = {};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.pNext = nullptr;
    fbInfo.renderPass = renderPass;
    fbInfo.attachmentCount = includeDepth ? 2 : 1;
    fbInfo.pAttachments = attachments;
    fbInfo.width = width;
    fbInfo.height = height;
    fbInfo.layers = 1;

    uint32_t i;

    framebuffers.clear();
    framebuffers.resize(swapChainObj->scPublicVars.swapchainImageCount);
    for (i = 0; i < swapChainObj->scPublicVars.swapchainImageCount; i++) {
        attachments[0] = swapChainObj->scPublicVars.colorBuffer[i].view;
        result = vkCreateFramebuffer(deviceObj->device, &fbInfo, nullptr, &framebuffers.at(i));
        assert(result == VK_SUCCESS);
    }
}

void VulkanRenderer::destroyFramebuffers() {
    for (uint32_t i = 0; i < swapChainObj->scPublicVars.swapchainImageCount; i++) {
        vkDestroyFramebuffer(deviceObj->device, framebuffers.at(i), nullptr);
    }
    framebuffers.clear();
}

void VulkanRenderer::destroyRenderpass() {
    vkDestroyRenderPass(deviceObj->device, renderPass, nullptr);
}

void VulkanRenderer::destroyDrawableVertexBuffer() {
    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->destroyVertexBuffer();
    }
}

void VulkanRenderer::destroyDrawableUniformBuffer() {
    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->destroyUniformBuffer();
    }
}

void VulkanRenderer::destroyTextureResource() {
    vkFreeMemory(deviceObj->device, texture.mem, nullptr);
    vkDestroySampler(deviceObj->device, texture.sampler, nullptr);
    vkDestroyImage(deviceObj->device, texture.image, nullptr);
    vkDestroyImageView(deviceObj->device, texture.view, nullptr);
}

void VulkanRenderer::destroyDrawableCommandBuffer() {
    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->destroyCommandBuffer();
    }
}

void VulkanRenderer::destroyDrawableSynchronizationObjects() {
    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->destroySynchronizationObjects();
    }
}

void VulkanRenderer::destroyDepthBuffer() {
    vkDestroyImageView(deviceObj->device, Depth.view, nullptr);
    vkDestroyImage(deviceObj->device, Depth.image, nullptr);
    vkFreeMemory(deviceObj->device, Depth.mem, nullptr);
}

void VulkanRenderer::destroyCommandBuffer() {
    VkCommandBuffer cmdBufs[] = {cmdDepthImage, cmdVertexBuffer, cmdTexture};
    vkFreeCommandBuffers(deviceObj->device, cmdPool, sizeof(cmdBufs) / sizeof(VkCommandBuffer), cmdBufs);
}

void VulkanRenderer::destroyCommandPool() {
    VulkanDevice *deviceObj = application->deviceObj;

    vkDestroyCommandPool(deviceObj->device, cmdPool, nullptr);
}

void VulkanRenderer::buildSwapChainAndDepthImage() {
    // 获取用于提交指令的队列
    deviceObj->getDeviceQueue();

    // 创建交换链，获取颜色图对象
    swapChainObj->createSwapChain(cmdDepthImage);

    // 创建深度图
    createDepthImage();
}

void VulkanRenderer::createVertexBuffer() {
    CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdVertexBuffer);
    CommandBufferMgr::beginCommandBuffer(cmdVertexBuffer);

    for (VulkanDrawable *drawableObj: drawableList) {
        drawableObj->createVertexBuffer(geometryData, sizeof(geometryData), sizeof(geometryData[0]), false);
    }
    CommandBufferMgr::endCommandBuffer(cmdVertexBuffer);
    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdVertexBuffer);
}

void VulkanRenderer::createShaders() {
    if (application->isResizing)
        return;

    void *vertShaderCode, *fragShaderCode;
    size_t sizeVert, sizeFrag;

#ifdef AUTO_COMPILE_GLSL_TO_SPV
    vertShaderCode = readFile("./../Texture.vert", &sizeVert);
    fragShaderCode = readFile("./../Texture.frag", &sizeFrag);

    shaderObj.buildShader((const char *) vertShaderCode, (const char *) fragShaderCode);
#else
    vertShaderCode = readFile("./../Texture-vert.spv", &sizeVert);
    fragShaderCode = readFile("./../Texture-frag.spv", &sizeFrag);

    shaderObj.buildShaderModuleWithSPV((uint32_t *) vertShaderCode, sizeVert, (uint32_t *) fragShaderCode, sizeFrag);
#endif
}

// 创建描述符集
void VulkanRenderer::createDescriptors() {
    for (VulkanDrawable *drawableObj: drawableList) {
        // 这取决于应用程序如何管理描述符的创建。描述符可以缓存并重用于所有类似的对象
        drawableObj->createDescriptorSetLayout(true);

        // 创建描述符集
        drawableObj->createDescriptor(true);
    }
}

void VulkanRenderer::createPipelineStateManagement() {
    for (VulkanDrawable *drawableObj: drawableList) {
        // 使用描述符布局并创建管道布局
        drawableObj->createPipelineLayout();
    }

    pipelineObj.createPipelineCache();

    const bool depthPresent = false;
    for (VulkanDrawable *drawableObj: drawableList) {
        VkPipeline *pipeline = (VkPipeline *) malloc(sizeof(VkPipeline));
        if (pipelineObj.createPipeline(drawableObj, pipeline, &shaderObj, depthPresent)) {
            pipelineList.push_back(pipeline);
            drawableObj->setPipeline(pipeline);
        } else {
            free(pipeline);
            pipeline = nullptr;
        }
    }
}

//转换图像布局
void VulkanRenderer::convertImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, const VkImageSubresourceRange &subresourceRange, const VkCommandBuffer &cmd) {
    // 依赖于指令缓冲
    assert(cmd != VK_NULL_HANDLE);

    // deviceObj->queue必须完成初始化
    assert(deviceObj->queue != VK_NULL_HANDLE);

    VkImageMemoryBarrier imgMemoryBarrier = {};
    imgMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imgMemoryBarrier.pNext = nullptr;
    imgMemoryBarrier.srcAccessMask = 0;
    imgMemoryBarrier.dstAccessMask = 0;
    imgMemoryBarrier.oldLayout = oldImageLayout;
    imgMemoryBarrier.newLayout = newImageLayout;
    imgMemoryBarrier.image = image;
    imgMemoryBarrier.subresourceRange = subresourceRange;

    if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        imgMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    // 源布局(旧)
    switch (oldImageLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            imgMemoryBarrier.srcAccessMask = 0;
            break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    switch (newImageLayout) {
        // 确保从该映像复制的任何内容都已完成
        // 这种布局中的图像只能用作命令的目标操作数
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            break;

        // 确保任何复制或CPU对图像的写操作先完成
        // 这种布局中的图像只能用作着色器的只读资源
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        // 此布局中的图像只能用作帧缓冲颜色附件
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            break;

        // 此布局中的图像只能用作帧缓冲区深度/模板附件
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;
    }

    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(cmd, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &imgMemoryBarrier);
}

// 销毁渲染器中存在的每个管道对象
void VulkanRenderer::destroyPipeline() {
    for (VkPipeline *pipeline: pipelineList) {
        vkDestroyPipeline(deviceObj->device, *pipeline, nullptr);
        free(pipeline);
    }
    pipelineList.clear();
}
