#include <stdio.h>
#include <thread>
#include <string.h>
#include <string>
#include <vector>
#include <chrono>
#include <future>

#ifdef _WIN32
#include <ws2tcpip.h>
#include <winsock2.h>
#include <iphlpapi.h>

#else
#include <arpa/inet.h>
#include <ifaddrs.h
#endif

using namespace std;


void pingthread(in_addr ip_in, FILE* file)
{

	// ouverture de connexion 

	// conversion in_addr en string
	char ip[16] = "";
	inet_ntop(AF_INET, &ip_in.S_un.S_addr, ip, sizeof(char) * 16);


	// creation de la socket
	int s = socket(PF_INET, SOCK_RAW, 1);

	if (s == INVALID_SOCKET)
	{
		printf("Error socket creation\n");
		printf("%i\n", GetLastError());
		return;
	}


	// structure ICMP header
	typedef struct { 
		uint8_t type;
		uint8_t code;
		uint16_t chksum;
		uint32_t data;
	} icmp_pckt;


	// set the ping header
	icmp_pckt pckt;
	pckt.type = 8;
	pckt.code = 0;
	pckt.chksum = 0xfff7;
	pckt.data = 0;

	// create ip header
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.S_un.S_addr = ip_in.S_un.S_addr;

	// send the packet
	int aSendRslt = sendto(s, (char*)&pckt, sizeof(pckt), 0, (struct sockaddr*)&addr, sizeof(addr));

	if (aSendRslt == INVALID_SOCKET)
	{
		// envoie echoué
		printf("Send error: %i\n", aSendRslt);
		return;
	}


	// prepare the response
	char res[30] = "";
	struct sockaddr* ResAddr = NULL;
	unsigned int* res_addr_size = NULL;

	// receive PONG;
	int response = recvfrom(s, res, sizeof(res), 0, ResAddr, (int*)res_addr_size);


	if(response == INVALID_SOCKET)
	{
		//printf("response error: %i\n", response);
		//printf("Error nb: %i\n", GetLastError());
		return;
	}
	else
	{
		//printf("ping recv: %s\n", res);


		// ecriture des resultats
		fwrite(ip, sizeof(char), 16, file);
		fwrite("\n", sizeof(char), 1, file);

		printf("Connexion success %s\n", ip);

	}
	// fermeture de connexion
	
	free(ResAddr);
	free(res_addr_size);

	closesocket(s);
	
	return;
}

int pingDispatcher(string ip_start, string ip_end)
{
	in_addr nb_s, nb_e;

	vector<thread> threads;

	// conversion 
	inet_pton(AF_INET, ip_start.c_str(), &nb_s);
	inet_pton(AF_INET, ip_end.c_str(), &nb_e);

	// ouverture du fichier de data
	FILE* file;
	fopen_s(&file, "ips_list.txt", "w");
	if (file == NULL)
	{
		printf("Error creating file ips_list.txt\n");
		return 1;

	}


	int plages_ip = ntohl(nb_e.S_un.S_addr) - ntohl(nb_s.S_un.S_addr);

	for (int i = 0; i <= plages_ip; i++) // on arrête si nb_ip_start <= que nb_ip_end 
	{
		//printf("%s\n", ip_to_ping);


		// TODO trouver l'addresse ip locale de la machine

	


		//inet_ntop(AF_INET, &h, myip, sizeof(myip));

		//printf("myip: %s\n", myip);

		threads.push_back(thread(pingthread, nb_s, file));

		nb_s.S_un.S_addr = htonl(ntohl(nb_s.S_un.S_addr) + 1); // on incrémente l'addresse ip de 1
	
	}
	


	// tout ce qui dépasse 50 ms est annulé 
	for (int i = 0; i < threads.size(); i++)
	{
		threads[i].detach();
		Sleep(5); // timeout
		threads[i].~thread();
		
	}




	fclose(file);
	return 0;
}










int main(int argc, char* argv[])
{
	// initialisation

	chrono::milliseconds t0 = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

	printf("Initialisation...\n");


#ifdef WIN32

	WSADATA wsaData;

	int rslt = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rslt != 0)
	{
		printf("Initialisation failed: %i\n", rslt);
		exit(0);
	}

#endif // WIN32


	// lancement du scann

	pingDispatcher((string)"192.168.0.1", (string)"192.168.0.247");


	// calcul du temps passé

	chrono::milliseconds t1 = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

	t1 -= t0;

	printf("Finished, time spent: %f s\n", (float)t1.count()/1000.0);







#ifdef WIN32
	WSACleanup();
#endif // WIN32

	return 0;
}