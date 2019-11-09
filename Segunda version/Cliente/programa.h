#ifndef PROGRAMA_H_INCLUDED
#define PROGRAMA_H_INCLUDED

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>

std::string pedirCadena(std::string text);
int pedirPuerto();
void enviarAlgo(SOCKET socket);
void recivirAlgo(SOCKET socket);
int obtenerFuncion(char mensaje[100]);
std::string pedirIP();
void sleepTime(int time);

void receiveMessage(char mensaje2[100]);

int iniciarPrograma(char name[]);
void finalizar();
#endif // PROGRAMA_H_INCLUDED
