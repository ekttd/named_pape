#include <windows.h>
#include <iostream>
#include <string>

void printMenu() {
    std::cout << "1. Connect to Named Pipe\n";
    std::cout << "2. Read from Named Pipe\n";
    std::cout << "3. Exit\n";
    std::cout << "Enter your choice: ";
}

HANDLE hPipe;

void readCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped) {
    if (dwErrorCode == 0) {
        char* buffer = (char*)lpOverlapped->hEvent;
        if (dwNumberOfBytesTransfered < 128) { 
            //buffer[dwNumberOfBytesTransfered] = '\0';
            std::cout << "Read bytes from Named Pipe: " << buffer << std::endl;
        }
        else {
            std::cerr << "Read bytes from Named Pipe exceed buffer size" << std::endl;
        }
    }
    else {
        std::cerr << "ReadFileEx failed with error code: " << dwErrorCode << std::endl;
    }
}

void connectToNamedPipe() {
    hPipe = CreateFile(
        L"\\\\.\\pipe\\lab4_2",
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        nullptr
    );
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFile failed: " << GetLastError() << std::endl;
        return;
    }
    std::cout << "Connected to Named Pipe successfully.\n";
}

void readFromNamedPipe() {
    char buffer[128];
    OVERLAPPED overlapped = { 0 };
    overlapped.hEvent = overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (overlapped.hEvent == NULL) {
        std::cerr << "CreateEvent failed: " << GetLastError() << std::endl;
        return;
    }

    BOOL result = ReadFileEx(hPipe, buffer, sizeof(buffer), &overlapped, readCompletionRoutine);
    if (!result) {
        std::cerr << "ReadFileEx failed: " << GetLastError() << std::endl;
        return;
    }

    SleepEx(INFINITE, TRUE);  
}



int main() {
    int choice;
    while (true) {
        printMenu();
        std::cin >> choice;

        switch (choice) {
        case 1:
            connectToNamedPipe();
            break;
        case 2:
            readFromNamedPipe();
            break;
        case 3:
            CloseHandle(hPipe);
            return 0;
        default:
            std::cerr << "Invalid choice, please try again.\n";
        }
    }
    return 0;
}
