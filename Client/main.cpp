#include <iostream>
#include <fstream>
#include <winsock2.h>

using namespace std;

int main(int argc, char** argv)
{
	// Inicjalizacja mechanizmu gniazd - takie samo jak w serwerze
	WSADATA wsa;
	
	int siecdziala = WSAStartup( MAKEWORD( 2, 2 ), &wsa );

	if( siecdziala == NO_ERROR )
	{
		cout << "Gniazda dostepne" << endl;
	}
	else
	{
		cout << "Gniazda niedpstepne!" << endl;
		
		WSACleanup();
		
		return 1;
	}

	
	// W³¹czenie gniazda - takie samo jak w serwerze
	SOCKET gniazdo = socket( AF_INET, SOCK_STREAM,
									  IPPROTO_TCP );

	if( gniazdo != INVALID_SOCKET )
	{
		cout << "Gniazdo utworzone" << endl;
	}
	else
	{
		cout << "Nie mozna utworzyc gniazda" << endl;
		
		WSACleanup();
			
		return 2;
	}

	// Wczytanie danych z pliku, ¿eby nie ustawiaæ za ka¿dym razem w kodzie programu
	unsigned short ushPort = 80;
	string 		   strHost = "127.0.0.1";
	
	fstream Plik_cfg;
	
	Plik_cfg.open( "IP_Host_Port.cfg", ios::in );

	if( Plik_cfg.good() == true )
	{
		Plik_cfg >> strHost;
		Plik_cfg >> ushPort;
		
		Plik_cfg.close();
		
		cout << "Wczytano konfiguracje z pliku" << endl;	
	}
	else
	{
		Plik_cfg.close();
		
		cout << "Nie mo¿na wczytaæ ustawieñ z pliku" << endl;
		
		return 10;
	}

	// Ustawienie parametrów gniazda - podobnie jak w serwerze
	sockaddr_in ustawienia;
	
	memset( &ustawienia, 0, sizeof( ustawienia ) );
	
	ustawienia.sin_family = AF_INET;
	ustawienia.sin_addr.s_addr = inet_addr( strHost.c_str() );
	ustawienia.sin_port = htons( ushPort );
	
	// Próba po³¹czenia z serwerem
	int polaczono = connect( gniazdo,
							 (SOCKADDR*)&ustawienia,
							 sizeof( ustawienia ) );
							 
	if( polaczono != SOCKET_ERROR )
	{
		cout << "Polaczenie nawiazane" << endl;
	}
	else
	{
		cout << "Nie mozna nawiazac polaczenia" << endl;
		
		closesocket( gniazdo );
		
		WSACleanup();
		
		return 3;	
	}

	for( ; ; )
	{
		string strLinia = "";
		
		getline( cin, strLinia );
		
		if( ( strcmp( strLinia.c_str(), "exit" ) == 0 ) || 
			( strcmp( strLinia.c_str(), "quit" ) == 0 ) )
		{
			break;	
		} 
		
		char buffOUT[ 256 ];
		
		memset( buffOUT, 0, sizeof( buffOUT ) );
		
		strcpy( buffOUT, strLinia.c_str() );
		
		int wyslano = send( gniazdo, buffOUT, strlen( buffOUT ), 0 );
		
		cout << "Wyslano znakow: " << wyslano << endl;
		
		cout << buffOUT;
		
		
		char buff_IN[ 4096 ];
		
		memset( buff_IN, 0, sizeof( buff_IN ) );
		
		int odebrano = recv( gniazdo, buff_IN, sizeof( buff_IN ), 0 );
	
		cout << "Odebrano znakow: " << strlen( buff_IN ) << endl;
		cout << buff_IN << endl;
	}

	closesocket( gniazdo );

	WSACleanup();
		
	return 0;
}
