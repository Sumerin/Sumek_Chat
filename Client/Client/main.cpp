//CLIENT


#include "Client.h"



#define PORT 1111
#define SERVER_ADDRESS "127.0.0.1"


using namespace std;









int main()
{
	
	Client *A = new Client(SERVER_ADDRESS,PORT);

	

	while (A!=NULL)
	{
		A->clientSentMessage();
	}
	

	


	delete A;
	system("pause");
	return 0;
}