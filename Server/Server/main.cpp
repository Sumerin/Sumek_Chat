//SERVER

#include "server.h"
#define BROADCAST_ADDRESS "127.0.0.1"

#define errno WSAGetLastError()
#define PORT 1111


void check_errno(int err)
{
	if (err < 0)
	{
		throw errno;
	}

}




int main()
{

	Room *beta_test=new Room(1);
	int result;

	SOCKADDR_IN my_addr; // address socketu do obslugi kolejki polaczen
	int addrlen = sizeof(my_addr);
	my_addr.sin_addr.s_addr = htons(INADDR_ANY);// inet_pton
	my_addr.sin_port = htons(PORT);// port
	my_addr.sin_family = AF_INET;//ipv4

	try
	{

		SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); // przypisanie addresu do soketa
		check_errno(sListen);

		result = bind(sListen, (SOCKADDR*)&my_addr, addrlen);
		check_errno(result);

		result = listen(sListen, SOMAXCONN);
		check_errno(result);

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
				std::cout << "Connected " << ++i << std::endl;

				beta_test->join(newConnection);

			}
		}
	}
	catch (int error)
	{
		cout << "WSA Error: " << error << " check it on https://msdn.microsoft.com/en-us/library/windows/desktop/ms740668(v=vs.85).aspx" << endl;
	}
	delete beta_test;
	system("pause");

	return 0;
}