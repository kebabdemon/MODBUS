#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "../lib/port.h"

#define MAX_PORTS 20


bool scan_ports(const char *portName, char *portInfo) {
    int HART_BAUD_RATE = atoi(BAUS);
    HANDLE hComm;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    DWORD bytesWritten, bytesRead;
    char testCommand[] = {0x01, 0x00};
    char response[256];

    hComm = CreateFile(portName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);

    if (hComm == INVALID_HANDLE_VALUE) {
        return false;
    }

    // Configure serial port
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hComm, &dcbSerialParams)) {
        CloseHandle(hComm);
        return false;
    }

    dcbSerialParams.BaudRate = HART_BAUD_RATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.Parity = EVENPARITY;
    dcbSerialParams.StopBits = ONESTOPBIT;
    if (!SetCommState(hComm, &dcbSerialParams)) {
        CloseHandle(hComm);
        return false;
    }

    // Set timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hComm, &timeouts);

    // Send test command
    if (!WriteFile(hComm, testCommand, sizeof(testCommand), &bytesWritten, NULL)) {
        CloseHandle(hComm);
        return false;
    }

    // Read response
    if (ReadFile(hComm, response, sizeof(response), &bytesRead, NULL) && bytesRead > 0) {
        // Successfully read
        snprintf(portInfo, 256, "Port: %s, Baud: %d, Parity: EVEN", portName, HART_BAUD_RATE);
        CloseHandle(hComm);
        return true; // Valid response
    }

    CloseHandle(hComm);
    return false;
}

char *find_psu_com() {
    static char detectedPort[10];

    for (int i = 1; i <= MAX_PORTS; ++i) {
        char portName[10];
        snprintf(portName, sizeof(portName), "\\\\.\\COM%d", i);

        if (scan_ports(portName, detectedPort)) {
            snprintf(detectedPort, sizeof(detectedPort), "\\\\.\\COM%d", i);
            return detectedPort;
        }
    }

    return NULL; // No PSU detected
}
