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

void sendMessage(SOCKET socket, char mensaje[100]){

    send(socket, mensaje, sizeof(mensaje), 0);

}

void receiveMessage(char mensaje[100]){

    std::string newMensaje(mensaje);
    newMensaje=newMensaje.substr(4);
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
    sendMessage(conexion, "(F)");
    // Finalizo
    closesocket(conexion);
    WSACleanup();
    sleepTime(90);
    return true;

}

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

    puerto = pedirPuerto(); // se pide el puerto en el que correra el servidor

	// Resolve the server address and port
	SOCKADDR_IN direccion;
	int tamano = sizeof(direccion);
	direccion.sin_addr.s_addr = inet_addr("127.0.0.1"); // ip en donde correra el servidor
	direccion.sin_port = htons(puerto); // se le pasa el puerto para hacer la direccion
	direccion.sin_family = AF_INET;

	// Create a SOCKET for connecting to server
	SOCKET ESCUCHAR = INVALID_SOCKET;
	ESCUCHAR = socket(AF_INET, SOCK_STREAM, NULL);
	if (ESCUCHAR == INVALID_SOCKET) { //veo que carge bien el socket
        std::cout << "socket failed with error: %ld\n" << WSAGetLastError();
        WSACleanup();
        return 1;
    }

    // bind el socket y ver si no hay error
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

    // poner en accept para esperar conexion
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
    // cuando se conecta
    closesocket(ESCUCHAR);
    std::cout << "se recibio una solicitud de conexion " << "\n";
    sleepTime(10);

	std::cin.get();
	int quit=false;

	while (!quit)
	{
		std::string cadena;
	    char mensaje[100];
	    char mensaje2[100];
	    int opct=0;
		// Recv, Read: Recibe mensaje
        recv(NUEVA_CONEXION, mensaje2, sizeof(mensaje2), 0);
        opct = obtenerFuncion(mensaje2);
        switch (opct){
            case 1: // Función M (Mensaje):
            receiveMessage(mensaje2);
            break;
            case 2: // Función T (Transferencia de archivo):
            recvData(NUEVA_CONEXION,mensaje2);
            break;
            case 3: // Función F (Fin):
            quit = closer(NUEVA_CONEXION);
            break;
            default:
            break;
        }
        opct=0;
        if(!quit){
            while(opct<1 || opct>3){
                cadena = pedirCadena(" Server: ");
                strcpy(mensaje, cadena.c_str());
                opct = obtenerFuncion(mensaje);
            }
            switch (opct){
                case 1: // Función M (Mensaje):
                send(NUEVA_CONEXION, mensaje, sizeof(mensaje), 0);
                break;
                case 2: // Función T (Transferencia de archivo):
                sendData(NUEVA_CONEXION,mensaje);
                break;
                case 3: // Función F (Fin):
                quit = closer(NUEVA_CONEXION);
                break;
                default:
                break;
            }
        }


	}
    // cleanup
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
