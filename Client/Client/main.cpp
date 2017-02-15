//CLIENT


#include "Client.h"

#define PORT 1111



using namespace std;









int main()
{
	
	Client *A;
	string IP;

	string sBuffer;

	do
	{
		cout << "IP: ";
		getline(cin, IP);

	} while (!ipPattern(IP));

	cout << "what have i done" << endl;
	A = new Client( IP, PORT);
	while (A->getAlive())
	{
		A->clientSentMessage(sBuffer);
		getline(cin, sBuffer);
		
	}
	

	
	delete A;
	system("pause");
	return 0;
}