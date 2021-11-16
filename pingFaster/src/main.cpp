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




void pingthread(in_addr ip_in)//, FILE* file)
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
		//fwrite(ip, sizeof(char), 16, file);
		//fwrite("\n", sizeof(char), 1, file);

		printf("Connexion success %s\n", ip);

	}

	// fermeture de connexion
	closesocket(s);
	
	return;
}


int pingDispatcher(string ip_start, string ip_end, string local_ip)
{
	in_addr nb_s, nb_e, nb_bypass;

	vector<thread> threads;

	// conversion 
	inet_pton(AF_INET, ip_start.c_str(), &nb_s);
	inet_pton(AF_INET, ip_end.c_str(), &nb_e);
	inet_pton(AF_INET, local_ip.c_str(), &nb_bypass);

	// ouverture du fichier de data (useless)
	/*
	FILE* file;
	fopen_s(&file, "ips_list.txt", "w");
	if (file == NULL)
	{
		printf("Error creating file ips_list.txt\n");
		return 1;

	}
	*/

	int plages_ip = ntohl(nb_e.S_un.S_addr);//- ntohl(nb_s.S_un.S_addr);
	int ip_nb_bypass = ntohl(nb_bypass.S_un.S_addr);

	for (int i = ntohl(nb_s.S_un.S_addr); i <= plages_ip; i++) // on arrête si nb_ip_start <= que nb_ip_end 
	{
	
		if (i != ip_nb_bypass)
		{
			threads.push_back(thread(pingthread, nb_s));//, file));
		}

		nb_s.S_un.S_addr = htonl(ntohl(nb_s.S_un.S_addr) + 1); // on incrémente l'addresse ip de 1
	
	}
	

	Sleep(200);
	// tout ce qui dépasse 50 ms est annulé 
	for (int i = 0; i < threads.size(); i++)
	{
		threads[i].detach();
	}




	//fclose(file);
	return 0;
}










int main(int argc, char* argv[])
{
	// initialisation

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


	// garde fou

	if (argc < 4) 
	{
		printf("./pingFaster.exe <ip_start> <ip_end> <your_ip>\n");
	}
	else
	{
		while (true)
		{
			char reply = NULL;
			printf("Are you sur ? [y/n]\n");
			cin >> reply;

			if (reply == 'y')
			{
				break;
			}
			else if (reply == 'n')
			{
				return 0;
			}

		}



	}


	// lancement du scann

	chrono::milliseconds t0 = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());


	int rval = pingDispatcher(argv[1], argv[2], argv[3]);

	if (rval)
	{
		return 1;
	}

	printf("And of course: %s\n", argv[3]);

	// calcul du temps passé

	chrono::milliseconds t1 = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());

	t1 -= t0;


	printf("Finished, time spent: %f s\n", (float)t1.count()/1000.0);







#ifdef WIN32
	WSACleanup();
#endif // WIN32

	return 0;
}