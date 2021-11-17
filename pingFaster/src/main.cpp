#include "Pinger.h"

using namespace std;


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
		Sleep(5000);
		return 0;
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

	Pinger pinger = Pinger(argv[1], argv[2], argv[3]);

	printf("And of course: %s\n", argv[3]);



	// calcul du temps passé
	chrono::milliseconds t1 = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
	t1 -= t0;


	// affichage du temps passé
	printf("Finished, time spent: %f s\n", (float)t1.count()/1000.0);


	// nettoyage


#ifdef WIN32
	WSACleanup();
#endif // WIN32

	return 0;
}