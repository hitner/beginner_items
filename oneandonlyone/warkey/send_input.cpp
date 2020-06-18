#include "stdafx.h"
#include "send_input.h"
#include <thread>


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

void _DoSendInput2()
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

void SimulateSendInput()
{

    auto captureThread = []() {
      while (1)
      {
        _DoSendInput();
        //FoucsCapture2();
        ::Sleep(5000);
      }

    };

    std::thread t1(captureThread);
    t1.join();
  
}
