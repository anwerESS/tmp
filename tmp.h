#include <iostream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

int executeCommand(const std::string& command, std::string& output) {
#ifdef _WIN32
    // Create a pipe for command output
    HANDLE pipeRead, pipeWrite;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&pipeRead, &pipeWrite, &saAttr, 0)) {
        return -1;  // Failed to create pipe
    }

    // Set up STDOUT to write to the pipe
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.hStdOutput = pipeWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;

    // Execute the command
    if (!CreateProcess(NULL, const_cast<char*>(command.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pipeRead);
        CloseHandle(pipeWrite);
        return -1;  // Failed to execute the command
    }

    // Close the write end of the pipe
    CloseHandle(pipeWrite);

    // Read the command output from the pipe
    DWORD bytesRead;
    const int bufferSize = 4096;
    char buffer[bufferSize];

    while (true) {
        if (!ReadFile(pipeRead, buffer, bufferSize - 1, &bytesRead, NULL) || bytesRead == 0) {
            break;  // No more data or read error
        }

        buffer[bytesRead] = '\0';
        output += buffer;
    }

    // Close the read end of the pipe
    CloseHandle(pipeRead);

    // Get the exit code of the process
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return static_cast<int>(exitCode);

#else  // For non-Windows platforms (Linux, macOS, etc.)

    // Execute the command and capture the output using popen()
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return -1;  // Failed to execute the command
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        output += buffer;
    }

    int exitCode = pclose(pipe);

    return WIFEXITED(exitCode) ? WEXITSTATUS(exitCode) : -1;

#endif
}

int main() {
    std::string command = "ls";  // Replace with your desired command

    std::string output;
    int exitCode = executeCommand(command, output);

    std::cout << "Exit code: " << exitCode << std::endl;
    std::cout << "Command output:\n" << output << std::endl;

    return 0;
}
