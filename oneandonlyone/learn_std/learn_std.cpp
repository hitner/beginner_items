// learn_std.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>

int main()
{
    std::cout << "Hello World!\n";

    size_t pid_list_buffer_size = 256;
    DWORD return_length = 0;

    PJOBOBJECT_BASIC_PROCESS_ID_LIST p_hpid_list = (PJOBOBJECT_BASIC_PROCESS_ID_LIST)std::calloc(1, pid_list_buffer_size);
    BOOL ret = ::QueryInformationJobObject(
      NULL,
      JobObjectBasicProcessIdList,
      p_hpid_list,
      (DWORD)pid_list_buffer_size,
      &return_length);

    std::cout << "Hello World: " << return_length << std::endl;

    for (SSIZE_T idx = 0; idx < (SSIZE_T)p_hpid_list->NumberOfProcessIdsInList; idx++)
    {
      ULONG pid = p_hpid_list->ProcessIdList[idx];
      
    }
   
    return 0;
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
