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

    swapChainObj = new VulkanSwapChain(this);
    VulkanDrawable *drawableObj = new VulkanDrawable(this);
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

    // 创建描述符集布局
    createDescriptors();

    // 创建流水线状态管理对象
    createPipelineStateManagement();

    // Build the push constants
    createPushConstants();
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

    sprintf(name, "Push Constants");
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
    //imgViewInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY};
    imgViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;// 设置R通道调和
    imgViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;// 设置G通道调和
    imgViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;// 设置B通道调和
    imgViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;// 设置A通道调和
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
        // 设置图像布局为：深度蒙版优化
        convertImageLayout(Depth.image,
                       imgViewInfo.subresourceRange.aspectMask,
                       VK_IMAGE_LAYOUT_UNDEFINED,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, (VkAccessFlagBits) 0, cmdDepthImage);
    }
    CommandBufferMgr::endCommandBuffer(cmdDepthImage);
    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdDepthImage);

    // 创建图像视图并允许应用程序使用图像
    imgViewInfo.image = Depth.image;
    result = vkCreateImageView(deviceObj->device, &imgViewInfo, nullptr, &Depth.view);
    assert(result == VK_SUCCESS);
}

void VulkanRenderer::createRenderPass(bool isDepthSupported, bool clear) {
    // VulkanSwapChain::createSwapChain()来获得彩色表面图像;VulkanRenderer::createDepthBuffer()来获得深度缓冲图像

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
    // 依赖于reateDepthBuffer(), createRenderPass() and recordSwapChain()
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
    VkCommandBuffer cmdBufs[] = {cmdDepthImage, cmdVertexBuffer, cmdPushConstant};
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
    vertShaderCode = readFile("./../PushConstant.vert", &sizeVert);
    fragShaderCode = readFile("./../PushConstant.frag", &sizeFrag);

    shaderObj.buildShader((const char *) vertShaderCode, (const char *) fragShaderCode);
#else
    vertShaderCode = readFile("./../PushConstant-vert.spv", &sizeVert);
    fragShaderCode = readFile("./../PushConstant-frag.spv", &sizeFrag);

    shaderObj.buildShaderModuleWithSPV((uint32_t *) vertShaderCode, sizeVert, (uint32_t *) fragShaderCode, sizeFrag);
#endif
}

// 创建描述符集
void VulkanRenderer::createDescriptors() {
    for (VulkanDrawable *drawableObj: drawableList) {
        // 这取决于应用程序如何管理描述符的创建。描述符可以缓存并重用于所有类似的对象
        drawableObj->createDescriptorSetLayout(false);

        // 创建描述符集
        drawableObj->createDescriptor(false);
    }
}

void VulkanRenderer::createPushConstants() {
    CommandBufferMgr::allocCommandBuffer(&deviceObj->device, cmdPool, &cmdPushConstant);
    CommandBufferMgr::beginCommandBuffer(cmdPushConstant);

    enum ColorFlag {
        RED = 1,
        GREEN = 2,
        BLUE = 3,
        MIXED_COLOR = 4,
    };

    float mixerValue = 0.5f;
    unsigned constColorRGBFlag = MIXED_COLOR;

    // 创建推送常亮的数据，这里包含一个常数形式的颜色标记，以及针对非常亮颜色值的混合选项
    unsigned pushConstants[2] = {};
    pushConstants[0] = constColorRGBFlag;
    memcpy(&pushConstants[1], &mixerValue, sizeof(float));

    // 检查推送推送常量的大小是否超出了设备的最大推送常亮的大小
    int maxPushContantSize = getDevice()->gpuProps.limits.maxPushConstantsSize;
    if (sizeof(pushConstants) > maxPushContantSize) {
        assert(0);
        printf("推送常量的数据比预期更大, 最大允许的大小为：%d", maxPushContantSize);
    }

    for (VulkanDrawable *drawableObj: drawableList) {
        vkCmdPushConstants(cmdPushConstant, drawableObj->pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushConstants), pushConstants);
    }

    CommandBufferMgr::endCommandBuffer(cmdPushConstant);
    CommandBufferMgr::submitCommandBuffer(deviceObj->queue, &cmdPushConstant);
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

void VulkanRenderer::convertImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkAccessFlagBits srcAccessMask, const VkCommandBuffer &cmd) {
    assert(cmd != VK_NULL_HANDLE);
    assert(deviceObj->queue != VK_NULL_HANDLE);

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.pNext = nullptr;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = 0;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    // 设置源和目标访问掩码以及管线阶段
    VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    switch (oldImageLayout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            srcStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
            // ... 其他布局的访问标志和阶段 ...
    }

    switch (newImageLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            destStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            destStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            srcStageFlags = VK_PIPELINE_STAGE_HOST_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT;
            destStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            destStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            destStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            break;
            // ... 其他布局的访问标志和阶段 ...
    }

    vkCmdPipelineBarrier(
            cmd,
            srcStageFlags, destStageFlags,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);
}

// 销毁渲染器中存在的每个管道对象
void VulkanRenderer::destroyPipeline() {
    for (VkPipeline *pipeline: pipelineList) {
        vkDestroyPipeline(deviceObj->device, *pipeline, nullptr);
        free(pipeline);
    }
    pipelineList.clear();
}
