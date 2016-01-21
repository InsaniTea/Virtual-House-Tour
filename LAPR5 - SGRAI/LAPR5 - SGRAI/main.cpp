/* GRUPO 6 LAPR5
-1130339 - AntÛnio Pinheiro
-1130371 - Cristina Lopes
-1130616 - Ricardo Moreira
-1130487 - Pedro Magalh„es
-1130728 - Jo„o Cabral
-1130353 - Pedro Coelho */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <AL/alut.h>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <GL/glaux.h>
#endif

#include "mathlib.h"
#include "studio.h"
#include "mdlviewer.h"
#include "XMLParser.h"
#include "ZipFileHandling.h"
#include "EstruturaImovel.h"
#include "ParticleEngine.h"

using namespace std;

#pragma comment (lib, "glaux.lib")    /* link with Win32 GLAUX lib usada para ler bitmaps */
#pragma comment (lib, "openAL32.lib")
#pragma comment (lib, "alut.lib")
#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ ) 

// função para ler jpegs do ficheiro readjpeg.c
extern "C" int read_JPEG_file(const char *, char **, int *, int *, int *);

const int TIMER_MS = 25; //The number of milliseconds to which the timer is set
const float PI = 3.1415926535f;

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define RAD(x)          (M_PI*(x)/180)
#define GRAUS(x)        (180*(x)/M_PI)

#define	GAP					              25

#define IMOVEL_HEIGHT			    38
#define IMOVEL_WIDTH			    38
#define NUM_PISOS					2
#define ESPESSURA_CHAO_TETO			0.5
#define ALTURA_PAREDES				3
#define ALTURA_TOTAL_PISO			6
#define GL_MAX_LIGHTS 20

/*para saber qual o piso atual*/


#define	OBJECTO_ALTURA		      0.4
#define OBJECTO_VELOCIDADE	      0.5
#define OBJECTO_ROTACAO		        5
#define OBJECTO_RAIO		      0.12
#define SCALE_HOMER               0.015
#define EYE_ROTACAO			        1

#ifdef _WIN32
#define NOME_TEXTURA_CUBOS        "Textura.bmp"
#else
#define NOME_TEXTURA_CUBOS        "Textura.jpg"
#endif

#define NOME_TEXTURA_CHAO         "ChaoBase.jpg"
#define NOME_TEXTURA_CHAO_PISO_0  "ChaoPiso0.jpg"

#define NUM_TEXTURAS              1
#define ID_TEXTURA_CUBOS          2
#define ID_TEXTURA_CHAO           1
#define ID_TEXTURA_CHAO_PISO_0    2

#define	CHAO_DIMENSAO		      40

#define NUM_JANELAS               2
#define JANELA_TOP                0
#define JANELA_NAVIGATE           1

typedef struct teclas_t{
	GLboolean		up, down, left, right, Q, R, T, Y, U, I, O, P, G, F, J, K, N, M;
}teclas_t;

typedef struct pos_t{
	GLfloat			x, y, z;
}pos_t;

typedef struct objecto_t{
	pos_t			pos;
	GLfloat			dir;
	GLfloat			vel;
}objecto_t;

typedef struct camera_t{
	pos_t			eye;
	GLfloat			dir_long;  // longitude olhar (esq-dir)
	GLfloat			dir_lat;   // latitude olhar	(cima-baixo)
	GLfloat			fov;
}camera_t;

typedef struct ESTADO{
	camera_t		camera;
	GLint			timer;
	GLint			mainWindow, topSubwindow, navigateSubwindow;
	teclas_t		teclas;
	GLboolean		localViewer;
	GLuint			vista[NUM_JANELAS];
	ALuint			buffer[9], source[3];
}ESTADO;

typedef struct MODELO{
	GLuint			texID[NUM_JANELAS][NUM_TEXTURAS];
	GLuint			texturasID[50]; //para guardar todos os id's das texturas
	GLuint			imovel[NUM_JANELAS];
	GLuint			chao[NUM_JANELAS];
	objecto_t		objecto;
	GLuint			xMouse;
	GLuint			yMouse;
	StudioModel		homer[NUM_JANELAS];   // Modelo Homer
	StudioModel		movel;
	GLboolean		andar;
	GLuint			prev;
	vector<StudioModel> listaMoveis;
	GLuint		    tempo_anim;
	vector<vector<float>> colisoes_Mobila;
	int				pisoAtual;
	int				flagSubir;
	int				flagDescer;
	char*			tooltip;
} MODELO;

/////////////////////////////////////
//variaveis globais

ESTADO estado;
MODELO modelo;

XMLParser xml;
vector<vector<vector<retangulo_t>>> paredes_imovel;
vector<vector<vector<retangulo_t>>> chao_imovel;
vector<vector<vector<retangulo_t>>> teto_imovel;
vector<vector<string>> listaMoveis;
vector<vector<string>> listaPos;
vector<string> nomeTexturas;
vector<vector<string>> listaLuzes;
coordenadas_t posicao_inicial;
vector<coordenadas_t> skybox;
camaras_t camaras;
ParticleEngine* _particleEngine;
GLuint _textureId;

//piso em que o homer estava na vez anterior em que o timer foi chamado
int piso_anterior;
EstruturaImovel* estImovel;
void createDisplayLists(int janelaID);

////////////////////////////////////
/// IluminaÁ„o e materiais


void setLight(){
	int contador = 0;
	float x, y, z, at;
	glEnable(GL_LIGHTING);
	for (int i = 0; i < listaLuzes.size(); i++)
	{
		if (contador <= 6){
			glPushMatrix();
			x = stof(listaLuzes[i][0]);
			y = stof(listaLuzes[i][1]);
			z = stof(listaLuzes[i][2]);
			GLfloat qaDiffuseLight[] = { x, y, z, 1.0 };
			GLfloat qaSpecularLight[] = { x, y, z, 1.0 };

			x = stof(listaLuzes[i][3]);
			y = stof(listaLuzes[i][4]);
			z = stof(listaLuzes[i][5]);
			GLfloat qaLightPosition[] = { 0.0, 0.0, 0.0, 1 };
			GLfloat dirVector0[] = { x, y, z, 0.0 };

			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0 + i);

			x = stof(listaLuzes[i][6]);
			y = stof(listaLuzes[i][7]);
			z = stof(listaLuzes[i][8]);
			glTranslatef(x, y, z);

			glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, qaDiffuseLight);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, qaLightPosition);
			glLightfv(GL_LIGHT0 + i, GL_SPECULAR, qaSpecularLight);

			at = stof(listaLuzes[i][9]);
			glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 90.0);// set cutoff angle
			glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, dirVector0);
			glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, 1); // set focusing strength
			glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, at); /*atenuacao da luz*/
			glLightf(GL_LIGHT0 + i, GL_DEPTH_RANGE, .02);
			contador++;
			glPopMatrix();
		}

	}

}

void setMaterial()
{
	GLfloat mat_specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_shininess = 104;

	// criaÁ„o autom·tica das componentes Ambiente e Difusa do material a partir das cores
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// definir de outros par‚metros dos materiais estaticamente
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
}

/////SISTEMA DE PARTICULAS/////

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Image* image = loadBMP("circle.bmp");
	Image* alphaChannel = loadBMP("circlealpha.bmp");
	_textureId = loadAlphaTexture(image, alphaChannel);
	delete image;
	delete alphaChannel;
}

void desenhaParticulas(){
	glPushMatrix();

	initRendering();

	glTranslatef(3.75f, 1.5f, -18.0f);
	glScalef(2.0f, 2.0f, 2.0f);
	_particleEngine = new ParticleEngine(_textureId);
	_particleEngine->draw();

	glPopMatrix();
}

void update(int value) {
	_particleEngine->advance(TIMER_MS / 1000.0f);
	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}

/////SKYBOX/////////

void Draw_Skybox()
{
	float	x = skybox[0].xyz[0],
		y = skybox[0].xyz[1],
		z = skybox[0].xyz[2],
		width = skybox[1].xyz[0],
		height = skybox[1].xyz[1],
		length = skybox[1].xyz[2];

	// Center the Skybox around the given x,y,z position
	x = x - width / 2;
	y = y - height / 2;
	z = z - length / 2;


	// Draw Front side
	glBindTexture(GL_TEXTURE_2D, modelo.texturasID[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z + length);
	glEnd();

	// Draw Back side
	glBindTexture(GL_TEXTURE_2D, modelo.texturasID[8]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Left side
	glBindTexture(GL_TEXTURE_2D, modelo.texturasID[6]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Right side
	glBindTexture(GL_TEXTURE_2D, modelo.texturasID[7]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
	glEnd();

	// Draw Up side
	glBindTexture(GL_TEXTURE_2D, modelo.texturasID[9]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
	glEnd();

	// Draw Down side
	glBindTexture(GL_TEXTURE_2D, modelo.texturasID[9]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z);
	glEnd();

}

void reshapeNavigateSubwindow(int width, int height)
{
	// glViewport(botom, left, width, height)
	// define parte da janela a ser utilizada pelo OpenGL
	glViewport(0, 0, (GLint)width, (GLint)height);
	// Matriz Projeccao
	// Matriz onde se define como o mundo e apresentado na janela
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(estado.camera.fov, (GLfloat)width / height, 0.1, camaras.zfar_navegacao);
	// Matriz Modelview
	// Matriz onde s„o realizadas as tranformacoes dos modelos desenhados
	glMatrixMode(GL_MODELVIEW);
}

void strokeCenterString(char *str, double x, double y, double z, double s)
{
	int i, n;

	n = strlen(str);
	glPushMatrix();
	glTranslated(x - glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char*)str)*0.5*s, y - 119.05*0.5*s, z);
	glScaled(s, s, s);
	for (i = 0; i < n; i++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)str[i]);
	glPopMatrix();
}

void desenhaBussola(int width, int height, char* info)
{
	// Altera viewport e projecÁ„o
	glPushMatrix();
	glViewport(width - 60, 0, 60, 60);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(-30, 30, -30, 30);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);
	glRotatef(-GRAUS(estado.camera.dir_long + estado.camera.dir_lat) - 90, 0, 0, 1);

	glBegin(GL_TRIANGLES);
	glColor4f(0, 0, 0, 0.2);
	glVertex2f(0, 15);
	glVertex2f(-6, 0);
	glVertex2f(6, 0);
	glColor4f(1, 1, 1, 0.2);
	glVertex2f(6, 0);
	glVertex2f(-6, 0);
	glVertex2f(0, -15);
	glEnd();

	glLineWidth(1.0);
	glColor3f(1, 0.4, 0.4);
	strokeCenterString("N", 0, 20, 0, 0.1);
	strokeCenterString("S", 0, -20, 0, 0.1);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	glPushMatrix();

	/*informacao do piso*/
	string identificador;
	string piso;

	if (modelo.pisoAtual == 1){
		identificador = "Encontra-se no piso 0";
	}
	else{
		identificador = "Encontra-se no piso 1";
	}

	char *cstr = new char[identificador.size()];
	strcpy(cstr, identificador.c_str());

	glViewport(width - 260, height - 250, 250, 250);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(-50, 60, -30, 30);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);

	glLineWidth(1.0);
	glColor3f(1, 0.4, 0.4);
	strokeCenterString(cstr, 20, 20, 0, 0.05);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	glPopMatrix();



	glPushMatrix();

	glViewport(width - 240, height - 400, 250, 250);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(-50, 60, -30, 30);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);

	glLineWidth(1.0);
	glColor3f(5.0, 5.0, 5.0);
	strokeCenterString(info, 20, 20, 0, 0.05);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);


	//repıe projecÁ„o chamando redisplay
	reshapeNavigateSubwindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glPopMatrix();
}


void escreveInfo(char* info){

	glViewport(800 - 220, 170, 250, 250);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(-50, 60, -30, 30);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_COLOR_MATERIAL);

	glLineWidth(1.0);
	glColor3f(1, 0.4, 0.4);
	strokeCenterString(info, 20, 20, 0, 0.03);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	desenhaBussola(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), info);
	glutSetWindow(estado.navigateSubwindow);

}

///////////////////////////////////
//// Redisplays

void redisplayTopSubwindow(int width, int height)
{
	// glViewport(botom, left, width, height)
	// define parte da janela a ser utilizada pelo OpenGL
	glViewport(0, 0, (GLint)width, (GLint)height);
	// Matriz Projeccao
	// Matriz onde se define como o mundo e apresentado na janela
	glMatrixMode(GL_PROJECTION);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat qaAmbientLight[] = { 0.8, 0.8, 0.8, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);

	glLoadIdentity();
	gluPerspective(120, (GLfloat)width / height, .5, camaras.zfar_planta);
	//glOrtho(-((IMOVEL_HEIGHT + 5.0) / 2.0), (IMOVEL_HEIGHT + 5.0) / 2.0, 0.0, 10.0 + ALTURA_PAREDES*paredes_imovel.size() - 1, -((IMOVEL_HEIGHT + 5.0) / 2.0), (IMOVEL_HEIGHT + 5.0) / 2.0);
	// Matriz Modelview
	// Matriz onde s„o realizadas as tranformacoes dos modelos desenhados
	glMatrixMode(GL_MODELVIEW);

}



void reshapeMainWindow(int width, int height)
{
	// glViewport(botom, left, width, height)
	// define parte da janela a ser utilizada pelo OpenGL
	glViewport(0, 0, (GLint)width, (GLint)height);
	// Matriz Projeccao
	// Matriz onde se define como o mundo e apresentado na janela
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(estado.camera.fov, (GLfloat)width / height, 0.1, camaras.zfar_navegacao);
	// Matriz Modelview
	// Matriz onde s„o realizadas as tranformacoes dos modelos desenhados
	glMatrixMode(GL_MODELVIEW);

	//Reshape para a subwindow
	GLint l;

	width > height ? (l = height / 4.0) : (l = width / 4.0);

	glutSetWindow(estado.topSubwindow);
	glutPositionWindow(10, 10);
	glutReshapeWindow(l, l);
}

GLboolean detectaColisaoParede(GLfloat nx, GLfloat ny, GLfloat nz){
	int fx = 0, fy = 0, fz = 0;
	GLfloat x = 0, z = 0;
	float valores1[2];
	float valores2[2];
	float vec[2];
	coordenadas_t coordenadas;

	for each (vector<retangulo_t> vec_parede in paredes_imovel[modelo.pisoAtual - 1])
	{
		for each (retangulo_t parede in vec_parede)
		{

			/*determinacao de como e que a parede esta feita e qual e o intervalo das coordenadas em que tem de estar o homer para bater na parede*/
			coordenadas_t coordenada1 = parede.canto[1];
			coordenadas_t coordenada2 = parede.canto[3];

			valores1[0] = coordenada1.xyz[0] * .6;
			valores1[1] = coordenada1.xyz[2] * .6;

			valores2[0] = coordenada2.xyz[0] * .6;
			valores2[1] = coordenada2.xyz[2] * .6;

			/*------------------------------------------------------*/

			if ((int)valores1[0] == (int)valores2[0]){
				fx = 1;
			}
			else if ((int)valores1[1] == (int)valores2[1]){
				fz = 1;
			}


			if (fx == 1){
				if (valores1[1] > valores2[1]){
					if ((int)nz <= (int)valores1[1] && (int)nz >= (int)valores2[1] && (int)nx == (int)valores1[0]){
						return GL_TRUE;
					}
				}
				else if (valores1[1] < valores2[1]){
					if ((int)nz >= (int)valores1[1] && (int)nz <= (int)valores2[1] && (int)nx == (int)valores1[0]){
						return GL_TRUE;
					}
				}
			}
			if (fz == 1){
				if (valores1[0] > valores2[0]){
					if ((int)nx <= (int)valores1[0] && (int)nx >= (int)valores2[0] && (int)nz == (int)valores1[1]){
						return GL_TRUE;
					}
				}
				else if (valores1[0] < valores2[0]){
					if ((int)nx >= (int)valores1[0] && (int)nx <= (int)valores2[0] && (int)nz == (int)valores1[1]){
						return GL_TRUE;
					}
				}
			}
		}
	}
	return GL_FALSE;
}

GLboolean detectaColisaoMobilia(GLfloat nx, GLfloat ny, GLfloat nz)
{
	float x = 0;
	float y = 0;
	float z = 0;
	modelo.tooltip = "";
	ALint state;
	//alSourcePlay(estado.source[0]);

	for (int i = 0; i < modelo.colisoes_Mobila.size(); i++)
	{
		string info = listaMoveis[i][15];
		char *cstr = new char[info.size()];
		strcpy(cstr, info.c_str());

		alSourceStop(estado.source[1]);


		x = modelo.colisoes_Mobila[i][0] * modelo.colisoes_Mobila[i][4];
		y = modelo.colisoes_Mobila[i][1] * modelo.colisoes_Mobila[i][5];
		z = modelo.colisoes_Mobila[i][2] * modelo.colisoes_Mobila[i][6];

		if (modelo.colisoes_Mobila[i][3] == 0){
			if (((int)nx == (int)x) && ((int)nz == -(int)y) && ((int)ny == (int)z)){
				modelo.tooltip = cstr;

				if (info == "Fonte : 250Euros"){

					alSourcePlay(estado.source[1]);

				}

				return GL_TRUE;
			}

		}
		else if (modelo.colisoes_Mobila[i][3] == 90){
			if (((int)nx == -(int)y) && ((int)nz == -(int)x) && ((int)ny == (int)z)){
				modelo.tooltip = cstr;
				if (info == "Fonte : 250Euros"){
					alSourcePlay(estado.source[1]);
				}

				return GL_TRUE;
			}

		}
		else if (modelo.colisoes_Mobila[i][3] == -90){
			if (((int)nx == (int)y) && ((int)nz == (int)x) && ((int)ny == (int)z)){
				modelo.tooltip = cstr;
				if (info == "Fonte : 250Euros"){
					alSourcePlay(estado.source[1]);

				}

				return GL_TRUE;
			}

		}
		else if (modelo.colisoes_Mobila[i][3] == 180){
			if (((int)nx == -(int)x) && ((int)nz == (int)y) && ((int)ny == (int)z)){
				modelo.tooltip = cstr;
				if (info == "Fonte : 250Euros"){

					alSourcePlay(estado.source[1]);

				}

				return GL_TRUE;
			}
		}
	}
	return GL_FALSE;
}

GLboolean detectaColisaoEscada(GLfloat nx, GLfloat ny, GLfloat nz)
{
	float x1 = 8.7;
	float x2 = 10.9;
	float y1 = 0.0;
	float y2 = 3.0;
	float z1 = -9.5;
	float z2 = -5.5;
	GLboolean flag = GL_FALSE;

	if (nx > x1 &&nx < x2){
		if (ny >= y1 && ny <= y2){
			if (nz > z1 && nz < z2){
				flag = GL_TRUE;
			}
		}

	}

	return flag;
}



GLboolean detectaColisao(GLfloat nx, GLfloat ny, GLfloat nz)
{
	GLboolean b1;
	b1 = detectaColisaoMobilia(nx, ny, nz);
	GLboolean b2;
	b2 = detectaColisaoParede(nx, ny, nz);
	GLboolean b3 = GL_FALSE, b4 = GL_FALSE;

	if (nx > 25 || nx < -25){
		b3 = GL_TRUE;
	}
	if (nz > 25 || nz < -25){
		b4 = GL_TRUE;
	}
	if (b1 == GL_TRUE || b2 == GL_TRUE || b3 == GL_TRUE || b4 == GL_TRUE){
		return GL_TRUE;
	}
	return GL_FALSE;
}




/////////////////////////////////////
//Modelo

/*desenha o poligono aplicando a textura*/
void desenhaPoligonoTextura(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat  d[], GLfloat normal[], GLfloat tx, GLfloat ty, int texID)
{
	if (texID != -1){

		glBindTexture(GL_TEXTURE_2D, modelo.texturasID[texID]);

	}
	glBegin(GL_POLYGON);
	glNormal3fv(normal);
	glTexCoord2f(tx + 0, ty + 0);
	glVertex3fv(a);
	glTexCoord2f(tx + 0, ty + 0.25);
	glVertex3fv(b);
	glTexCoord2f(tx + 0.25, ty + 0.25);
	glVertex3fv(c);
	glTexCoord2f(tx + 0.25, ty + 0);
	glVertex3fv(d);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, NULL);
}

void desenhaPoligonoArray(retangulo_t parede){

	GLfloat tx = 0.75, ty = 0.75;

	glPushMatrix();

	glPushAttrib(GL_COLOR);
	glColor3f(0.7, 0.7, 0.7);
	glScalef(0.6, 0.6, 0.6);

	//glBindTexture(GL_TEXTURE_2D, modelo.texturasID[2]);

	desenhaPoligonoTextura(parede.canto[2].xyz, parede.canto[3].xyz, parede.canto[5].xyz, parede.canto[4].xyz, parede.normal[0].xyz, tx, ty, parede.texturas[0]);
	desenhaPoligonoTextura(parede.canto[4].xyz, parede.canto[5].xyz, parede.canto[7].xyz, parede.canto[6].xyz, parede.normal[1].xyz, tx, ty, parede.texturas[1]);
	desenhaPoligonoTextura(parede.canto[5].xyz, parede.canto[3].xyz, parede.canto[1].xyz, parede.canto[7].xyz, parede.normal[2].xyz, tx, ty, parede.texturas[2]);
	desenhaPoligonoTextura(parede.canto[6].xyz, parede.canto[0].xyz, parede.canto[2].xyz, parede.canto[4].xyz, parede.normal[3].xyz, tx, ty, parede.texturas[3]);
	desenhaPoligonoTextura(parede.canto[1].xyz, parede.canto[0].xyz, parede.canto[6].xyz, parede.canto[7].xyz, parede.normal[4].xyz, tx, ty, parede.texturas[4]);
	desenhaPoligonoTextura(parede.canto[0].xyz, parede.canto[1].xyz, parede.canto[3].xyz, parede.canto[2].xyz, parede.normal[5].xyz, tx, ty, parede.texturas[5]);

	glPopAttrib();

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, NULL);

}



void desenhaModeloDir(objecto_t obj, int width, int height)
{
	// Altera viewport e projecÁ„o
	glViewport(width - 60, 0, 60, 60);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glRotatef(GRAUS(obj.dir), 0, 0, 1);

	glBegin(GL_QUADS);
	glColor4f(1, 0, 0, 0.5);
	glVertex2f(5, 2.5);
	glVertex2f(-10, 2.5);
	glVertex2f(-10, -2.5);
	glVertex2f(5, -2.5);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(10, 0);
	glVertex2f(5, 5);
	glVertex2f(5, -5);
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	//repıe projecÁ„o chamando redisplay
	redisplayTopSubwindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void desenhaAngVisao(camera_t *cam)
{
	GLfloat ratio;
	ratio = (GLfloat)glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glPushMatrix();
	glTranslatef(cam->eye.x, OBJECTO_ALTURA, cam->eye.z);
	glColor4f(0, 0, 1, 0.2);
	glRotatef(GRAUS(cam->dir_long), 0, 1, 0);
	glBegin(GL_TRIANGLES);
	glVertex3f(0, 0, 0);
	glVertex3f(5 * cos(RAD(cam->fov*ratio*0.5)), 0, -5 * sin(RAD(cam->fov*ratio*0.5)));
	glVertex3f(5 * cos(RAD(cam->fov*ratio*0.5)), 0, 5 * sin(RAD(cam->fov*ratio*0.5)));
	glEnd();
	glPopMatrix();
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void desenhaMobilia()
{
	float x = 0, y = 0, z = 0, graus = 0;

	for (int i = 0; i < listaMoveis.size(); i++)
	{

		glPushMatrix();
		/*passar de string para apontador de char*/
		char *cstr = new char[listaMoveis[i][0].length() + 1];
		strcpy(cstr, listaMoveis[i][0].c_str());

		x = stof(listaMoveis[i][1]);
		y = stof(listaMoveis[i][2]);
		z = stof(listaMoveis[i][3]);

		vector<float> dados;
		dados.push_back(x);
		dados.push_back(y);
		dados.push_back(z);

		mdlviewer_init(cstr, modelo.movel);
		glScalef(x, y, z);

		graus = stof(listaMoveis[i][4]);
		x = stof(listaMoveis[i][5]);
		y = stof(listaMoveis[i][6]);
		z = stof(listaMoveis[i][7]);
		glRotatef(graus, x, y, z);

		graus = stof(listaMoveis[i][8]);
		x = stof(listaMoveis[i][9]);
		y = stof(listaMoveis[i][10]);
		z = stof(listaMoveis[i][11]);
		glRotatef(graus, x, y, z);

		dados.push_back(graus);

		x = stof(listaMoveis[i][12]);
		y = stof(listaMoveis[i][13]);
		z = stof(listaMoveis[i][14]);
		glTranslatef(x, y, z);

		dados.push_back(x);
		dados.push_back(y);
		dados.push_back(z);

		modelo.colisoes_Mobila.push_back(dados);

		mdlviewer_display(modelo.movel);

		glPopMatrix();
	}
}

void desenhaMobiliaPlanta()
{
	int piso_do_imovel;
	float x = 0, y = 0, z = 0, graus = 0;

	for (int i = 0; i < listaMoveis.size(); i++)
	{
		piso_do_imovel = stoi(listaMoveis[i][16]);
		if (piso_do_imovel == modelo.pisoAtual - 1 && piso_do_imovel != -1)
		{
			glPushMatrix();
			/*passar de string para apontador de char*/
			char *cstr = new char[listaMoveis[i][0].length() + 1];
			strcpy(cstr, listaMoveis[i][0].c_str());

			x = stof(listaMoveis[i][1]);
			y = stof(listaMoveis[i][2]);
			z = stof(listaMoveis[i][3]);

			mdlviewer_init(cstr, modelo.movel);
			glScalef(x, y, z);

			graus = stof(listaMoveis[i][4]);
			x = stof(listaMoveis[i][5]);
			y = stof(listaMoveis[i][6]);
			z = stof(listaMoveis[i][7]);
			glRotatef(graus, x, y, z);

			graus = stof(listaMoveis[i][8]);
			x = stof(listaMoveis[i][9]);
			y = stof(listaMoveis[i][10]);
			z = stof(listaMoveis[i][11]);
			glRotatef(graus, x, y, z);

			x = stof(listaMoveis[i][12]);
			y = stof(listaMoveis[i][13]);
			z = stof(listaMoveis[i][14]);
			glTranslatef(x, y, z);

			mdlviewer_display(modelo.movel);

			glPopMatrix();
		}
	}
}

void desenhaModelo()
{
	glColor3f(0, 1, 0);
	glutSolidCube(OBJECTO_ALTURA);
	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(0, OBJECTO_ALTURA*0.75, 0);
	glRotatef(GRAUS(estado.camera.dir_long - modelo.objecto.dir), 0, 1, 0);
	glutSolidCube(OBJECTO_ALTURA*0.5);
	glPopMatrix();
}

#define STEP    1

void desenhaChao(GLfloat dimensao, GLuint texID)
{
	// cÛdigo para desenhar o ch„o
	GLfloat i, j;
	glBindTexture(GL_TEXTURE_2D, texID);

	glColor3f(0.5f, 0.5f, 0.5f);
	for (i = -dimensao; i <= dimensao; i += STEP)
		for (j = -dimensao; j <= dimensao; j += STEP)
		{
			glBegin(GL_POLYGON);
			glNormal3f(0, 1, 0);
			glTexCoord2f(1, 1);
			glVertex3f(i + STEP, 0, j + STEP);
			glTexCoord2f(0, 1);
			glVertex3f(i, 0, j + STEP);
			glTexCoord2f(0, 0);
			glVertex3f(i, 0, j);
			glTexCoord2f(1, 0);
			glVertex3f(i + STEP, 0, j);
			glEnd();
		}
	glBindTexture(GL_TEXTURE_2D, NULL);
}

/////////////////////////////////////
//navigateSubwindow
void motionNavigateSubwindow(int x, int y)
{
	int dif;
	dif = y - modelo.yMouse;
	if (dif > 0){//olhar para baixo
		estado.camera.dir_lat -= dif*RAD(EYE_ROTACAO);
		if (estado.camera.dir_lat < -RAD(45))
			estado.camera.dir_lat = -RAD(45);
	}

	if (dif<0){//olhar para cima
		estado.camera.dir_lat += abs(dif)*RAD(EYE_ROTACAO);
		if (estado.camera.dir_lat>RAD(45))
			estado.camera.dir_lat = RAD(45);
	}

	dif = x - modelo.xMouse;

	if (dif > 0){ //olhar para a direita
		estado.camera.dir_long -= dif*RAD(EYE_ROTACAO);
		/*
		if(estado.camera.dir_long<modelo.objecto.dir-RAD(45))
		estado.camera.dir_long=modelo.objecto.dir-RAD(45);
		*/
	}
	if (dif < 0){//olhar para a esquerda
		estado.camera.dir_long += abs(dif)*RAD(EYE_ROTACAO);
		/*
		if(estado.camera.dir_long>modelo.objecto.dir+RAD(45))
		estado.camera.dir_long=modelo.objecto.dir+RAD(45);
		*/

	}
	modelo.xMouse = x;
	modelo.yMouse = y;

}


void mouseNavigateSubwindow(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			modelo.xMouse = x;
			modelo.yMouse = y;
			glutMotionFunc(motionNavigateSubwindow);
		}
		else
			glutMotionFunc(NULL);
	}
}

void setNavigateSubwindowCamera(camera_t *cam, objecto_t obj)
{

	pos_t center;

	if (estado.vista[JANELA_NAVIGATE])
	{
		cam->eye.x = obj.pos.x;
		cam->eye.y = obj.pos.y + 1.8;
		cam->eye.z = obj.pos.z;
		center.x = obj.pos.x + cos(cam->dir_long)*cos(cam->dir_lat);
		center.z = obj.pos.z + sin(-cam->dir_long)*cos(cam->dir_lat);
		center.y = cam->eye.y + sin(cam->dir_lat);

		gluLookAt(cam->eye.x, cam->eye.y, cam->eye.z, center.x, center.y, center.z, 0, 1, 0);
	}
	else
	{
		center.x = obj.pos.x;
		center.y = obj.pos.y + 0.5;
		center.z = obj.pos.z;

		cam->eye.x = center.x - cos(cam->dir_long);
		cam->eye.z = center.z - sin(-cam->dir_long);
		cam->eye.y = center.y + 1.5;

		gluLookAt(cam->eye.x, cam->eye.y, cam->eye.z, center.x, center.y + 1.25, center.z, 0, 1, 0);
	}
}

void redisplayAll(void)
{
	glutSetWindow(estado.mainWindow);
	glutPostRedisplay();
	glutSetWindow(estado.topSubwindow);
	glutPostRedisplay();
}

void displayMainWindow()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	setNavigateSubwindowCamera(&estado.camera, modelo.objecto);
	setLight();

	glCallList(modelo.imovel[JANELA_NAVIGATE]);


	glCallList(modelo.chao[JANELA_NAVIGATE]);

	if (!estado.vista[JANELA_NAVIGATE])
	{
		glPushMatrix();
		glTranslatef(modelo.objecto.pos.x, modelo.objecto.pos.y + 1, modelo.objecto.pos.z);
		glRotatef(GRAUS(modelo.objecto.dir), 0, 1, 0);
		glRotatef(-90, 1, 0, 0);
		glScalef(SCALE_HOMER, SCALE_HOMER, SCALE_HOMER);
		mdlviewer_display(modelo.homer[JANELA_NAVIGATE]);
		glPopMatrix();
	}

	desenhaBussola(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), modelo.tooltip);

	desenhaParticulas();

	glutSwapBuffers();
}

/////////////////////////////////////
//topSubwindow
void setTopSubwindowCamera(camera_t *cam, objecto_t obj)
{
	cam->eye.x = obj.pos.x;
	cam->eye.z = obj.pos.z;
	/*if (modelo.pisoAtual == 1){
		if (estado.vista[JANELA_TOP])
		gluLookAt(obj.pos.x, 3.0, obj.pos.z, obj.pos.x, obj.pos.y, obj.pos.z, 0, 0, 1);
		else
		gluLookAt(obj.pos.x, 3.0, obj.pos.z, obj.pos.x, obj.pos.y, obj.pos.z, 0, 0, 1);
		}
		else if (modelo.pisoAtual==2){
		if (estado.vista[JANELA_TOP])
		gluLookAt(obj.pos.x, 6.0, obj.pos.z, obj.pos.x, obj.pos.y, obj.pos.z, 0, 0, 1);
		else
		gluLookAt(obj.pos.x, 6.0, obj.pos.z, obj.pos.x, obj.pos.y, obj.pos.z, 0, 0, 1);
		}*/

	gluLookAt(camaras.camara_planta[0].xyz[0], camaras.camara_planta[0].xyz[1] + ALTURA_PAREDES*(modelo.pisoAtual - 1), camaras.camara_planta[0].xyz[2],
		camaras.camara_planta[1].xyz[0], camaras.camara_planta[1].xyz[1], camaras.camara_planta[1].xyz[2],
		camaras.camara_planta[2].xyz[0], camaras.camara_planta[2].xyz[1], camaras.camara_planta[2].xyz[2]);
}

void displayTopSubwindow()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	setTopSubwindowCamera(&estado.camera, modelo.objecto);
	setLight();

	if (piso_anterior != modelo.pisoAtual)
	{
		createDisplayLists(JANELA_TOP);
		piso_anterior = modelo.pisoAtual;
	}

	glCallList(modelo.imovel[JANELA_TOP]);
	glCallList(modelo.chao[JANELA_TOP]);

	glPushMatrix();
	glTranslatef(modelo.objecto.pos.x, modelo.objecto.pos.y, modelo.objecto.pos.z);
	glRotatef(GRAUS(modelo.objecto.dir), 0, 1, 0);
	glRotatef(-90, 1, 0, 0);
	glScalef(SCALE_HOMER, SCALE_HOMER, SCALE_HOMER);
	mdlviewer_display(modelo.homer[JANELA_TOP]);
	glPopMatrix();

	//desenhaAngVisao(&estado.camera);
	//desenhaModeloDir(modelo.objecto, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glutSwapBuffers();
}


/////////////////////////////////////l
//mainWindow



/////////////////////////////////////
//mainWindow


void Timer(int value)
{
	GLfloat nx = 0, nz = 0, ny = 0, nx1 = 0, nz1 = 0, nx2 = 0, nz2 = 0, nx3 = 0, nz3 = 0, nx4 = 0, nz4 = 0, nx5 = 0, nz5 = 0, nx6 = 0, nz6 = 0;
	GLboolean andar = GL_FALSE;

	ALint state;
	alGetSourcei(estado.source[0], AL_SOURCE_STATE, &state);
	/*alSourcef(estado.source[0], AL_GAIN, 0.5f);
	alSourcei(estado.source[0], AL_BUFFER, estado.buffer[0]);
	alSourcePlay(estado.source[0]);*/

	GLuint curr = glutGet(GLUT_ELAPSED_TIME);
	// calcula velocidade baseado no tempo passado
	float velocidade = modelo.objecto.vel*(curr - modelo.prev)*0.003;

	glutTimerFunc(estado.timer, Timer, 0);
	modelo.prev = curr;
	
	if (estado.teclas.up){
		// calcula nova posiÁ„o nx,nz
		/*colisao do centro geometrico do hommer*/
		nx = modelo.objecto.pos.x + velocidade * cos(modelo.objecto.dir);
		nz = modelo.objecto.pos.z + velocidade * sin(-modelo.objecto.dir);
		ny = modelo.objecto.pos.y;

		/*quando o hommer anda para a frente*/
		nx1 = nx + OBJECTO_RAIO * cos(-modelo.objecto.dir);
		nz1 = nz + OBJECTO_RAIO * sin(-modelo.objecto.dir);

		nx2 = nx + OBJECTO_RAIO * cos(-modelo.objecto.dir - RAD(45));
		nz2 = nz + OBJECTO_RAIO * sin(-modelo.objecto.dir - RAD(45));

		nx3 = nx + OBJECTO_RAIO * cos(-modelo.objecto.dir + RAD(45));
		nz3 = nz + OBJECTO_RAIO * sin(-modelo.objecto.dir + RAD(45));

		int tmp = 0;

		if (detectaColisao(nx, ny, nz) && detectaColisao(nx1, ny, nz1) && detectaColisao(nx2, ny, nz2) && detectaColisao(nx3, ny, nz3)){
			andar = GL_FALSE;
			estado.teclas.up = GL_FALSE;
		}
		else
		{
			if (modelo.flagSubir == 1)
			{
				if (detectaColisaoEscada(nx, ny, nz))
				{
					if (modelo.objecto.pos.x != 0){
						modelo.objecto.pos.y = modelo.objecto.pos.y + 0.052;
					}
					
					if (modelo.objecto.pos.y > 2.75)
					{
						modelo.pisoAtual = 2;

						if (modelo.objecto.pos.y >= 2.90 && modelo.objecto.pos.y < 3.0)
						{
							modelo.objecto.pos.y = 2.99;
						}
						else if (modelo.objecto.pos.y >= 2.95)
						{
							modelo.flagDescer = 1;
							modelo.flagSubir = 0;
							modelo.objecto.pos.y = 3.0;
						}
					}
				}
			}

			if (modelo.flagDescer == 1)
			{
				if (detectaColisaoEscada(nx, ny, nz))
				{
					modelo.objecto.pos.y = modelo.objecto.pos.y - 0.0515;
					if (modelo.objecto.pos.y < 1.5)
					{
						modelo.pisoAtual = 1;
						if (modelo.objecto.pos.y <= 0.5 && modelo.objecto.pos.y > 0.0)
						{
							modelo.objecto.pos.y = 0.0;
						}
						else if (modelo.objecto.pos.y <= 0.0)
						{
							modelo.flagDescer = 0;
							modelo.flagSubir = 1;
							modelo.objecto.pos.y = 0.0;
						}
					}
				}
			}
			modelo.objecto.pos.x = nx;
			modelo.objecto.pos.z = nz;
			andar = GL_TRUE;
		}
	}

	if (estado.teclas.down){
		// calcula nova posiÁ„o nx,nz
		/*colisao do centro geometrico do hommer*/
		nx = modelo.objecto.pos.x - velocidade * cos(modelo.objecto.dir);
		nz = modelo.objecto.pos.z - velocidade * sin(-modelo.objecto.dir);
		ny = modelo.objecto.pos.y;

		/*quando o hommer anda para tras*/
		nx4 = nx - OBJECTO_RAIO * cos(-modelo.objecto.dir);
		nz4 = nz - OBJECTO_RAIO * sin(-modelo.objecto.dir);

		nx5 = nx - OBJECTO_RAIO * cos(-modelo.objecto.dir - RAD(45));
		nz5 = nz - OBJECTO_RAIO * sin(-modelo.objecto.dir - RAD(45));

		nx6 = nx - OBJECTO_RAIO * cos(-modelo.objecto.dir + RAD(45));
		nz6 = nz - OBJECTO_RAIO * sin(-modelo.objecto.dir + RAD(45));

		if (detectaColisao(nx, ny, nz) && detectaColisao(nx4, ny, nz4) && detectaColisao(nx5, ny, nz5) && detectaColisao(nx6, ny, nz6)){
			andar = GL_FALSE;
			estado.teclas.up = GL_FALSE;
		}
		else{
			modelo.objecto.pos.x = nx;
			modelo.objecto.pos.z = nz;
			andar = GL_TRUE;
		}

		//andar = GL_TRUE;
	}
	if (estado.teclas.left){
		// rodar camara e objecto
		modelo.objecto.dir += RAD(OBJECTO_ROTACAO);
		estado.camera.dir_long += RAD(OBJECTO_ROTACAO);
	}
	if (estado.teclas.right){
		// rodar camara e objecto
		modelo.objecto.dir -= RAD(OBJECTO_ROTACAO);
		estado.camera.dir_long -= RAD(OBJECTO_ROTACAO);
	}

	// Sequencias - 0(parado) 3(andar) 20(choque)
	//  modelo.homer[JANELA_NAVIGATE].GetSequence()  le Sequencia usada pelo homer
	//  modelo.homer[JANELA_NAVIGATE].SetSequence()  muda Sequencia usada pelo homer

	for (int i = 0; i < listaPos.size(); i++)
	{


		/*sala*/
		if (listaPos[i][0] == "Q"){
			if (estado.teclas.Q){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.Q = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}
		/*cozinha*/
		if (listaPos[i][0] == "I"){
			if (estado.teclas.I){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.I = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}

		/*casa de banho*/
		if (listaPos[i][0] == "R"){
			if (estado.teclas.R){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.R = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}

		/*Hall*/
		if (listaPos[i][0] == "T"){
			if (estado.teclas.T){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.T = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}

		/*Garagem*/
		if (listaPos[i][0] == "Y"){
			if (estado.teclas.Y){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.Y = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}

		/*Escritorio*/
		if (listaPos[i][0] == "U"){
			if (estado.teclas.U){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.U = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}


		/*Piso 2*/
		/*Casa de Banho*/
		if (listaPos[i][0] == "F"){
			if (estado.teclas.F){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.F = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}


		/*Quarto Principal*/
		if (listaPos[i][0] == "J"){
			if (estado.teclas.J){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.J = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}

		/*Hall*/
		if (listaPos[i][0] == "K"){
			if (estado.teclas.K){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.K = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}

		/*Quarto Crianca*/
		if (listaPos[i][0] == "G"){
			if (estado.teclas.G){
				modelo.objecto.pos.x = stod(listaPos[i][1]);
				estado.camera.eye.x = stod(listaPos[i][1]);
				modelo.objecto.pos.z = stod(listaPos[i][3]);
				estado.camera.eye.z = stod(listaPos[i][3]);
				modelo.objecto.pos.y = stod(listaPos[i][2]);
				estado.camera.eye.y = stod(listaPos[i][2]);
				estado.teclas.G = GL_FALSE;
				modelo.pisoAtual = stod(listaPos[i][4]);
				modelo.flagSubir = stod(listaPos[i][5]);
				modelo.flagDescer = stod(listaPos[i][6]);
				modelo.objecto.dir = stod(listaPos[i][8]);
				estado.camera.dir_long = stod(listaPos[i][8]);
			}
		}

	}
	/*luz ambiente*/
	if (estado.teclas.N){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		GLfloat qaAmbientLight[] = { 0.8, 0.8, 0.8, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
		estado.teclas.N = GL_FALSE;
	}

	if (estado.teclas.M){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		GLfloat qaAmbientLight[] = { 0, 0, 0, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
		estado.teclas.M = GL_FALSE;
	}


	if (modelo.homer[JANELA_NAVIGATE].GetSequence() != 20) {
		ALint state;
		alGetSourcei(estado.source[0], AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING && rand() % 200 == 0)
		{
			if (andar)
				alSourcei(estado.source[0], AL_BUFFER, estado.buffer[0]);

			else
				alSourcei(estado.source[0], AL_BUFFER, estado.buffer[0]);


		alSourcePlay(estado.source[0]);
		}

		if (modelo.homer[JANELA_NAVIGATE].GetSequence() == 0 && andar) {
			modelo.homer[JANELA_NAVIGATE].SetSequence(3);
			modelo.homer[JANELA_TOP].SetSequence(3);
		}
		if (modelo.homer[JANELA_NAVIGATE].GetSequence() == 3 && !andar) {
			modelo.homer[JANELA_NAVIGATE].SetSequence(0);
			modelo.homer[JANELA_TOP].SetSequence(0);
		}
	}


	if (modelo.homer[JANELA_NAVIGATE].GetSequence() == 20 && !andar)
	{
		if (curr - modelo.tempo_anim > 1200)
		{
			modelo.homer[JANELA_NAVIGATE].SetSequence(0);
			modelo.homer[JANELA_TOP].SetSequence(0);
		}
	}

	redisplayAll();
}

void imprime_ajuda(void)
{
	printf("\n\nVISITA VIRTUAL A UM IMOVEL\n");
	printf("\n******* Movimentos do HOMER ******* \n");
	printf("     up  - Anda para a frente \n");
	printf("     down- Anda para tras \n");
	printf("     left- Anda para a direita\n");
	printf("     righ- Anda para a esquerda\n\n");
	printf("     \n******* Camara ******* \n");
	printf("     F1 - Alterna camara da janela da Esquerda \n");
	printf("     F2 - Alterna camara da janela da Direita \n");
	printf("     PAGE_UP, PAGE_DOWN - Altera abertura da camara (Zoom) \n");
	printf("     \n******* Iluminacao ******* \n");
	printf("     N - Luz ambiente ON\n");
	printf("     M - Luz ambiente OFF\n\n");
	printf("H - AJUDA \n\n");

	printf("\nPesquisa por divisoes \n\n");

	for (int i = 0; i < listaPos.size(); i++)
	{
		cout << listaPos[i][7] << "\n";
	}
	printf("     \nESC - Sair\n");

}


void Key(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(1);
		break;
	case 'h':
	case 'H':
		imprime_ajuda();
		break;
	case 'l':
	case 'L':
		estado.localViewer = !estado.localViewer;
		break;
	case 'w':
	case 'W':
		glutSetWindow(estado.navigateSubwindow);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_TEXTURE_2D);
		glutSetWindow(estado.topSubwindow);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_TEXTURE_2D);
		break;
	case 's':
	case 'S':
		glutSetWindow(estado.navigateSubwindow);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_TEXTURE_2D);
		glutSetWindow(estado.topSubwindow);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_TEXTURE_2D);
		break;
	case 'q':
	case 'Q':
		estado.teclas.Q = GL_TRUE;
		break;
	case 'r':
	case 'R':
		estado.teclas.R = GL_TRUE;
		break;
	case 't':
	case 'T':
		estado.teclas.T = GL_TRUE;
		break;
	case 'y':
	case 'Y':
		estado.teclas.Y = GL_TRUE;
		break;
	case 'u':
	case 'U':
		estado.teclas.U = GL_TRUE;
		break;
	case 'i':
	case 'I':
		estado.teclas.I = GL_TRUE;
		break;
	case 'p':
	case 'P':
		estado.teclas.P = GL_TRUE;
		break;
	case 'g':
	case 'G':
		estado.teclas.G = GL_TRUE;
		break;
	case 'f':
	case 'F':
		estado.teclas.F = GL_TRUE;
		break;
	case 'j':
	case 'J':
		estado.teclas.J = GL_TRUE;
		break;
	case 'k':
	case 'K':
		estado.teclas.K = GL_TRUE;
		break;
	case 'n':
	case 'N':
		estado.teclas.N = GL_TRUE;
		break;
	case 'm':
	case 'M':
		estado.teclas.M = GL_TRUE;
		break;
	}

}


void SpecialKey(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP: estado.teclas.up = GL_TRUE;
		break;
	case GLUT_KEY_DOWN: estado.teclas.down = GL_TRUE;
		break;
	case GLUT_KEY_LEFT: estado.teclas.left = GL_TRUE;
		break;
	case GLUT_KEY_RIGHT: estado.teclas.right = GL_TRUE;
		break;
	case GLUT_KEY_F1: estado.vista[JANELA_TOP] = !estado.vista[JANELA_TOP];
		break;
	case GLUT_KEY_F2: estado.vista[JANELA_NAVIGATE] = !estado.vista[JANELA_NAVIGATE];
		break;
	case GLUT_KEY_PAGE_UP:
		if (estado.camera.fov > 20)
		{
			estado.camera.fov--;
			glutSetWindow(estado.navigateSubwindow);
			reshapeNavigateSubwindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			redisplayAll();
		}
		break;
	case GLUT_KEY_PAGE_DOWN:
		if (estado.camera.fov < 130)
		{
			estado.camera.fov++;
			glutSetWindow(estado.navigateSubwindow);
			reshapeNavigateSubwindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			redisplayAll();
		}
		break;
	}

}
// Callback para interaccao via teclas especiais (largar na tecla)
void SpecialKeyUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP: estado.teclas.up = GL_FALSE;
		break;
	case GLUT_KEY_DOWN: estado.teclas.down = GL_FALSE;
		break;
	case GLUT_KEY_LEFT: estado.teclas.left = GL_FALSE;
		break;
	case GLUT_KEY_RIGHT: estado.teclas.right = GL_FALSE;
		break;
	}
}

////////////////////////////////////
// InicializaÁıes



void createDisplayLists(int janelaID)
{
	modelo.imovel[janelaID] = glGenLists(2);
	glNewList(modelo.imovel[janelaID], GL_COMPILE);
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
	glPushMatrix();

	if (janelaID == JANELA_TOP){
		estImovel->desenhaPlanta(modelo.pisoAtual);
		desenhaMobiliaPlanta();
	}
	else{
		estImovel->desenhaImovel();
		desenhaMobilia();
	}

	Draw_Skybox();	// Draw the Skybox

	glPopMatrix();
	glPopAttrib();
	glEndList();

	modelo.chao[janelaID] = modelo.imovel[janelaID] + 1;
	glNewList(modelo.chao[janelaID], GL_COMPILE);
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);
	desenhaChao(CHAO_DIMENSAO, modelo.texID[JANELA_NAVIGATE][modelo.texturasID[0]]);
	glPopAttrib();
	glEndList();
}


///////////////////////////////////
/// Texturas


// Só para windows (usa biblioteca glaux)
#ifdef _WIN32

AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File = NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File = fopen(Filename, "r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}
#endif

void createTextures(GLuint texID[])
{
	char *image;
	int w, h, bpp;

#ifdef _WIN32
	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	memset(TextureImage, 0, sizeof(void *) * 1);           	// Set The Pointer To NULL
#endif

	glGenTextures(NUM_TEXTURAS, texID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (read_JPEG_file("", &image, &w, &h, &bpp))
	{
		glBindTexture(GL_TEXTURE_2D, texID[ID_TEXTURA_CHAO]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	else{
		printf("Textura %s not Found\n", NOME_TEXTURA_CHAO);
		exit(0);
	}

	glBindTexture(GL_TEXTURE_2D, NULL);
}


void criarTexturas(vector<string> texturas)
{
	char *image;
	int w, h, bpp;

#ifdef _WIN32
	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	memset(TextureImage, 0, sizeof(void *) * 1);           	// Set The Pointer To NULL
#endif

	glGenTextures(texturas.size(), modelo.texturasID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (int i = 0; i < nomeTexturas.size(); i++)
	{
		char *cstr = new char[nomeTexturas[i].length() + 1];
		strcpy(cstr, nomeTexturas[i].c_str());

		if (read_JPEG_file(cstr, &image, &w, &h, &bpp))
		{
			glBindTexture(GL_TEXTURE_2D, modelo.texturasID[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
		}
	}
	glBindTexture(GL_TEXTURE_2D, NULL);
}

void init()
{
	xml.carregarXML();
	paredes_imovel = xml.getParedesPisosImovel();
	chao_imovel = xml.getChaoPisosImovel();
	teto_imovel = xml.getTetoPisosImovel();
	nomeTexturas = xml.carregarTexturasImovel();
	listaMoveis = xml.carregarMoveisImovel();
	listaLuzes = xml.carregarLuzes();
	listaPos = xml.carregarPosicoes();
	criarTexturas(nomeTexturas);
	posicao_inicial = xml.getInfoInicialPersonagem();
	skybox = xml.getInfoSkybox();
	camaras = xml.getInfoCamaras();

	vector<vector<vector<retangulo_t>>> arrayVectores[3] = {
		paredes_imovel,
		chao_imovel,
		teto_imovel
	};

	estImovel = new EstruturaImovel(arrayVectores, modelo.texturasID);

	GLfloat amb[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	estado.timer = 100;

	modelo.tooltip = "";
	modelo.pisoAtual = 1;
	modelo.flagSubir = 1;
	modelo.flagDescer = 0;

	piso_anterior = modelo.pisoAtual;

	estado.camera.eye.x = posicao_inicial.xyz[0];
	estado.camera.eye.y = posicao_inicial.xyz[1];
	estado.camera.eye.z = posicao_inicial.xyz[2];
	estado.camera.dir_long = posicao_inicial.graus;
	estado.camera.dir_lat = 0;
	estado.camera.fov = 60;

	estado.localViewer = 1;
	estado.vista[JANELA_TOP] = 0;
	estado.vista[JANELA_NAVIGATE] = 0;

	modelo.objecto.pos.x = posicao_inicial.xyz[0];
	modelo.objecto.pos.y = posicao_inicial.xyz[1];
	modelo.objecto.pos.z = posicao_inicial.xyz[2];
	modelo.objecto.dir = posicao_inicial.graus;
	modelo.objecto.vel = OBJECTO_VELOCIDADE;

	modelo.xMouse = modelo.yMouse = -1;
	modelo.andar = GL_FALSE;

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);  // por causa do Scale ao Homer

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

}

/////////////////////////////////////
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(800 + GAP * 3, 400 + GAP * 2);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	if ((estado.mainWindow = glutCreateWindow("Visita de ImÛvel")) == GL_FALSE)
		exit(1);

	// Registar callbacks do GLUT da janela principal
	init();
	setLight();
	setMaterial();

	//createTextures(modelo.texID[JANELA_NAVIGATE]);
	createDisplayLists(JANELA_NAVIGATE);
	mdlviewer_init("homer.mdl", modelo.homer[JANELA_NAVIGATE]);

	glutReshapeFunc(reshapeMainWindow);
	glutDisplayFunc(displayMainWindow);
	glutMouseFunc(mouseNavigateSubwindow);

	glutTimerFunc(estado.timer, Timer, 0);
	glutKeyboardFunc(Key);
	glutSpecialFunc(SpecialKey);
	glutSpecialUpFunc(SpecialKeyUp);

	// criar a sub window 
	estado.topSubwindow = glutCreateSubWindow(estado.mainWindow, GAP, GAP, 400, 400);
	init();
	setLight();
	setMaterial();
	//createTextures(modelo.texID[JANELA_TOP]);
	createDisplayLists(JANELA_TOP);

	mdlviewer_init("homer.mdl", modelo.homer[JANELA_TOP]);

	glutReshapeFunc(redisplayTopSubwindow);
	glutDisplayFunc(displayTopSubwindow);

	glutTimerFunc(estado.timer, Timer, 0);
	glutKeyboardFunc(Key);
	glutSpecialFunc(SpecialKey);
	glutSpecialUpFunc(SpecialKeyUp);

	srand((unsigned)time(NULL));




	alutInit(&argc, argv);
	estado.buffer[0] = alutCreateBufferFromFile("ZipFiles/sounds/beeth.wav");
	alGenSources(4, estado.source);
	alSourcei(estado.source[0], AL_BUFFER, estado.buffer[0]);

	alSourcePlay(estado.source[0]);


	estado.buffer[1] = alutCreateBufferFromFile("ZipFiles/sounds/fonte.wav");
	alGenSources(4, estado.source);
	alSourcei(estado.source[1], AL_BUFFER, estado.buffer[1]);
	
	imprime_ajuda();
	
	glutMainLoop();
	
	return 0;
}

