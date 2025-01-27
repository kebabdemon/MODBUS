#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "../lib/port.h"
#include "../lib/listen_write.h"
#include "../lib/com.h"
#include <gtk/gtk.h>

// Helper function to convert binary data to a hex string with spaces
void binary_to_hex_string_with_spaces(const unsigned char *data, size_t length, char *hexString) {
    for (size_t i = 0; i < length; ++i) {
        sprintf(hexString + (i * 3), "%02X ", data[i]);  // Add a space after each byte
    }
    hexString[length * 3 - 1] = '\0'; // Replace the last space with a null terminator
}

bool write_to_port(HANDLE hComm, const char *command, DWORD commandSize, GtkTextBuffer *buffer) {
    DWORD bytesWritten;

    // Write the command to the serial port
    if (!WriteFile(hComm, command, commandSize, &bytesWritten, NULL)) {
        print_to_buffer(buffer, "Error writing to serial port\n");
        return false;
    }

    return true;
}

bool read_from_port(HANDLE hComm, char *response, DWORD responseSize, GtkTextBuffer *buffer) {
    DWORD bytesRead;

    // Read the response from the serial port
    if (!ReadFile(hComm, response, responseSize, &bytesRead, NULL) || bytesRead == 0) {
        print_to_buffer(buffer, "Error reading from serial port\n");
        return false;
    }

    return true;
}

void listen_write(const char *portName, const char *command, DWORD commandSize, int BAUS, GtkTextBuffer *buffer) {
    HANDLE hComm;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    unsigned char response[256];
    char hexString[768];  // Buffer to store hex string with spaces (each byte is three characters: two hex digits + one space)
    DWORD bytesRead;

    // Open the serial port
    hComm = CreateFile(portName,
                       GENERIC_READ | GENERIC_WRITE,
                       0, // no sharing
                       NULL, // no security attributes
                       OPEN_EXISTING,
                       0, // non-overlapped I/O
                       NULL); // no template file

    if (hComm == INVALID_HANDLE_VALUE) {
        DWORD errorCode = GetLastError();
        char message[256];
        snprintf(message, sizeof(message), "Error opening serial port %s, Error code: %lu\n", portName, errorCode);
        print_to_buffer(buffer, message);
        return;
    }

    // Configure serial port
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hComm, &dcbSerialParams)) {
        print_to_buffer(buffer, "Error getting serial port state\n");
        CloseHandle(hComm);
        return;
    }

    dcbSerialParams.BaudRate = BAUS;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.Parity = NOPARITY; // Assuming no parity
    dcbSerialParams.StopBits = ONESTOPBIT;
    if (!SetCommState(hComm, &dcbSerialParams)) {
        print_to_buffer(buffer, "Error setting serial port state\n");
        CloseHandle(hComm);
        return;
    }

    // Set timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hComm, &timeouts);

    // Write to the port
    if (!write_to_port(hComm, command, commandSize, buffer)) {
        CloseHandle(hComm);
        return;
    }

    // Read from the port
    if (!ReadFile(hComm, response, sizeof(response) - 1, &bytesRead, NULL) || bytesRead == 0) {
        print_to_buffer(buffer, "Error reading from serial port\n");
        CloseHandle(hComm);
        return;
    }

    // Convert binary response to hex string with spaces
    binary_to_hex_string_with_spaces(response, bytesRead, hexString);

    // Prepare the response message
    char message[512];
    snprintf(message, sizeof(message), "Response from %s: %s\n", portName, hexString);
    print_to_buffer(buffer, message);

    // Close the port
    CloseHandle(hComm);
}
