#include "Util.h"

//==============================================================================================================

#include "WaveFile.h"
#include <comdef.h>
#include <iostream>


#define MAX_AUDIO_BUF 4
#define BUFFERNOTIFYSIZE 192000
BOOL g_bPlaying = FALSE;//是否正在播放
LPDIRECTSOUNDNOTIFY8 g_pDSNotify = nullptr;
DSBPOSITIONNOTIFY g_aPosNotify[MAX_AUDIO_BUF];//设置通知标志的数组
HANDLE g_event[MAX_AUDIO_BUF];
DWORD g_dwNextWriteOffset = 0;

LPDIRECTSOUNDBUFFER8 g_pDSBuffer8 = nullptr;//buffer
LPDIRECTSOUND8 g_pDsd = nullptr;//dsound
CWaveFile *g_pWaveFile = nullptr;

BOOL InitDirectSound(HWND hWnd, std::string string) {
    //下面初始化DirectSound作业。
    HRESULT hr;
    if (FAILED(hr = DirectSoundCreate8(nullptr, &g_pDsd, nullptr)))
        return FALSE;
    //设置设备的协作度
    if (FAILED(hr = g_pDsd->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
        return FALSE;

    g_pWaveFile = new CWaveFile;
    _bstr_t bstr(string.c_str());
    LPTSTR str2 = (LPTSTR) bstr;
    g_pWaveFile->Open(str2, nullptr, WAVEFILE_READ);

    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLFX | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = g_pWaveFile->GetSize();//MAX_AUDIO_BUF * BUFFERNOTIFYSIZE ;
    dsbd.lpwfxFormat = g_pWaveFile->m_pwfx;


    LPDIRECTSOUNDBUFFER lpbuffer;
    //创建辅助缓冲区对象
    if (FAILED(hr = g_pDsd->CreateSoundBuffer(&dsbd, &lpbuffer, nullptr)))
        return FALSE;
    if (FAILED(hr = lpbuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *) &g_pDSBuffer8)))
        return FALSE;
    lpbuffer->Release();


    //设置DirectSound通知 机制
    for (int i = 0; i < MAX_AUDIO_BUF; i++) {
        g_event[i] = CreateEvent(0, 0, 0, 0);
        g_aPosNotify[i].dwOffset = i * BUFFERNOTIFYSIZE;
        g_aPosNotify[i].hEventNotify = g_event[i];
    }
    if (FAILED(hr = g_pDSBuffer8->QueryInterface(IID_IDirectSoundNotify, (LPVOID *) &g_pDSNotify)))
        return FALSE;
    g_pDSNotify->SetNotificationPositions(MAX_AUDIO_BUF, g_aPosNotify);
    g_pDSNotify->Release();

    return TRUE;
}

void ProcessBuffer() {
    DWORD dwBytesWrittenToBuffer = 0;
    VOID *pDSLockedBuffer = nullptr;
    VOID *pDSLockedBuffer2 = nullptr;
    DWORD dwDSLockedBufferSize;
    DWORD dwDSLockedBufferSize2;
    HRESULT hr;
    hr = g_pDSBuffer8->Lock(g_dwNextWriteOffset, BUFFERNOTIFYSIZE, &pDSLockedBuffer, &dwDSLockedBufferSize, &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0);
    if (hr == DSERR_BUFFERLOST) {
        g_pDSBuffer8->Restore();
        g_pDSBuffer8->Lock(g_dwNextWriteOffset, BUFFERNOTIFYSIZE, &pDSLockedBuffer, &dwDSLockedBufferSize,
                           &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0);
    }
    if (SUCCEEDED(hr)) {
        //write
        g_pWaveFile->Read((BYTE *) pDSLockedBuffer, dwDSLockedBufferSize, &dwBytesWrittenToBuffer);
        g_dwNextWriteOffset += dwBytesWrittenToBuffer;

        if (nullptr != pDSLockedBuffer2) {
            g_pWaveFile->Read((BYTE *) pDSLockedBuffer2, dwDSLockedBufferSize2, &dwBytesWrittenToBuffer);
            g_dwNextWriteOffset += dwBytesWrittenToBuffer;
        }
        g_dwNextWriteOffset %= (BUFFERNOTIFYSIZE * MAX_AUDIO_BUF);

        if (dwBytesWrittenToBuffer < BUFFERNOTIFYSIZE) {
            FillMemory((BYTE *) pDSLockedBuffer + dwBytesWrittenToBuffer,
                       BUFFERNOTIFYSIZE - dwBytesWrittenToBuffer,
                       (BYTE) (g_pWaveFile->m_pwfx->wBitsPerSample == 8 ? 128 : 0));

            g_bPlaying = FALSE;
        }
        hr = g_pDSBuffer8->Unlock(pDSLockedBuffer, dwDSLockedBufferSize,
                                  pDSLockedBuffer2, dwDSLockedBufferSize2);
    }
}

DWORD WINAPI PlayThread(LPVOID lpParame) {
    DWORD res;
    LPVOID lplockbuf;
    DWORD len;
    DWORD dwWrite;

    g_pDSBuffer8->Lock(0, 0, &lplockbuf, &len, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
    g_pWaveFile->Read((BYTE *) lplockbuf, len, &dwWrite);
    g_pDSBuffer8->Unlock(lplockbuf, len, nullptr, 0);
    g_pDSBuffer8->SetCurrentPosition(0);
    g_pDSBuffer8->Play(0, 0, DSBPLAY_LOOPING);
    g_dwNextWriteOffset = 0;
    while (g_bPlaying) {
        res = WaitForMultipleObjects(MAX_AUDIO_BUF, g_event, FALSE, INFINITE);
        if (res > WAIT_OBJECT_0)
            ProcessBuffer();
    }

    return 0;
}


BOOL DirectSoundPlay(bool bLoop) {
    LPVOID lplockbuf;
    DWORD len;
    DWORD dwWrite;

    g_pDSBuffer8->Lock(0, 0, &lplockbuf, &len, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
    g_pWaveFile->Read((BYTE *) lplockbuf, len, &dwWrite);
    g_pDSBuffer8->Unlock(lplockbuf, len, nullptr, 0);
    g_pDSBuffer8->SetCurrentPosition(0);
    g_pDSBuffer8->Play(0, 0, bLoop);

    /*g_bPlaying =TRUE;
	g_pWaveFile->ResetFile();
	CreateThread(0,0,PlayThread,nullptr,nullptr,nullptr);*/

    return TRUE;
}

BOOL DirectSoundPlay2(bool bLoop) {
    LPVOID lplockbuf;
    DWORD len;
    DWORD dwWrite;

    /*g_pDSBuffer8->Lock(0, 0, &lplockbuf, &len, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
	g_pWaveFile->Read((BYTE*)lplockbuf, len, &dwWrite);
	g_pDSBuffer8->Unlock(lplockbuf, len, nullptr, 0);
	g_pDSBuffer8->SetCurrentPosition(0);
	g_pDSBuffer8->Play(0, 0, bLoop);*/

    g_bPlaying = TRUE;
    g_pWaveFile->ResetFile();
    CreateThread(0, 0, PlayThread, nullptr, nullptr, nullptr);

    return TRUE;
}


void playMusic(HWND hwnd, std::string soundName, bool flag) {
    if (!InitDirectSound(hwnd, soundName)) {

        MessageBox(nullptr, TEXT("初始化DS错误"), 0, 0);
    }
    printf("叮叮叮\n");
    DirectSoundPlay(flag);
}

void playBackGroundMusic(HWND hwnd, std::string soundName, bool flag) {
    printf("播放");
    /*if (!InitDirectSound(hwnd, soundName))
		{
			MessageBox(nullptr, TEXT("初始化DS错误"), 0, 0);
		}*/
    DirectSoundPlay(flag);
}

void soundStop() {
    printf("暂停2");
    g_pDSBuffer8->Stop();
    //g_pDSBuffer8->Release();
    //CloseHandle(PlayThread);


    /*g_bPlaying = false;
	TerminateThread(PlayThread, EXIT_FAILURE);
	CloseHandle(PlayThread);*/

    /*SuspendThread(PlayThread);*/
}

//==============================================================================================================


using namespace std;

int preX = 0;
int preY = 0;
bool left_mouse_down = false;
void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    // 处理鼠标拖动事件
    if (left_mouse_down) {
    }

    //保存本次的鼠标位置作为上次的位置
    preX = (int) xpos;
    preY = (int) ypos;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        left_mouse_down = true;
        printf("鼠标按下\n");
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        left_mouse_down = false;
        printf("鼠标松开\n");
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    printf("scroll called [%F %F]\n", xoffset, yoffset);
}

// 键盘按键回调函数
void processInputs(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        std::cout << "按下了W键" << std::endl;
}


void init_resources(TBuiltInResource &Resources) {
    Resources.maxLights = 32;
    Resources.maxClipPlanes = 6;
    Resources.maxTextureUnits = 32;
    Resources.maxTextureCoords = 32;
    Resources.maxVertexAttribs = 64;
    Resources.maxVertexUniformComponents = 4096;
    Resources.maxVaryingFloats = 64;
    Resources.maxVertexTextureImageUnits = 32;
    Resources.maxCombinedTextureImageUnits = 80;
    Resources.maxTextureImageUnits = 32;
    Resources.maxFragmentUniformComponents = 4096;
    Resources.maxDrawBuffers = 32;
    Resources.maxVertexUniformVectors = 128;
    Resources.maxVaryingVectors = 8;
    Resources.maxFragmentUniformVectors = 16;
    Resources.maxVertexOutputVectors = 16;
    Resources.maxFragmentInputVectors = 15;
    Resources.minProgramTexelOffset = -8;
    Resources.maxProgramTexelOffset = 7;
    Resources.maxClipDistances = 8;
    Resources.maxComputeWorkGroupCountX = 65535;
    Resources.maxComputeWorkGroupCountY = 65535;
    Resources.maxComputeWorkGroupCountZ = 65535;
    Resources.maxComputeWorkGroupSizeX = 1024;
    Resources.maxComputeWorkGroupSizeY = 1024;
    Resources.maxComputeWorkGroupSizeZ = 64;
    Resources.maxComputeUniformComponents = 1024;
    Resources.maxComputeTextureImageUnits = 16;
    Resources.maxComputeImageUniforms = 8;
    Resources.maxComputeAtomicCounters = 8;
    Resources.maxComputeAtomicCounterBuffers = 1;
    Resources.maxVaryingComponents = 60;
    Resources.maxVertexOutputComponents = 64;
    Resources.maxGeometryInputComponents = 64;
    Resources.maxGeometryOutputComponents = 128;
    Resources.maxFragmentInputComponents = 128;
    Resources.maxImageUnits = 8;
    Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
    Resources.maxCombinedShaderOutputResources = 8;
    Resources.maxImageSamples = 0;
    Resources.maxVertexImageUniforms = 0;
    Resources.maxTessControlImageUniforms = 0;
    Resources.maxTessEvaluationImageUniforms = 0;
    Resources.maxGeometryImageUniforms = 0;
    Resources.maxFragmentImageUniforms = 8;
    Resources.maxCombinedImageUniforms = 8;
    Resources.maxGeometryTextureImageUnits = 16;
    Resources.maxGeometryOutputVertices = 256;
    Resources.maxGeometryTotalOutputComponents = 1024;
    Resources.maxGeometryUniformComponents = 1024;
    Resources.maxGeometryVaryingComponents = 64;
    Resources.maxTessControlInputComponents = 128;
    Resources.maxTessControlOutputComponents = 128;
    Resources.maxTessControlTextureImageUnits = 16;
    Resources.maxTessControlUniformComponents = 1024;
    Resources.maxTessControlTotalOutputComponents = 4096;
    Resources.maxTessEvaluationInputComponents = 128;
    Resources.maxTessEvaluationOutputComponents = 128;
    Resources.maxTessEvaluationTextureImageUnits = 16;
    Resources.maxTessEvaluationUniformComponents = 1024;
    Resources.maxTessPatchComponents = 120;
    Resources.maxPatchVertices = 32;
    Resources.maxTessGenLevel = 64;
    Resources.maxViewports = 16;
    Resources.maxVertexAtomicCounters = 0;
    Resources.maxTessControlAtomicCounters = 0;
    Resources.maxTessEvaluationAtomicCounters = 0;
    Resources.maxGeometryAtomicCounters = 0;
    Resources.maxFragmentAtomicCounters = 8;
    Resources.maxCombinedAtomicCounters = 8;
    Resources.maxAtomicCounterBindings = 1;
    Resources.maxVertexAtomicCounterBuffers = 0;
    Resources.maxTessControlAtomicCounterBuffers = 0;
    Resources.maxTessEvaluationAtomicCounterBuffers = 0;
    Resources.maxGeometryAtomicCounterBuffers = 0;
    Resources.maxFragmentAtomicCounterBuffers = 1;
    Resources.maxCombinedAtomicCounterBuffers = 1;
    Resources.maxAtomicCounterBufferSize = 16384;
    Resources.maxTransformFeedbackBuffers = 4;
    Resources.maxTransformFeedbackInterleavedComponents = 64;
    Resources.maxCullDistances = 8;
    Resources.maxCombinedClipAndCullDistances = 8;
    Resources.maxSamples = 4;
    Resources.limits.nonInductiveForLoops = 1;
    Resources.limits.whileLoops = 1;
    Resources.limits.doWhileLoops = 1;
    Resources.limits.generalUniformIndexing = 1;
    Resources.limits.generalAttributeMatrixVectorIndexing = 1;
    Resources.limits.generalVaryingIndexing = 1;
    Resources.limits.generalSamplerIndexing = 1;
    Resources.limits.generalVariableIndexing = 1;
    Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

EShLanguage FindLanguage(const VkShaderStageFlagBits shader_type) {
    switch (shader_type) {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return EShLangVertex;

        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return EShLangTessControl;

        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return EShLangTessEvaluation;

        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return EShLangGeometry;

        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return EShLangFragment;

        case VK_SHADER_STAGE_COMPUTE_BIT:
            return EShLangCompute;

        default:
            return EShLangVertex;
    }
}

void init_glslang() {
    glslang::InitializeProcess();
}

void finalize_glslang() {
    glslang::FinalizeProcess();
}
void wait_seconds(int seconds) {
    Sleep(seconds * 1000);
}
//获取毫秒数
timestamp_t get_milliseconds() {
    LARGE_INTEGER frequency;
    BOOL useQPC = QueryPerformanceFrequency(&frequency);
    if (useQPC) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / frequency.QuadPart;
    } else {
        return GetTickCount();
    }
}
