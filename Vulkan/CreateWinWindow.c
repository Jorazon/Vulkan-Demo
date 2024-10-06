#ifdef _WIN32

#include "MakeWindow.h"

const char CLASS_NAME[] = "SampleWindowClass";
WNDCLASSA windowClass = { 0 };
const char WINDOW_NAME[] = "Vulkan Triangle";
HINSTANCE hInstance = NULL;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Get HINSTANCE of exe
void get_hInstance() {
  hInstance = GetModuleHandle(NULL);
}

WNDCLASSA* register_window_class() {
  get_hInstance();

  if (!hInstance) {
    fwprintf(stderr, L"HINSTANCE get failed with 0x%x\n", GetLastError());
  }

  windowClass.lpfnWndProc = WindowProc;                   // Window procedure function. Use DefWindowProcA instead of NULL
  windowClass.hInstance = hInstance;                      // Application instance handle
  windowClass.lpszClassName = CLASS_NAME;                 // Class name
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);      // Default cursor
  windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Background color

  // Register the window class
  if (!RegisterClassA(&windowClass)) {
    fprintf(stderr, "Failed to register window class");
    return NULL;
  }

  return &windowClass;
}

void* make_window(uint32_t width, uint32_t height){
  // try to register window class
    if (!register_window_class()) return NULL;

    HWND window_handle = NULL;

    window_handle = CreateWindowExA(
    WS_EX_LEFT, //          [in]           DWORD     dwExStyle,
    CLASS_NAME, //          [in, optional] LPCSTR    lpClassName,
    WINDOW_NAME, //         [in, optional] LPCSTR    lpWindowName,
    WS_OVERLAPPEDWINDOW, // [in]           DWORD     dwStyle,
    CW_USEDEFAULT, //       [in]           int       X,
    CW_USEDEFAULT, //       [in]           int       Y,
    width, //               [in]           int       nWidth,
    height, //              [in]           int       nHeight,
    NULL, //                [in, optional] HWND      hWndParent,
    NULL, //                [in, optional] HMENU     hMenu,
    NULL, //                [in, optional] HINSTANCE hInstance,
    NULL //                 [in, optional] LPVOID    lpParam
  );

  if (!window_handle) {
    fprintf(stderr, "Failed to create window\n");
    return NULL;
  }

  ShowWindow(window_handle, SW_SHOW);

  return (void*)window_handle;
}

// Window procedure function
// paints text onto the window
// idk what the point is got it from a tutorial
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    TextOutA(hdc, 10, 10, "Hello, Windows!", 15);
    EndPaint(hwnd, &ps);
  }
    return 0;

  default:
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
  }
}

// loop
void window_loop(
    VkEngine engine,
    VkSwapChain swapChain,
    VkGraphicsPipeline pipeline,
    VkQueue graphicsQueue,
    VkQueue presentQueue
) {
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    RenderTriangle(
      engine._device,
      swapChain.swapChain,
      pipeline.imageAvailableSemaphore,
      pipeline.renderFinishedSemaphore,
      pipeline.commandBuffers,
      swapChain.swapChainImageCount,
      graphicsQueue,
      presentQueue
    );
  }
  return;
}

#endif // _WIN32
