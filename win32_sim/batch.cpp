#include <iostream>
#include <windows.h>

void ExecuteBatchFileInNewWindow(const char *batchFile)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create command line string, using cmd.exe /c to execute the batch file
    std::string command = "cmd.exe /c "/*"cmd.exe /k "*/;
    command += batchFile;

    // Set window flags to hide the window
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
// Set window flags to show the new window
    // si.dwFlags |= STARTF_USESHOWWINDOW;
    // si.wShowWindow = SW_SHOWNORMAL;
    if (CreateProcess(NULL, const_cast<char *>(command.c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                      NULL, NULL, &si, &pi))
    {
        std::cout << batchFile << " is running." << std::endl;

        // No longer wait for the process to end
        // Close handles to avoid resource leaks
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        std::cerr << "Failed to create process (" << GetLastError() << ") for " << batchFile << std::endl;
    }
}

extern "C" {
    int ExecuteBatchFile()
    {
        ExecuteBatchFileInNewWindow("realgui\\example\\web\\tree\\run_server.bat");
        std::cout << std::endl << "TREE: http://localhost:8000/realgui/example/web/tree/" << std::endl <<
                  std::endl;
        return 0;
    }
}
