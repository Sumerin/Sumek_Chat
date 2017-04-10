#include "server.h"

bool WSAData_run = false; // be sure not to start wsData twice

	bool Room::send_packet(Packet packetType, const char * data, integer size, const SOCKET dest)
	{

		integer sentedByte = 0;
		integer sizeBuffer;
		Packet packetTypeBuffer;

		sizeBuffer = htons(size);
		packetTypeBuffer = (Packet)htons((integer)packetType);

		switch (packetType)
		{
			case Packet::Sumek_ChatMessage:
			{
										  while (sizeof(Packet)-sentedByte > 0) //package Type
										  {
											  sentedByte += send(dest, (char*)&packetTypeBuffer + sentedByte, sizeof(Packet)-sentedByte, NULL);
										  }

										  sentedByte = 0;

										  while (sizeof(integer)-sentedByte > 0) // size of package
										  {
											  sentedByte += send(dest, (char*)&sizeBuffer + sentedByte, sizeof(integer)-sentedByte, NULL);
										  }

										  sentedByte = 0;

										  while (size - sentedByte > 0) // the message
										  {
											  sentedByte += send(dest, data + sentedByte, size - sentedByte, NULL);
										  }
			}
			break;



			case Packet::Sumek_Command:

				while (sizeof(Packet)-sentedByte > 0) // package type
				{
					sentedByte += send(dest, (char*)&packetTypeBuffer + sentedByte, sizeof(Packet)-sentedByte, NULL);
				}

				sentedByte = 0;

				while (sizeof(integer)-sentedByte > 0) // size of answered message
				{
					sentedByte += send(dest, (char*)&sizeBuffer + sentedByte, sizeof(integer)-sentedByte, NULL);
				}


				sentedByte = 0;

				while (size - sentedByte > 0) // the message
				{
					sentedByte += send(dest, data + sentedByte, size - sentedByte, NULL);
				}
			
			break;

		default:

		break;

		}



	return true;


	}






	void  start_thread(void * arg) // function used with create_thread to make new thread for another client
	{
		pass_arg *temp = (pass_arg*)arg;
		temp->dest->income_msg(temp->number, temp);
	}


	Room::Room(integer number)
	{
		start_WSADATA();
		RoomIndex = number;
	}
	Room::~Room()
	{
		WSACleanup();
	}


	void Room::start_WSADATA()
	{
		if (WSAData_run == false)
		{

			WSADATA wsaData;
			WORD dllVersion = MAKEWORD(2, 1);

			if (WSAStartup(dllVersion, &wsaData) != 0)
			{
				MessageBoxA(NULL, "Startup Failed", "Sumer_chat_Error", MB_OK | MB_ICONERROR);
				exit(1);
			}
		}
		WSAData_run = true;
	}

	void start()
	{
		//TODO: thread obslugujacy wszystkie wiadomsci przychodzace do roomu 
	}





	void Room::join(SOCKET newConnetcion)
	{

		integer size = MOTD.size();
		pass_arg *arg = new pass_arg();

		if (numberOfClient == MAX_SIZE)
		{
			size = Reject_1.size();
			send_packet(Packet::Sumek_ChatMessage, Reject_1.c_str(), size, newConnetcion);
			return;
		}

		client[numberOfClient].Connection = newConnetcion;
		client[numberOfClient].Alive = true;
		
		send_packet(Packet::Sumek_ChatMessage, MOTD.c_str(), size, newConnetcion);

		arg->dest = this;
		arg->number = numberOfClient++;

		

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)start_thread, (LPVOID)arg, NULL, NULL);
	}





	void Room::income_msg(integer num, pass_arg* relase)
	{
		delete relase;
		int iResult = 1;
		while (client[num].Alive)
		{
			char *buffer;
			integer sizeBuffer;
			integer size;
			integer nicknameSize;
			Packet packetType;
			Packet packetTypeBuffer;

			nicknameSize = client[num].nickname.size();

			iResult = recv(client[num].Connection, (char *)&packetTypeBuffer, sizeof(Packet), NULL);

			packetType = (Packet)ntohs((integer)packetTypeBuffer);

			if (iResult < 0 || !client[num].Alive)
			{
				packetType = Packet::Sumek_Close;
			}



			switch (packetType)
			{


				case Packet::Sumek_ChatMessage: // reciveed message and pass it to others
				{
											  recv(client[num].Connection, (char *)&sizeBuffer, sizeof(integer), NULL);

											  size = ntohs(sizeBuffer);
											  
											  size += nicknameSize + 2;
											  buffer = new char[size + 1];

											  memcpy(buffer, client[num].nickname.c_str(), nicknameSize);

											  buffer[nicknameSize] = ':';
											  buffer[nicknameSize + 1] = ' ';

											  recv(client[num].Connection, buffer + nicknameSize + 2, size, NULL);
											  buffer[size] = '\0';
											  cout << num << ": " << buffer << std::endl;

											  for (int i = numberOfClient - 1; i >= 0; i--)
											  {
												  if (i == num || !client[i].Alive)
												  {
													  continue;
												  }
												  client_mutex[i].lock();
												  send_packet(Packet::Sumek_ChatMessage, buffer, size, client[i].Connection);
												  client_mutex[i].unlock();
											  }

											  delete buffer;
				}
				break;


				case Packet::Sumek_Command: //recived command and answear
				{

											 integer code = 0;;

											 recv(client[num].Connection, (char *)&sizeBuffer, sizeof(integer), NULL);

											 size = ntohs(sizeBuffer);

											 buffer = new char[size+1];
											 buffer[size] = '\0';

											 recv(client[num].Connection, buffer, size, NULL);




										  for (int i = 0; i < 2; i++) // 2 is the number of characters per command // !!! TODO: 10 + 8 = 9 + 9 BAD IDEA !!
										  {
											  code += buffer[i]>'a' - 1 ? buffer[i] - ('a' - 'A') : buffer[i];    //  if it's lowercase convert to upper and add else just add 
										  }




												





										  switch (code)
										  {
												
											case ('M' + 'E') : // who am i?
											{
																 send_packet(Packet::Sumek_Command,client[num].nickname.c_str(), nicknameSize , client[num].Connection);
											}
											break;

											case ('C' + 'H') : // Change my nickname
											{
																   client[num].nickname= string( buffer + 3);
											}
											break;
										  }



										  delete[] buffer;

				}
				break;


				case Packet::Sumek_Close:
				{
											client[num].Alive = false;
											nicknameSize = client[num].nickname.size();
											size = nicknameSize + sizeof(": Disconected!!");
											buffer = new char[size + 1];

											memcpy(buffer, client[num].nickname.c_str(), nicknameSize);
											memcpy(buffer + nicknameSize, ": Disconected!!", sizeof(": Disconected!!"));



											buffer[size] = '\0';
											cout << num << ": " << buffer << std::endl;

											for (int i = numberOfClient - 1; i >= 0; i--)
											{
												if (i == num || !client[i].Alive)
												{
													continue;
												}

												send_packet(Packet::Sumek_ChatMessage, buffer, size, client[i].Connection);
											}

											delete[] buffer;

											closesocket(client[num].Connection);
				}
				break;
			}
			
		}
	}






