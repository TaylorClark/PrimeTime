#include <iostream>
#include <winsock2.h>
#include <string>
#include "Network/SocketMgr.h"

using namespace std;


int main( int, char* )
{
	SocketMgr& socketMgr = SocketMgr::Get();
	
	if( !socketMgr.Init() )
	{
		cout << "Failed to intiailize winsock." << endl;
		return 1;
	}

	cout << "Your IP :" << socketMgr.GetLocalIP().ToString() << endl;

	// Check if we will listen or connect
	char choice;
	cout << "Listen (l), connect (c), broadcast send (b), or broadcast receive (r) (any other entry to quit):";
	cin >> choice;

	if( choice == 'l' )
	{
		NetConnection conn = socketMgr.ListenTCPSync( 15500, 15550 );
		if( conn.IsConnected() )
		{
			NetMessage msg;
			conn.ReceiveDataWait( msg );
			conn.Close();
		}
	}
	else if( choice == 'c' )
	{
		// Retrieve the IP address
		cout << "Enter the IP to attempt to connect in dotted format ex. 192.168.1.1 (-1 to quit) :";
		std::string sIP;
		cin >> sIP;
		IPAddress foreignIP( sIP.c_str() );
		
		// Retrieve the port number
		cout << "Enter the port to attempt to connect (-1 to quit) :";
		int port = 0;
		cin >> port;
		
		if( foreignIP.GetVal() != 0 && port != -1 )
		{
			NetConnection conn = socketMgr.ConnectToTCPSync( foreignIP, (uint16)port );
			if( conn.IsConnected() )
			{
				conn.SendData( NetMessage() );
				Sleep( 2000 );
			}
		}
	}
	else if( choice == 'r' )
	{
		// Retrieve the port number
		cout << "Enter the port to listen on (-1 to quit) :";
		int port = 0;
		cin >> port;
		if( port != -1 )
		{
			NetConnectionBroadcast conn = socketMgr.CreateLANBroadcastListener( (uint16)port );
			if( conn.IsConnected() )
			{
				NetMessage msg;
				int waitCount = 0;
				while( 1 )
				{
					if( conn.ReceiveData( msg ) )
						break;
					cout << "No data, waiting (" << waitCount << ")..." << endl;
					++waitCount;

					Sleep( 500 );
				}
				conn.Close();
			}
		}
	}
	else if( choice == 'b' )
	{
		// Retrieve the port number
		cout << "Enter the port to send to (-1 to quit) :";
		int port = 0;
		cin >> port;
		if( port != -1 )
		{
			NetConnectionBroadcast conn = socketMgr.CreateLANBroadcastSender( port );
			if( conn.IsConnected() )
			{
				conn.SendData( NetMessage() );
				Sleep( 2000 );
			}
		}
	}

	cin >> choice;

	// Free the Winsock data
	WSACleanup();

	return 0;
}
