#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>     /* atoi */

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>

#include "programa.h"

/*
void sendMessage(char mensaje2[100]){
    if(strcmp(mensaje2, "(F)")==0){
        std::cout << "Se procederá a cerrar la aplicación\n" ;
        //quit=true;
        // Envía mensaje
        send(conexion, "(F)", 6, 0);
        closesocket(conexion);
        WSACleanup();
        sleepTime(90);
    }else{
        std::cout << "Se recibio el mensaje: " << mensaje2 << "\n";
    }
}
*/
int obtenerFuncion(char mensaje[100]){

    int funtion=0;
    string msj(mensaje);
    string s1("(M)");
    string s2("(T)");
    string s3("(F)");

    if((msj.compare(0, 2, s1, 0, 2)) == 0)funtion=1;
    if((msj.compare(0, 2, s2, 0, 2)) == 0)funtion=2;
    if((msj.compare(0, 2, s3, 0, 2)) == 0)funtion=3;

    return funtion;
};

void receiveMessage(char mensaje2[100]){
    std::cout << "Se recibio el mensaje: " << mensaje2 << "\n";
};

bool closer(SOCKET conexion){
    std::cout << "Se procederá a cerrar la aplicación\n" ;
    // Envía mensaje
    send(conexion, "(F)", 6, 0);
    closesocket(conexion);
    WSACleanup();
    sleepTime(90);
    return true;
}

int iniciarPrograma(char name[]){

    SetConsoleTitleA(name);
    SOCKADDR_IN DIRECCION;
    SOCKET conexion = INVALID_SOCKET;
	WSAData wsaData;
	WORD dll = MAKEWORD(2, 1);
	int puerto;
	int iResult;
    std::string IP;

	// Initialize Winsock
	iResult = WSAStartup(dll, &wsaData);
	if (iResult != 0) {
        std::cout << "WSAStartup failed with error: %d\n" << iResult;
        return 1;
    }

    // Initialize Winsock

	int sizeofaddr = sizeof(DIRECCION);

    IP = pedirIP();
    puerto = pedirPuerto();

	DIRECCION.sin_addr.s_addr = inet_addr(IP.c_str());
	DIRECCION.sin_port = htons(puerto);
	DIRECCION.sin_family = AF_INET;

    // Create a SOCKET for connecting to server
    // Socket: Crea un descriptor de socket. t
    conexion = socket(AF_INET, SOCK_STREAM, NULL);
    if (conexion == INVALID_SOCKET) {
        std::cout << "socket failed with error: %ld\n" << WSAGetLastError();
        WSACleanup();
        return 1;
    }

	 // Connect to server.
	 // Connect: Inicia conexión con conector remoto
	iResult = connect(conexion, (SOCKADDR*)&DIRECCION, sizeofaddr);
	 if (iResult == SOCKET_ERROR) {
        // Close: Cierra socket
        closesocket(conexion);
        WSACleanup();
        return 1;
    }

	std::cin.get();
	int quit=false;

	while (!quit)
	{
		std::string cadena;
	    char mensaje[100];
        cadena = pedirCadena(" Cliente: ");
		strcpy(mensaje, cadena.c_str());
		// Send, Write: Envía mensaje
        send(conexion, mensaje, sizeof(mensaje), 0);

		char mensaje2[100];
		// Recv, Read: Recibe mensaje
        recv(conexion, mensaje2, sizeof(mensaje2), 0);
        int opct = obtenerFuncion(mensaje2);
        switch (opct){
            case 1: // Función M (Mensaje):
            receiveMessage(mensaje2);
            break;
            case 2: // Función T (Transferencia de archivo):
            std::cout << "Se recibio comando de transferencia de archivo\n";
            break;
            case 3: // Función F (Fin):
            quit = closer(conexion);
            break;
            default:
            break;
        }


	}
    // cleanup
    closesocket(conexion);
    WSACleanup();

};




std::string pedirCadena(std::string text){
    std::string cadena;
    std::cout << text;
    std::getline(std::cin, cadena);
    return cadena;
};

int pedirPuerto(){
    int puerto = 0;
    std::cout << "Require Port: ";
    std::cin >> puerto;
    return puerto;
};

std::string pedirIP(){
    std::string IP ;
    std::cout << "Require IP: ";
    std::cin >> IP;
    return IP;
};

void sleepTime(int time){
    int i = 0;
    while(i<time) {
        i++;
        Sleep(60);
    }
};

void finalizar(){
    WSACleanup();
};


