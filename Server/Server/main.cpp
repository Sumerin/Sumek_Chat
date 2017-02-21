//SERVER

#include "server.h"
#define BROADCAST_ADDRESS "127.0.0.1"


#define PORT 1111




int main()
{



	Room *beta_test		=	new Room(1);


	sockaddr_in my_addr; 		// address socketu do obslugi kolejki polaczen
	unsigned int addrlen 	= 	sizeof(my_addr);
	my_addr.sin_addr.s_addr = 	inet_addr(BROADCAST_ADDRESS);
	my_addr.sin_port 		= 	htons(PORT);// port
	my_addr.sin_family 		= 	AF_INET;//ipv4


	SOCKET sListen 	= 	socket(AF_INET, SOCK_STREAM, NULL); // przypisanie addresu do soketa
    check_errno(sListen);

    integer binding = 	bind(sListen, (sockaddr*)&my_addr, addrlen);
    check_errno(binding);



	listen(sListen, SOMAXCONN);


	SOCKET newConnection;

	integer i = 0;

	while (true)
	{

		newConnection 	= 	accept(sListen, (sockaddr*)&my_addr, &addrlen); // akceptacja polaczenia i rpzylaczenie do pokoju

		if (newConnection == -1)
		{

			std::cout << "Failed to accept the client's connection" << strerror(errno) <<endl;
		}

		else
		{
			std::cout << "Connected " << ++i <<std::endl;

			beta_test -> join(newConnection);

		}
	}
	system("pause");

	return 0;
}