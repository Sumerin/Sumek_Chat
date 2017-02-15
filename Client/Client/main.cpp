//CLIENT


#include "Client.h"

#define PORT 1111



using namespace std;









int main()
{
	
	Client *A;
	string ip;

	string sBuffer;

	do
	{
		cout << "IP: ";
		getline(cin, ip);

	} while (!ipPattern(ip));

	cout << "what have i done" << endl;
	A = new Client( ip, PORT);
	while (A->getAlive())
	{
		A->clientSentMessage(sBuffer);
		getline(cin, sBuffer);
		
	}
	

	
	delete A;

	return 0;
}
