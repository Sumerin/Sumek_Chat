//SERVER

#include "server.h"
#define BROADCAST_ADDRESS "127.0.0.1"


#define PORT 1111



/*
*  Room:
*		- 1 Thread do obs³ugi join leave dynamiczny
*		- 1 Thread do obs³ugi ca³ego income
*		- (opcjonalnie) 1 Thread do obs³ugi outcome
*
*
*
*
*
*
*
*
*
*
*/





int main()
{

	Room *beta_test=new Room(1);


	SOCKADDR_IN my_addr; // address socketu do obslugi kolejki polaczen
	integer addrlen = sizeof(my_addr);
	my_addr.sin_addr.s_addr = inet_addr(BROADCAST_ADDRESS);// inet_pton
	my_addr.sin_port = htons(PORT);// port
	my_addr.sin_family = AF_INET;//ipv4


	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); // przypisanie addresu do soketa
	bind(sListen, (SOCKADDR*)&my_addr, addrlen);
	listen(sListen, SOMAXCONN);


	SOCKET newConnection;
	integer i = 0;
	while (true)
	{

		newConnection = accept(sListen, (SOCKADDR*)&my_addr, &addrlen); // akceptacja polaczenia i rpzylaczenie do pokoju
		if (newConnection == 0)
		{
			std::cout << "Failed to accept the client's connection" << std::endl;
		}
		else
		{
			std::cout << "Connected " << ++i <<std::endl;

			beta_test->join(newConnection);

		}
	}
	system("pause");

	return 0;
}