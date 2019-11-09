#ifndef PROGRAMA_H_INCLUDED
#define PROGRAMA_H_INCLUDED


std::string pedirCadena(std::string text);
int pedirPuerto();
int obtenerFuncion(char mensaje[100]);
std::string pedirIP();
void sleepTime(int time);

void receiveMessage(char mensaje2[100]);

int iniciarPrograma(char name[]);
void finalizar();
#endif // PROGRAMA_H_INCLUDED
