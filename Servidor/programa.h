#ifndef PROGRAMA_H_INCLUDED
#define PROGRAMA_H_INCLUDED

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>

int iniciarPrograma(char name[]);

void finaliar();

int pedirPuerto();

void sleepTime(int time);

std::string pedirCadena(std::string text);

#endif // PROGRAMA_H_INCLUDED
