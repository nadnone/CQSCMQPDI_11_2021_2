#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <thread>
#include <string.h>
#include <string>
#include <vector>
#include <chrono>

#ifdef _WIN32
#include <ws2tcpip.h>
#include <winsock2.h>

// Je n'ai aucune idée de si ce program fonctionne sous Linux ou MacOS
#else
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#endif

using namespace std;

class Pinger
{
public:
	Pinger(string ip_start, string ip_end, string local_ip);
	~Pinger();

private:
	void pingthread(in_addr ip_in);

};
