#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* atoi */

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>

#include "programa.h"

#define BUFFSIZE 1


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
/*int obtenerFuncion(char mensaje[100]){

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
};*/

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

	// Inicializar Winsock
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
        enviarAlgo(conexion);
        recivirAlgo(conexion);
		//char mensaje3[100];
		// Recv, Read: Recibe mensaje
        /*recv(conexion, mensaje3, sizeof(mensaje3), 0);
        int opct = obtenerFuncion(mensaje3);
        switch (opct){
            case 1: // Función M (Mensaje):
            receiveMessage(mensaje3);
            break;
            case 2: // Función T (Transferencia de archivo):
            std::cout << "Se recibio comando de transferencia de archivo\n";
            break;
            case 3: // Función F (Fin):
            quit = closer(conexion);
            break;
            default:
            break;
        }*/


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

void enviarAlgo(SOCKET socket){
    std::string cadena;
    char mensaje2[100];
    char cad1[100];
    char cad2[100];
    cadena = pedirCadena("Cliente: ");
    std::string cad=cadena.substr(0,3);
    strcpy(cad1, cad.c_str());
    if(strcmp(cad1, "(M)")==0 || strcmp(cad1, "(F)")==0){
        strcpy(mensaje2, cadena.c_str());
        send(socket, mensaje2, sizeof(mensaje2), 0);
    }else if(strcmp(cad1, "(T)")==0){
        int n=0, b;
        char sendbuffer[100]; // envia de a 100 bits
        cad=cadena.substr(4);
        strcpy(cad2, cad.c_str());
        FILE *fp = fopen(cad2, "rb");
        if(fp == NULL){
            perror("File");
            enviarAlgo(socket);
        }else{
            strcpy(mensaje2, cadena.c_str());
            send(socket, mensaje2, sizeof(mensaje2), 0);
            std::cout << "Se empezara a mandar un archivo.\n";
            int tam=0;
            std::ifstream is;
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
    }else{
        std::cout << "error al elegir un comando.\n";
        enviarAlgo(socket);
    }
};

void recivirAlgo(SOCKET socket){
    int quit=false;
    char mensaje[100];
    char validarOpcion[100];
    recv(socket, mensaje, sizeof(mensaje), 0);
    std::string cad(mensaje);
    std::string cad3=cad.substr(0,3);
    std::string cad4=cad.substr(4);
    strcpy(validarOpcion, cad3.c_str());
    if(strcmp(validarOpcion, "(F)")==0){
        std::cout << "Se procederá a cerrar la aplicación\n" ;
        quit=true;
        closesocket(socket);
        WSACleanup();
        sleepTime(90);
    }else if(strcmp(validarOpcion, "(M)")==0){
        char mensajeCompleto[200];
        strcpy(mensajeCompleto, cad4.c_str());
        std::cout <<"Se recibio el mensaje: " << mensajeCompleto << "\n";
        //enviarAlgo(socket);
    }else{
        char nombreArchivo[200];
        char buff[1025];
        int b,tam=0,tam_ar;
        strcpy(nombreArchivo, cad4.c_str());
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
            //enviarAlgo(socket);
        } else {
            perror("File");
        }
    }
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


