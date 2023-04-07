//Copyright 2023, Tom Kannenberg, All rights reserved.
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <vector>
#include <cstddef>

constexpr LPCSTR mc = "Minecraft* 1.19.4";

constexpr int cornerNW = 'i';           constexpr int aUP    = 72;
constexpr int cornerNE = 'o';           constexpr int aRIGHT = 77;
constexpr int cornerSW = 'k';           constexpr int aDOWN  = 80;
constexpr int cornerSE = 'l';           constexpr int aLEFT  = 75;


HWND windows[4] = {nullptr, nullptr, nullptr, nullptr};
int monitors[4] = {0, 0, 0, 0};
int currentWindow = 0;
int windowCount = 0;

void resetAll() {

    for (int i = 0; i < 4; i++) {
        if (windows[i] != NULL) {
            SetWindowTextA(windows[i], mc);
            SetWindowLong(windows[i], GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
            windows[i] = NULL;
        }
    }
    
    //reset all window names
    const char* otitle = mc;
    std::string oldtitle = (std::string) otitle + "x";
    
    while (true) {
        HWND window = FindWindowA(NULL, oldtitle.c_str());
        
        if (window == NULL) {
            break;
        }
        SetWindowTextA(window, mc);
    }
}

void getWindow() {
    
    if (windowCount < 4) {
        windows[windowCount] = FindWindowA(NULL, mc);
        
        if (windows[windowCount] == NULL) {
            std::cout << mc << " not found" << std::endl;
            
        } else {
            std::cout << mc << " found" << std::endl;
            currentWindow = windowCount;
            const char* ntitle = mc;
            std::string newtitle = (std::string) ntitle + "x";
            SetWindowTextA(windows[windowCount], newtitle.c_str());
        }
        RECT monitor;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &monitor, 0);
        int monitorWidth = (monitor.right - monitor.left) / 2;
        int monitorHeight = (monitor.bottom - monitor.top) / 2;
        SetWindowPos(windows[windowCount], NULL, 0, 0, monitorWidth, monitorHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        SetWindowLong(windows[windowCount], GWL_STYLE, WS_POPUP | WS_VISIBLE);
        windowCount++;
    }
}

void removeWindow() {
    if (windowCount > 0) {
        SetWindowTextA(windows[currentWindow], mc);
        SetWindowLong(windows[currentWindow], GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        windows[windowCount] = NULL;
        std::cout << "Window " << windowCount << " removed" << std::endl;
        windowCount--;
    }
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    std::vector<HMONITOR>* monitors = reinterpret_cast<std::vector<HMONITOR>*>(dwData);
    monitors->push_back(hMonitor);
    return TRUE;
}

HMONITOR GetMonitorByIndex(int index) {
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors));
    
    if (index >= 0 && index < static_cast<int>(monitors.size())) {
        return monitors[index];
    }

    return NULL;
}

void resizeWindow(int& wWidth, int& wHeight, int& wX, int& wY, int mLeft, int mTop, int mRight, int mBottom, bool isCorner, const int& wPos) {
    
    if (isCorner) {
        wWidth = (mRight - mLeft) / 2;
        wHeight = (mBottom - mTop) / 2;
        
        switch (wPos) {
            case 0:     wX = mLeft;             wY = mTop;              break;
            case 1:     wX = mLeft + wWidth;    wY = mTop;              break;
            case 2:     wX = mLeft + wWidth;    wY = mTop + wHeight;    break;
            case 3:     wX = mLeft;             wY = mTop + wHeight;    break;
        }
        
    } else {
        switch (wPos) {
            case 0:     wWidth = mRight - mLeft;    wHeight = (mBottom - mTop) / 2;    break;
            case 1:     wWidth = (mRight - mLeft) / 2;    wHeight = mBottom - mTop;    break;
            case 2:     wWidth = mRight - mLeft;    wHeight = (mBottom - mTop) / 2;    break;
            case 3:     wWidth = (mRight - mLeft) / 2;    wHeight = mBottom - mTop;    break;
        }

        switch (wPos) {
            case 0:     wX = mLeft;             wY = mTop;              break;
            case 1:     wX = mLeft + wWidth;    wY = mTop;              break;
            case 2:     wX = mLeft;             wY = mTop + wHeight;    break;
            case 3:     wX = mLeft + wWidth;    wY = mTop + wHeight;    break;
        }
    }
}

void moveWindow(int wPos, bool isCorner = true) {
    int mIndex = monitors[currentWindow];
    HMONITOR hMonitor = GetMonitorByIndex(mIndex);
    MONITORINFO mInfo = { sizeof(MONITORINFO) };
    GetMonitorInfo(hMonitor, &mInfo);
    RECT mRect = mInfo.rcWork;
    
    int wWidth, wHeight, wX, wY;
    resizeWindow(wWidth, wHeight, wX, wY, mRect.left, mRect.top, mRect.right, mRect.bottom, isCorner, wPos);

    // Set the position and size of the window
    SetWindowPos(windows[currentWindow], NULL, wX, wY, wWidth, wHeight, SWP_NOZORDER | SWP_NOACTIVATE);
}

void selectWindow(int window) {
    if (windows[window] != nullptr) {
        currentWindow = window;
        std::cout << "Window 1 selected" << std::endl;
    } else {
        std::cout << "Window 1 not found" << std::endl;
    }
}

void selectMonitor(char dir) {
    int newMonitor = monitors[currentWindow] + (dir == '[' ? -1 : 1);
    if (newMonitor < 0 || newMonitor >= sizeof(monitors)/sizeof(monitors[0])) {
        std::cout << "Can't go to monitor " << newMonitor << " as it doesn't exist" << std::endl;
    } else {
        monitors[currentWindow] = newMonitor;
        std::cout << "Monitor " << monitors[currentWindow] << " selected" << std::endl;
    }
}

void setFullscreenOnCurrentMonitor() {
    int monitorIndex = monitors[currentWindow];
    HMONITOR hMonitor = GetMonitorByIndex(monitorIndex);
    MONITORINFO monitorInfo = { sizeof(MONITORINFO) };
    GetMonitorInfo(hMonitor, &monitorInfo);
    RECT monitorRect = monitorInfo.rcWork;
    int windowWidth = monitorRect.right - monitorRect.left;
    int windowHeight = monitorRect.bottom - monitorRect.top;
    int windowX = monitorRect.left;
    int windowY = monitorRect.top;
    SetWindowPos(windows[currentWindow], NULL, windowX, windowY, windowWidth, windowHeight, SWP_NOZORDER | SWP_NOACTIVATE);
}

void printInstructions() {
    std::cout <<
        "Press a key:\n"
        "+: get the next window\n"
        "-: remove the last window\n"
        "1-4: select a window\n"
        "i, o, k, l: move the window as borderless fullscreen into the selected corner\n"
        "Arrow keys: move the window into the selected half of the screen\n"
        "[ and ]: select another monitor" << std::endl << std::endl;
}

int main() {
    printInstructions();
    resetAll();
    
    int input;
    while (true) {
        if (_kbhit()) {
            input = _getch();
            switch (input) {
        
                case '+':   getWindow();                      break;
                case '-':   removeWindow();                     break;

                //select windows
                case '1':   selectWindow(0);                    break;
                case '2':   selectWindow(1);                    break;
                case '3':   selectWindow(2);                    break;
                case '4':   selectWindow(3);                    break;

                //move windows          //corners                                                    //halfs
                case cornerNW:   moveWindow(0);             break;      case aUP:       moveWindow(0, false);       break;
                case cornerNE:   moveWindow(1);             break;      case aRIGHT:    moveWindow(1, false);       break;
                case cornerSE:   moveWindow(2);             break;      case aDOWN:     moveWindow(2, false);       break;
                case cornerSW:   moveWindow(3);             break;      case aLEFT:     moveWindow(3, false);       break;
                case 'f':   setFullscreenOnCurrentMonitor();    break;
                case 224:   /*ignore 224*/                      break;
                case '[':   selectMonitor(input);               break;
                case ']' :  selectMonitor(input);               break;
                case 'q':   resetAll();                      return 0;
        
                default:
                    std::cout << "Invalid input" << input << std::endl;
                break;
            }
            
        }
        Sleep(50);
    }
}