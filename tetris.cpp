/*
"Lucas, he visto como te brillan los ojos cuando hablamos de dinero" ~ Rosalia Peña Ros 2018
"..." ~ Lucas 2018

La siguiente pieza de codigo es libre, no tiene garantia alguna y solamente se ha programado
con propositos academicos. Eres libre de hackearla y distribuirla como quieras.

Esta (muy) basada en el Tetris hecho por javidx9 https://github.com/OneLoneCoder para consola de windows. 
Id a verlo que se aprende mucho con el.
*/
#define UNICODE 

#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>

using namespace std;

//Dimensiones de la consola
int nAnchura = 120;
int nAltura = 80;

//Dimensiones del campo
int nCampoAltura = 25;
int nCampoAnchura = 15;

//Velocidad
int nVel = 20;

wstring tetromino[7];

bool gameOver = false;
unsigned char *pCampo = nullptr;
vector<int> Lineas;

int Rotar(int px, int py, int rotacion) {
	switch (rotacion)
	{
	case 0: return py * 4 + px;
	case 1: return  px * 4 + (3 - py);
	case 2: return (3 - py) * 4 + (3 - px);
	case 3: return (3 - px) * 4 + py;
	}
	return 0;
}

bool cabe(int nTetromino, int nRotacion, int nPosX, int nPosY) {
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++) {
			//Obtener el indice efectivo de la pieza
			int pi = Rotar(px, py, nRotacion);

			//Obtener el indice en el campo
			int fi = (nPosY + py) * nCampoAnchura + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nCampoAnchura) {
				if (nPosY + py >= 0 && nPosY + py < nCampoAltura) {
					if (tetromino[nTetromino][pi] != L'.' && pCampo[fi] != 0)
						return false;
				}
			}
		}
	return true;
}

int main() {	//Creacion de assets
	tetromino[0].append(L"..X...X...X...X."); // Tetronimos 4x4
	tetromino[1].append(L"..X..XX...X.....");
	tetromino[2].append(L".....XX..XX.....");
	tetromino[3].append(L"..X..XX..X......");
	tetromino[4].append(L".X...XX...X.....");
	tetromino[5].append(L".X...X...XX.....");
	tetromino[6].append(L"..X...X..XX.....");

	pCampo = new unsigned char[nCampoAnchura*nCampoAltura];
	for (int x = 0; x < nCampoAnchura; x++) {
		for (int y = 0; y < nCampoAltura; y++) {
			pCampo[y*nCampoAnchura + x] = (x == 0 || x == nCampoAnchura - 1 || y == nCampoAltura - 1) ? 9 : 0;
		}
	}
	//for (int y = 0; y < nCampoAltura; y++) { for (int x = 0; x < nCampoAnchura; x++) cout <<(int) pCampo[y*nCampoAnchura + x] << " "; cout << endl; }

	//Preparacion del entorno de consola 
	wchar_t *pantalla = new wchar_t[nAltura*nAnchura];
	for (int i = 0; i < nAltura*nAnchura; i++) pantalla[i] = L' ';
	HANDLE hConsola = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsola);
	DWORD nBytesEscritos = 0;

	//Cosicas de la logica del juego
	int nPieza = 0;
	int nRotacion = 0;
	int nX = nCampoAnchura/2;
	int nY = 0;
	int nVelI = 0;
	int nPuntos = 0;

	bool bTecla[4];
	bool nRotacionAntigua = false;

	//Bucle del juego
	while (!gameOver) {
		//TIMING ===============================================
		this_thread::sleep_for(40ms);
		nVelI++;

		//ENTRADA DEL USUARIO ===================================
		for (int k = 0; k < 4; k++)									//Dcha Izda Abajo Z
			bTecla[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		if (nVelI == nVel) {
			Beep(nY*100+100, 100);

			if(cabe(nPieza, nRotacion, nX, nY+1)) //Si la pieza cabe se baja 
				nY += (cabe(nPieza, nRotacion, nX, nY + 1)) ? 1 : 0;
			
			//Si no cabe hay que hacer un par de cosillas :/
			else {
				nPuntos += 25;
				//Inscribir la pieza en los bloques del campo
				for (int x = 0; x < 4; x++) {
					for (int y = 0; y < 4; y++) {
						if (tetromino[nPieza][Rotar(x, y, nRotacion)] != L'.') {
							pCampo[nCampoAnchura*(y + nY) + x + nX] = nPieza + 1;
						}
					}
				}


				//Comprobar si se ha logrado completar alguna linea 
				for(int py = 0; py < 4; py++)
					if (nY + py < nCampoAltura - 1)
					{
						bool bLinea = true;
						for (int px = 1; px < nCampoAnchura - 1; px++)
							bLinea &= (pCampo[(nY + py)*nCampoAnchura + px]) != 0;

						if (bLinea)
						{
							//Cargarse la linea, ponerlo todo a =
							for (int px = 1; px < nCampoAnchura - 1; px++)
								pCampo[(nY + py)*nCampoAnchura + px] = 8;
							Lineas.push_back(nY + py);
						}
					}



				//Generar una nueva pieza
				nPieza = rand() % 7;
				nRotacion = 0;
				nY = 0; nX = nCampoAnchura / 2;

				//Si la nueva pieza no cabe, a tomar por saco
				if (!cabe(nPieza, nRotacion, nX, nY)) gameOver = true;
			}
			nVelI = 0;
		}

		//LOGICA DEL JUEGO =====================================
		nX += (bTecla[0] && cabe(nPieza, nRotacion, nX + 1, nY)) ? 1 : 0;
		nX -= (bTecla[1] && cabe(nPieza, nRotacion, nX - 1, nY)) ? 1 : 0;
		nY += (bTecla[2] && cabe(nPieza, nRotacion, nX, nY + 1)) ? 1 : 0;

		if (bTecla[3] && cabe(nPieza, (nRotacion + 1) % 4, nX , nY) && !nRotacionAntigua) {
			nRotacion = (nRotacion + 1) % 4;
			nRotacionAntigua = true;
		}
		if(!bTecla[3]) nRotacionAntigua = false;

		//RENDERIZAR SALIDA ====================================

			//Dibujado del campo
		for (int x= 0; x < nCampoAnchura; x++)
			for (int y = 0; y < nCampoAltura; y++)
				pantalla[(y + 2)*nAnchura + (x + 2)] = L" ABCDEFG=#"[pCampo[y*nCampoAnchura + x]];
			
			//Dibujado de la pieza
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nPieza][Rotar(px, py, nRotacion)] != L'.'){ 
					pantalla[(nY + py + 2) * nAnchura + (nX + px + 2)] = nPieza + 65;
				}

		swprintf_s(&pantalla[2 * nAnchura  + nCampoAnchura + 6], 16, L"Puntos: %d", nPuntos);

		if (!Lineas.empty()) {
			WriteConsoleOutputCharacter(hConsola, pantalla, nAnchura*nAltura, { 0,0 }, &nBytesEscritos);
			this_thread::sleep_for(400ms);

			nPuntos += Lineas.size() * 100  +  ((Lineas.size() == 4) ? 600 : 0);

			for(auto &v : Lineas)
				for (int px = 1; px < nCampoAnchura - 1; px++) {
					for (int py = v; py > 0; py--)
						pCampo[py * nCampoAnchura + px] = pCampo[(py - 1) * nCampoAnchura + px];
					pCampo[px] = 0;
				}
			Lineas.clear();
		}

			//Sacar el framebuffer por pantalla
		WriteConsoleOutputCharacter(hConsola, pantalla, nAnchura*nAltura, { 0,0 }, &nBytesEscritos);
	}

	CloseHandle(hConsola);
	cout << "Game Over. Puntos : " << nPuntos <<  endl;
	system("pause");
}