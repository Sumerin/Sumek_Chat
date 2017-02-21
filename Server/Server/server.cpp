#include "server.h"


	bool Room::send_packet(Packet packetType, const char * data, integer size, const SOCKET dest)
	{

		integer sentedByte = 0;
		integer sizeBuffer;
		Packet packetTypeBuffer;

		sizeBuffer        =     htons(size);
		packetTypeBuffer  =     (Packet)htons((integer)packetType);

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


    void * start_thread(void * arg) // function used with create_thread to make new thread for another client
    {
        pass_arg *temp  =  (pass_arg*)arg;

        temp->dest->income_msg(temp->number, temp);

    }






	Room::Room(integer number)
	{
		RoomIndex = number;
	}







	void Room::join(SOCKET newConnetcion)
	{

		integer size    =   MOTD.size();


		if (numberOfClient == MAX_SIZE)//TODO : fix it
		{
			size = Reject_1.size();

			send_packet(Packet::Sumek_ChatMessage, Reject_1.c_str(), size, newConnetcion);

			return;
		}

		client[numberOfClient].Connection   =   newConnetcion;
		client[numberOfClient].Alive        =   true;




        pass_arg    *temp   =   new pass_arg();

        temp->number =   numberOfClient++;
        temp->dest   =   this;

		send_packet(Packet::Sumek_ChatMessage, MOTD.c_str(), size, newConnetcion);

        pthread_t temp_thread;

        integer err  =  pthread_create(&temp_thread, NULL, &start_thread, temp);

        if (err != 0)
        {

            cout<< "can't create thread :" << strerror(err) << endl;
            check_errno(-1);

        }
        else
        {

            cout<< "Thread created successfully" << endl;
        }
	}





	void Room::income_msg(integer num, pass_arg *temp)
	{
        delete  temp;

		integer iResult;
        char    *buffer;
        integer sizeBuffer;
        integer size;
        integer allsize;
        integer nicknameSize;
        Packet  packetType;
        Packet  packetTypeBuffer;
        integer code ;

		while (client[num].Alive)
		{


			nicknameSize  =  client[num].nickname.size();

			iResult       =  recv(client[num].Connection, (char *)&packetTypeBuffer, sizeof(Packet), NULL);

			packetType    =  (Packet)ntohs((integer)packetTypeBuffer);



			if (iResult <= 0 || !client[num].Alive)
			{
				packetType = Packet::Sumek_Close;
			}



			switch (packetType)
			{


				case Packet::Sumek_ChatMessage: // reciveed message and pass it to others
				{
											  recv(client[num].Connection, (char *)&sizeBuffer, sizeof(integer), NULL);

											  size   =   ntohs(sizeBuffer);
											  
											  allsize   =  size + nicknameSize + 2;
											  buffer =   new char[size + 1];

											  memcpy(buffer, client[num].nickname.c_str(), nicknameSize); // copy nickname to buf

											  buffer[nicknameSize]      = ':';
											  buffer[nicknameSize + 1]  = ' ';
                                              buffer[allsize]           = '\0';

											  recv(client[num].Connection, buffer + nicknameSize + 2, size, NULL);



											  cout << num << ": " << buffer << std::endl;

											  for (int i = numberOfClient - 1; i >= 0; i--)
											  {
												  if (i == num || !client[i].Alive)
												  {
													  continue;
												  }

												  send_packet(Packet::Sumek_ChatMessage, buffer, allsize, client[i].Connection);

                                                  cout << i << " wyslano " << buffer << std::endl;
											  }

											  delete buffer;
				}
				break;


				case Packet::Sumek_Command: //recived command and answear
				{

											 code = 0;

											 recv(client[num].Connection, (char *)&sizeBuffer, sizeof(integer), NULL);

											 size = ntohs(sizeBuffer);

											 buffer       =  new char[size+1];
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
											client[num].Alive   =   false;
											size                =   nicknameSize + sizeof(": Disconected!!");
											buffer              =   new char[size + 1];
                                            buffer[size] = '\0';
											memcpy(buffer, client[num].nickname.c_str(), nicknameSize);                   // copy nickname to buffer
											memcpy(buffer + nicknameSize, ": Disconected!!", sizeof(": Disconected!!"));  // copy text to buffer




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

											close(client[num].Connection);

                                            pthread_exit(0);
				}
				break;
			}
			
		}
	}


void check_errno(integer err)       // checking errno and switching off server
{
    if(err == -1)
    {
        cout<< " error occures: "<< strerror(errno)<< endl;
        exit(1);
    }

}



