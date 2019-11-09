#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "programa.h"


int iniciarPrograma(char name[]){

    SetConsoleTitleA(name);
	WSAData wsaData;
	WORD dll = MAKEWORD(2, 1);
	int puerto;

	// Inicializar Winsock
	if(WSAStartup(dll, &wsaData) != 0)
	{
		MessageBoxA(0, "ERROR, EJECUTE ESTE PROGRAMA COMO ADMINISTRADOR", "usuario", MB_OK);
		exit(0);
	}

    puerto = pedirPuerto(); // se pide el puerto en el que correra el servidor

	// Resolve the server address and port
	SOCKADDR_IN direccion;
	int tamano = sizeof(direccion);
	direccion.sin_addr.s_addr = inet_addr("192.168.0.2"); // ip en donde correra el servidor
	direccion.sin_port = htons(puerto); // se le pasa el puerto para hacer la direccion
	direccion.sin_family = AF_INET;

	// crear el socket para conectar con el servidor
	SOCKET ESCUCHAR = INVALID_SOCKET;
	ESCUCHAR = socket(AF_INET, SOCK_STREAM, NULL);
	if (ESCUCHAR == INVALID_SOCKET) { //veo que carge bien el socket
        std::cout << "Error al cargar el socket: %ld\n" << WSAGetLastError();
        WSACleanup();
        return 1;
    }

    // bind el socket y ver si no hay error
    int iResult;
	iResult = bind(ESCUCHAR, (SOCKADDR*)&direccion, sizeof(direccion));
	if (iResult == SOCKET_ERROR) {
        std::cout << "Error en el Bind: %d\n" << WSAGetLastError();
        closesocket(ESCUCHAR);
        WSACleanup();
        return 1;
    }

	iResult = listen(ESCUCHAR, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "Error al escuchar: %d\n" << WSAGetLastError();
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
        std::cout << "Error en el accept: %d\n" << WSAGetLastError();
        closesocket(ESCUCHAR);
        WSACleanup();
        return 1;
    }
    // cuando se conecta
    closesocket(ESCUCHAR);
    std::cout << "se recibio una solicitud de conexion " << "\n";
    sleepTime(10);
    std::cin.get();

    // que hace durante la conexion
	int quit=false;
	while(!quit)
	{

		recibirAlgo(NUEVA_CONEXION);

	}
	closesocket(NUEVA_CONEXION);
	WSACleanup();
}

int pedirPuerto(){
    int puerto = 0;
    std::cout << "Ingrese el puerto: ";
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

void recibirAlgo(SOCKET socket){
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
        enviarAlgo(socket);
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
            enviarAlgo(socket);
        } else {
            perror("File");
        }
    }
};

void enviarAlgo(SOCKET socket){
    std::string cadena;
    char mensaje2[100];
    char cad1[100];
    char cad2[100];
    cadena = pedirCadena("Servidor: ");
    std::string cad=cadena.substr(0,3);
    strcpy(cad1, cad.c_str());
    if(strcmp(cad1, "(M)")==0){
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
    }else if(strcmp(cad1, "(F)")==0){
        std::cout << "Se procederá a cerrar la aplicación\n" ;
        strcpy(mensaje2, cadena.c_str());
        send(socket, mensaje2, sizeof(mensaje2), 0);
        sleepTime(90);
        closesocket(socket);
        WSACleanup();
    }else{
        std::cout << "error al elegir un comando.\n";
        enviarAlgo(socket);
    }
}

void finaliar(){
	WSACleanup();
}
