//CLIENT


#include "Client.h"



#define PORT 1111
#define SERVER_ADDRESS "127.0.0.1"


using namespace std;









int main()
{
	
	Client *A = new Client(SERVER_ADDRESS,PORT);

	string sBuffer;

	while (A->getAlive())
	{
		A->clientSentMessage(sBuffer);
		getline(cin, sBuffer);
		
	}
	

	

	system("pause");
	return 0;
}