#include "stdafx.h"
#include "send_input.h"
#include <thread>
#include "lzbase.h"
#include "Ntddkbd.h"
#include <winioctl.h>
//#include "ntdd8042.h"

void _DoSendInput()
{
  INPUT input[2];
  memset(input, 0, sizeof(input));
  input[0].type = INPUT_KEYBOARD;

  input[0].ki.wVk = 65; // ASCI value of A
  input[0].ki.dwFlags = 0;
  input[0].ki.time = 0;
  input[0].ki.dwExtraInfo = 0;
  SendInput(1, &input[0], sizeof(INPUT));

  ::Sleep(1);

  input[1].type = INPUT_KEYBOARD;
  input[1].ki.wVk = 65; // ASCI value of A
  input[1].ki.dwFlags = KEYEVENTF_KEYUP;
  input[1].ki.time = 0;
  input[1].ki.dwExtraInfo = 0;

  SendInput(1, &input[1], sizeof(INPUT));
}




HANDLE open_device(HANDLE rawinput_device)
{
  UINT name_size = 0;
  UINT result = GetRawInputDeviceInfo(rawinput_device, RIDI_DEVICENAME, 0, &name_size);
  if (result == (UINT)-1 || name_size == 0) {
    ShowLastErrorBox(L"dd", ::GetLastError());
    return NULL;
  }

  std::unique_ptr<WCHAR[]> name_buffer(new WCHAR[name_size + 1]);
  name_buffer.get()[name_size] = 0;
  result = GetRawInputDeviceInfo(rawinput_device, RIDI_DEVICENAME, name_buffer.get(), &name_size);
  if (result == (UINT)-1) {
    ShowLastErrorBox(L"dd", ::GetLastError());
    return NULL;
  }

  //  Windows XP fix: The raw device path in its native form (\??\...). When you have the form \\?\ that is a crutch MS invented to make long path names available on Win32 when NT arrived despite the limitation of the Win32 subsystem to the \?? object directory
  if (name_size > 2)
  {
    WCHAR *ptr = name_buffer.get();
    if ((ptr[0] == '\\') && (ptr[1] == '?'))
      ptr[1] = '\\';
  }

  HANDLE device_handle = CreateFile(name_buffer.get(), 0 ,
    FILE_SHARE_READ | FILE_SHARE_WRITE , 0, OPEN_EXISTING, 0, 0);
  if (device_handle == INVALID_HANDLE_VALUE) {
    ShowLastErrorBox(L"dd", ::GetLastError());
    return NULL;
  }
    

  return device_handle;
}


#define IOCTL_INTERNAL_I8042_KEYBOARD_WRITE_BUFFER   CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0FF1, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED, FILE_ANY_ACCESS)
void _DoSendInput2()
{
  static HANDLE keyboardHandle = NULL;
  if (!keyboardHandle) {
    UINT nDevices;
    PRAWINPUTDEVICELIST pRawInputDeviceList;
    if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {
    }
    if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) {
    }
    if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == -1) {
      ShowLastErrorBox(L"dd", ::GetLastError());
      return;
    }
    void * kkk = (void*)pRawInputDeviceList;
    for (int i = 0; i < nDevices; i++) {

      PRAWINPUTDEVICELIST tmp = (PRAWINPUTDEVICELIST)(i * sizeof(RAWINPUTDEVICELIST) + (int)kkk);
      if (tmp->dwType == RIM_TYPEKEYBOARD) {
        keyboardHandle = tmp->hDevice;
        keyboardHandle = open_device(tmp->hDevice);
      }
      /*LPVOID pData = (LPVOID) malloc(1000);
      UINT ret;
      RID_DEVICE_INFO deviceInfo;
      deviceInfo.cbSize = sizeof(RID_DEVICE_INFO);

      UINT copyed = ::GetRawInputDeviceInfo(tmp->hDevice, RIDI_DEVICENAME, &deviceInfo, &ret);
      PWCHAR  a = (PWCHAR)pData;

      INT error = ::GetLastError();
      if (error) {
        ShowLastErrorBox(L"", error);
      }*/
    }
    // do the job...

    // after the job, free the RAWINPUTDEVICELIST
    free(pRawInputDeviceList);
  }

  UCHAR OutputBuffer[100];
  DWORD dwOutput;
  KEYBOARD_INPUT_DATA kbinput;
  memset(&kbinput, 0, sizeof(KEYBOARD_INPUT_DATA));
  kbinput.UnitId = 0;
  kbinput.MakeCode = 0x2D;
  kbinput.Flags = KEY_MAKE;
  kbinput.Reserved = 0;
  kbinput.ExtraInformation = 0;

  BOOL bRet = ::DeviceIoControl(keyboardHandle, IOCTL_WRITE,
    (char*)&kbinput, sizeof(KEYBOARD_INPUT_DATA), OutputBuffer, 100, &dwOutput, NULL);

  if (!bRet) {
    ShowLastErrorBox(L"dd", ::GetLastError());
  }


  /*HANDLE hDevice = ::CreateFile(L"\\\\.\\kbd",
    0,                // no access to the drive
    FILE_SHARE_READ | // share mode
    FILE_SHARE_WRITE,
    NULL,             // default security attributes
    OPEN_EXISTING,    // disposition
    0,                // file attributes
    NULL);            // do not copy file attributes)
  if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
  {
    ShowLastErrorBox(L"dd", ::GetLastError());
  }
  else {
    CloseHandle(hDevice);
  }
  */
}

void SimulateSendInput()
{

    auto captureThread = []() {
      while (1)
      {
        _DoSendInput2();
        //FoucsCapture2();
        ::Sleep(5000);
      }

    };

    std::thread t1(captureThread);
    t1.join();
  
}
