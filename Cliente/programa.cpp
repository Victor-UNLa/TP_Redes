#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>     /* atoi */
#include <fstream>

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>

#include "programa.h"
#define BUFFSIZE 1


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

void receiveMessage(char mensaje[100]){

    std::string newMensaje(mensaje);
    newMensaje = newMensaje.substr(4);
    std::cout << "Se recibio el mensaje: " << newMensaje << "\n";

};



void sendData(SOCKET socket, std::string cadena){

    char cad2[100];
    int n=0, b;
    char mensaje[100];
    char sendbuffer[100]; // envia de a 100 bits
    std::string cad=cadena.substr(4);
    strcpy(cad2, cad.c_str());
    FILE *fp = fopen(cad2, "rb");
    if(fp == NULL){
        perror("File");
        std::cout << "Error no se encontro el archivo. Espere a que le envien un mensaje.\n";
        char msg[100]="error";
        sendMessage(socket,msg);
    }else{
        strcpy(mensaje, cadena.c_str());
        send(socket, mensaje, sizeof(mensaje), 0);
        std::cout << "Se empezara a mandar un archivo.\n";
        int tam=0;
        std::ifstream is; // usa lib <fstream> para manejo de archivo
        is.open (cad2, std::ios::binary );
        is.seekg (0, std::ios::end);
        tam = is.tellg();
        is.seekg (0, std::ios::beg);
        send(socket, reinterpret_cast<char*>(&tam), sizeof tam , 0);
        while( (b = fread(sendbuffer, 1, sizeof(sendbuffer), fp))>0 ){
            send(socket, sendbuffer, b, 0);
        }
        std::cout << "Se envio el archivo.\n";
        sleepTime(10);
    }

}

void recvData(SOCKET socket, std::string mensaje){

    char cad1[100];
    std::string cad=mensaje.substr(0,3);
    std::string nameFile=mensaje.substr(4);

    strcpy(cad1, cad.c_str());

    char nombreArchivo[200];
    char buff[1025];
    int b,tam=0,tam_ar;
    strcpy(nombreArchivo, nameFile.c_str());
    std::cout << "Se recibio comando de transferencia de archivo." << "\n";
    FILE* fp = fopen(nombreArchivo, "wb");
    if(fp != NULL){
        std::cout << "El archivo se guardara dentro de: /" << nombreArchivo << "\n";
        recv(socket, reinterpret_cast<char*>(&tam_ar), sizeof tam_ar, 0);
        while(tam<tam_ar){
            b = recv(socket, buff, 1024,0);
            fwrite(buff, 1, b, fp);
            tam+=b;
        }
        if(tam<tam_ar){
            std::cout << "El archivo NO se recibio completamente" << "\n";
        }
        fclose(fp);
        std::cout << "Archivo recibido." << "\n";
    } else {
        perror("File");
    }

}

bool closer(SOCKET conexion){

    std::cout << "Se procederá a cerrar la aplicación\n" ;
    // Envía mensaje
    send(conexion, "(F)", sizeof("(F)"), 0);
    // Finalizo
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

    std::cout << "solicitud de conexión aceptada" << "\n";
    sleepTime(10);

	std::cin.get();
	int quit=false;

	while (!quit)
	{
		std::string cadena;
	    char mensaje[100];
        int opct = 0;

		while(opct<1 || opct>3){
		    cadena = pedirCadena(" Cliente: ");
            strcpy(mensaje, cadena.c_str());
            opct = obtenerFuncion(mensaje);
        }
        switch (opct){
            case 1: // Función M (Mensaje):
            send(conexion, mensaje, sizeof(mensaje), 0);
            break;
            case 2: // Función T (Transferencia de archivo):
            sendData(conexion,mensaje);
            break;
            case 3: // Función F (Fin):
            quit = closer(conexion);
            break;
            default:
            break;
        }

        if(!quit){
            char mensaje2[100];
            // Recv, Read: Recibe mensaje
            recv(conexion, mensaje2, sizeof(mensaje2), 0);
            opct = obtenerFuncion(mensaje2);
            switch (opct){
                case 1: // Función M (Mensaje):
                receiveMessage(mensaje2);
                break;
                case 2: // Función T (Transferencia de archivo):
                recvData(conexion,mensaje2);
                break;
                case 3: // Función F (Fin):
                quit = closer(conexion);
                break;
                default:
                break;
            }
        }

	}
    // cleanup
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


