#include <iostream>
#include <fstream>
#include <winsock2.h>

using namespace std;

int main(int argc, char** argv)
{
	// Inicjalizacja mechanizmu gniazd - takie samo jak w cliencie
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

	
	// W��czenie gniazda - takie samo jak w cliencie
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

	// Wczytanie danych z pliku, �eby nie ustawia� za ka�dym razem w kodzie programu
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
		
		cout << "Nie mo�na wczyta� ustawie� z pliku" << endl;
		
		WSACleanup();
		
		return 10;
	}

	// Ustawienie parametr�w gniazda - podobnie jak w serwerze
	sockaddr_in ustawienia;
	
	memset( &ustawienia, 0, sizeof( ustawienia ) );
	
	ustawienia.sin_family = AF_INET;
	ustawienia.sin_addr.s_addr = inet_addr( strHost.c_str() );	// Adres IP serwera to TEN komputer, potem prze��czymy jak nawi��emy komunikacj�
	ustawienia.sin_port = htons( ushPort );
	
	// Zabindowanie gniazda
	if( bind( gniazdo, (SOCKADDR*)&ustawienia, sizeof( ustawienia ) ) != SOCKET_ERROR )
	{
		cout << "Zabindowano gniazdo" << endl;
	}
	else
	{
		cout << "Nie mo�na zabindowa� gniazda" << endl;    
	    
		closesocket( gniazdo );
		
		WSACleanup();
	    
		return 10;
	}
	
	// Pr�ba nas�uchiwania na danym porcie
	if( listen( gniazdo, 1 ) != SOCKET_ERROR )
	{
		cout << "Rozpoczecie procedury nasluchiwania" << endl;
	}
	else
	{
     	cout << "Nie mozna rozpoczac procedury nasluchiwania" << endl;
     	
    	closesocket( gniazdo );
		
		WSACleanup();
	    
		return 8; 	
	}	

	// Utworzenie gniazda pomocniczego do prze��czenia, je�li zostanie wychwycona komunikacja
	SOCKET gniazdo_nasluch;
	
	cout << "Trwa nasluchiwanie..." << endl;
	
	gniazdo_nasluch = accept( gniazdo, NULL, NULL );
	
	cout << "Klient polaczony" << endl;
	
	gniazdo = gniazdo_nasluch;
	
	

	for( ; ; )
	{
		char buff_IN[ 4096 ];
		
		memset( buff_IN, 0, sizeof( buff_IN ) );
		
		int odebrano = recv( gniazdo, buff_IN, sizeof( buff_IN ), 0 );
	
		cout << "Odebrano znakow: " << strlen( buff_IN ) << endl;
		cout << buff_IN << endl;
		
		
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
	}

	closesocket( gniazdo );

	WSACleanup();
		
	return 0;
}
