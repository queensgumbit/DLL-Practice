#include <windows.h> //acces to windows api's
#include <stdio.h>

int main(int argc, char *argv[])
{   //argv[] is array that hold all the things you typed
   //argument count -things you typed after the program name
    if (argc < 3) { //checking if the user wrote exactly two extra arguments(ProcessID&DLL)
        printf("Usage: injector.exe <PID> <FullDLLPath>\n");
        return 0;
    }


    DWORD pid = (DWORD)atoi(argv[1]); // /Dword - double word (32bit.grabing the first argument in the commandline (except the exe)
    const char *dllPath = argv[2];     // getting the DLL - located second after the sargument
    size_t dllPathLen = strlen(dllPath) + 1; //length of teh DLL path,adds +1 , all strings must end with a special invisible byte: the null terminator (\0).
     //ask john about it



    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    //opens a pointer to the target process, we have full premisssions(r,w ect), 
    //false is -  to unable child process to inherit the handle
    if (hProcess == NULL) {
        printf("Failed to open target process (%u)\n",GetLastError());//theres nothing in the handle
        return 1;
    }




    // Allocate memory inside the target process for the DLL path
    LPVOID pRemotePath = VirtualAllocEx(hProcess, NULL, 1 << 12 , MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); //asks the target process to give a chunk of memory big enough to
    // hold the DLL. 1 <<12 is 4kb the memory we need to allocate a single page. THE mem_commit| MEM_RESERVE-ASKS FOR :Reserve address space AND actually usable RAM
    if (pRemotePath == NULL) {         //IF MEMORYALLOC FAILED:
        printf("Failed to allocate memory in target process\n");
        CloseHandle(hProcess); //close the handle
        return 1;
    }




    // Write the DLL path into the allocated memory
    if (!WriteProcessMemory(hProcess, pRemotePath, argv[2], dllPathLen, NULL)) { // writes the DLL path to the memory i allocated.argv[2] is the argument after the pid ans is the path.
        printf("Failed to write to target process memory\n"); //if not:print error,free space&close hande
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess); //closes proccess handle
        return 1;
    }

    // Get address of LoadLibraryA
    LPVOID pLoadLibraryA = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"); //gets the actual memory adress of LOadLIbararyA(build in funcv rthat loads DLL)
    if (!pLoadLibraryA) {
        printf("Failed to find load libararya\n"); //if didnt get an actual adreess
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    // Create a remote thread in the target process to call LoadLibraryA(DLLPath), the injection part
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,(LPTHREAD_START_ROUTINE ) GetProcAddress(GetModuleHandle(L"kernel.32.dll"),"LoadLibararyA") ,
    pRemotePath, 0, NULL);

    if (!hThread) {
        printf("Failed to create remote thread\n");
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    printf("DLL injected successfully \n");

    // clening up
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return 0;
}
