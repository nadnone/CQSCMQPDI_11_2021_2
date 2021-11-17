#include "Pinger.h"

void Pinger::pingthread(in_addr ip_in)//, FILE* file)
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

	if (response == INVALID_SOCKET)
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

Pinger::Pinger(string ip_start, string ip_end, string local_ip)
{
	in_addr nb_s, nb_e, nb_bypass;

	vector<thread> threads;

	// conversion 
	inet_pton(AF_INET, ip_start.c_str(), &nb_s);
	inet_pton(AF_INET, ip_end.c_str(), &nb_e);
	inet_pton(AF_INET, local_ip.c_str(), &nb_bypass);

	int plages_ip = ntohl(nb_e.S_un.S_addr);//- ntohl(nb_s.S_un.S_addr);
	int ip_nb_bypass = ntohl(nb_bypass.S_un.S_addr);


	// lancement des threads
	for (int i = ntohl(nb_s.S_un.S_addr); i <= plages_ip; i++) // on arrête si nb_ip_start <= que nb_ip_end 
	{

		if (i != ip_nb_bypass)
		{
			threads.push_back(thread(&Pinger::pingthread, this, nb_s));//, file));
		}

		nb_s.S_un.S_addr = htonl(ntohl(nb_s.S_un.S_addr) + 1); // on incrémente l'addresse ip de 1

	}

	// tout ce qui dépasse 300 ms est annulé 
	Sleep(300);
	for (int i = 0; i < threads.size(); i++)
	{
		threads[i].detach();
	}




	//fclose(file);
	return;
}

Pinger::~Pinger()
{
}