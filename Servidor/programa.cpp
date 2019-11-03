#include "programa.h"

int iniciarPrograma(char name[]){

    SetConsoleTitleA(name);
	WSAData wsaData;
	WORD dll = MAKEWORD(2, 1);
	int puerto;

	// Initialize Winsock
	if(WSAStartup(dll, &wsaData) != 0)
	{
		MessageBoxA(0, "ERROR, EJECUTE ESTE PROGRAMA COMO ADMINISTRADOR", "usuario", MB_OK);
		exit(0);
	}

    puerto = pedirPuerto();

	// Resolve the server address and port
	SOCKADDR_IN direccion;
	int tamano = sizeof(direccion);
	direccion.sin_addr.s_addr = inet_addr("169.254.249.41");
	direccion.sin_port = htons(puerto);
	direccion.sin_family = AF_INET;

	// Create a SOCKET for connecting to server
	SOCKET ESCUCHAR = INVALID_SOCKET;
	ESCUCHAR = socket(AF_INET, SOCK_STREAM, NULL);
	if (ESCUCHAR == INVALID_SOCKET) {
        std::cout << "socket failed with error: %ld\n" << WSAGetLastError();
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    int iResult;
	iResult = bind(ESCUCHAR, (SOCKADDR*)&direccion, sizeof(direccion));
	if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: %d\n" << WSAGetLastError();
        closesocket(ESCUCHAR);
        WSACleanup();
        return 1;
    }

	iResult = listen(ESCUCHAR, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen failed with error: %d\n" << WSAGetLastError();
        closesocket(ESCUCHAR);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
	SOCKET NUEVA_CONEXION = INVALID_SOCKET;
	system("cls");
	std::cout << "El servidor se inicio correctamente y esta esperando solicitudes en el puerto "<< puerto << "\n";

	NUEVA_CONEXION = accept(ESCUCHAR, (SOCKADDR*)&direccion, &tamano);
    if (NUEVA_CONEXION == INVALID_SOCKET) {
        std::cout << "accept failed with error: %d\n" << WSAGetLastError();
        closesocket(ESCUCHAR);
        WSACleanup();
        return 1;
    }
    // No longer need server socket
    closesocket(ESCUCHAR);
    std::cout << "se recibio una solicitud de conexion " << "\n";
    sleepTime(10);
    std::cin.get();

    // Receive until the peer shuts down the connection
	int quit=false;
	while(!quit)
	{
		char mensaje[100];
		recv(NUEVA_CONEXION, mensaje, sizeof(mensaje), 0);
		if(strcmp(mensaje, "(F)")==0){
            std::cout << "Se procederá a cerrar la aplicación\n" ;
            quit=true;
            send(NUEVA_CONEXION, "(F)", 4, 0);
            closesocket(NUEVA_CONEXION);
            WSACleanup();
            sleepTime(90);
            return 1;
		}else{
            std::cout <<"Se recibio el mensaje: " << mensaje << "\n";
		}

		std::string cadena;
		char mensaje2[100];

        if(!quit){
		cadena = pedirCadena(" Server: ");

		strcpy(mensaje2, cadena.c_str());
		send(NUEVA_CONEXION, mensaje2, sizeof(mensaje2), 0);
        }
	}

	closesocket(NUEVA_CONEXION);
	WSACleanup();

}

int pedirPuerto(){
    int puerto = 0;
    std::cout << "Initialize Port: ";
	std::cin >> puerto;
    return puerto;
}

std::string pedirCadena(std::string text){
    std::string cadena;
    std::cout << text;
    std::getline(std::cin,cadena);
    return cadena;
}

void sleepTime(int time) {
    int i = 0;
    while(i<time) {
        i++;
        Sleep(60);
    }
}

void finaliar(){
	WSACleanup();
}
