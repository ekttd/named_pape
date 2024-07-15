#include <windows.h>
#include <iostream>
#include <string>

void printMenu() {
    std::cout << "1. Create Named Pipe\n";
    std::cout << "2. Connect to Named Pipe\n";
    std::cout << "3. Write to Named Pipe\n";
    std::cout << "4. Disconnect Named Pipe\n";
    std::cout << "5. Exit\n";
    std::cout << "Enter your choice: ";
}

HANDLE hPipe;
HANDLE hEvent;

void createNamedPipe() {
    hPipe = CreateNamedPipe(
        L"\\\\.\\pipe\\lab4_2",
        PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        0,
        0,
        0,
        nullptr
    );
    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe failed: " << GetLastError() << std::endl;
        return;
    }
    std::cout << "Named Pipe created successfully.\n";
}

void connectNamedPipe() {
    BOOL connected = ConnectNamedPipe(hPipe, nullptr) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!connected) {
        std::cerr << "ConnectNamedPipe failed: " << GetLastError() << std::endl;
        return;
    }
    std::cout << "Connected to Named Pipe successfully.\n";
}

void writeToNamedPipe() {
    std::string input;
    std::cout << "Enter the data to send: ";
    std::cin.ignore();
    std::getline(std::cin, input);

    OVERLAPPED overlapped = { 0 };
    hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (hEvent == nullptr) {
        std::cerr << "CreateEvent failed: " << GetLastError() << std::endl;
        return;
    }
    overlapped.hEvent = hEvent;

    DWORD bytesWritten;
    BOOL result = WriteFile(hPipe, input.c_str(), input.size(), &bytesWritten, &overlapped);
    if (!result && GetLastError() != ERROR_IO_PENDING) {
        std::cerr << "WriteFile failed: " << GetLastError() << std::endl;
        return;
    }

    DWORD waitResult = WaitForSingleObject(hEvent, INFINITE);
    if (waitResult == WAIT_OBJECT_0) {
        std::cout << "Data written successfully.\n";
    }
    else {
        std::cerr << "WaitForSingleObject failed: " << GetLastError() << std::endl;
    }
}

void disconnectNamedPipe() {
    if (!DisconnectNamedPipe(hPipe)) {
        std::cerr << "DisconnectNamedPipe failed: " << GetLastError() << std::endl;
        return;
    }
    std::cout << "Named Pipe disconnected successfully.\n";
}

int main() {
    int choice;
    while (true) {
        printMenu();
        std::cin >> choice;

        switch (choice) {
        case 1:
            createNamedPipe();
            break;
        case 2:
            connectNamedPipe();
            break;
        case 3:
            writeToNamedPipe();
            break;
        case 4:
            disconnectNamedPipe();
            break;
        case 5:
            CloseHandle(hPipe);
            CloseHandle(hEvent);
            return 0;
        default:
            std::cerr << "Invalid choice, please try again.\n";
        }
    }
    return 0;
}
