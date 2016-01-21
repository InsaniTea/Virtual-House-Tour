#pragma once
#include "XMLParser.h"
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef _WIN32
#include <GL/glaux.h>
#endif

class EstruturaImovel
{

private:
	std::vector<std::vector<std::vector<retangulo_t>>> paredes_pisos;
	std::vector<std::vector<std::vector<retangulo_t>>> chao_pisos;
	std::vector<std::vector<std::vector<retangulo_t>>> teto_pisos;

	GLuint* texturasID; //para guardar todos os id's das texturas

	void desenhaPoligonoTextura(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat  d[], GLfloat normal[], GLfloat tx, GLfloat ty, int texID);
	void desenhaPoligonoArray(retangulo_t parede);

public:
	EstruturaImovel(std::vector<std::vector<std::vector<retangulo_t>>>* arrayVectores, GLuint* IDtexturas);
	virtual ~EstruturaImovel();

	void desenhaImovel();
	void desenhaPlanta(int num_piso_atual);
};

