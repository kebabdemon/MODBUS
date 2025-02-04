#ifndef LISTEN_WRITE_H
#define LISTEN_WRITE_H

#include <windows.h>
#include <stdbool.h>


bool write_to_port(HANDLE hComm, const char *command, DWORD commandSize, GtkTextBuffer *buffer);
bool read_from_port(HANDLE hComm, char *response, DWORD responseSize, GtkTextBuffer *buffer);
void listen_write(const char *portName, const char *command, DWORD commandSize, int BAUS, GtkTextBuffer *buffer);

#endif

