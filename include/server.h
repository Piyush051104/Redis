#pragma once
#include <winsock2.h>

void startServer(int port);
void handleClient(SOCKET clientFd);