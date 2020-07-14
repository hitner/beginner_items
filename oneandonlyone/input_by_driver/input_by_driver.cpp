// input_by_driver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <Windows.h>

void PrintDeviceDetail(const DISPLAY_DEVICE & device) {
  wprintf(L"Name:%s\nString:%s\nID:%s\nKey:%s\n\n", device.DeviceName, 
    device.DeviceString, device.DeviceID, device.DeviceKey);
}

void PrintDeviceDetail(const DEVMODE & mode) {
  wprintf(L"width:%d, height:%d, fields:%d\n\n", mode.dmPelsWidth,
    mode.dmPelsHeight, mode.dmFields);
}

int main()
{
  DISPLAY_DEVICE device;
  memset(&device, 0, sizeof(DISPLAY_DEVICE));
  device.cb = sizeof(DISPLAY_DEVICE);

  UINT index;
  for (index = 0; index < 10; index++) {
    BOOL ret = ::EnumDisplayDevices(NULL, index, &device, 0); //adapter 可能有多個
    if (ret == 0) {
      break;
    }
    else {


      PrintDeviceDetail(device);
      DISPLAY_DEVICE device_detial;
      memset(&device_detial, 0, sizeof(DISPLAY_DEVICE));
      device_detial.cb = sizeof(DISPLAY_DEVICE);
      BOOL ret2 = ::EnumDisplayDevices(device.DeviceName, 0, &device_detial, 0); //display monitor may less than adapter
      if (ret2) {
        PrintDeviceDetail(device_detial);
        
      }
    }
  }

  DEVMODE currentMode;
  memset(&currentMode, 0, sizeof(DEVMODE));
  currentMode.dmSize = sizeof(DEVMODE);


  if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &currentMode)) {
    PrintDeviceDetail(currentMode);
  }

  DEVMODE dm = { 0 };
  dm.dmSize = sizeof(dm);
  for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++) {
    std::cout << "Mode #" << iModeNum << " = " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << std::endl;
  }


  DEVMODE devMode;
  memset(&devMode, 0, sizeof(DEVMODE));
  devMode.dmSize = sizeof(DEVMODE);
  devMode.dmPelsWidth = 1920;
  devMode.dmPelsHeight = 1080;
  devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

  LONG result = ::ChangeDisplaySettings(&devMode, 0);
  if (result == DISP_CHANGE_SUCCESSFUL) {
    std::cout << "Hello World! success \n";
  }
  else {
    std::cout << "Hello World! failed" << result << "\n";
  }
  
  int a;
  std::cin >> a;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
