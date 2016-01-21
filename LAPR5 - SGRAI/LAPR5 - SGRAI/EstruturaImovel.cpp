#include "EstruturaImovel.h"

using namespace std;

EstruturaImovel::EstruturaImovel(vector<vector<vector<retangulo_t>>>* arrayVectores, GLuint* IDtexturas)
{
	this->paredes_pisos = arrayVectores[0];
	this->chao_pisos = arrayVectores[1];
	this->teto_pisos = arrayVectores[2];

	this->texturasID = IDtexturas;
}


EstruturaImovel::~EstruturaImovel()
{
}

/*desenha o poligono aplicando a textura*/
void EstruturaImovel::desenhaPoligonoTextura(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat  d[], GLfloat normal[], GLfloat tx, GLfloat ty, int texID)
{
	if (texID != -1){

		glBindTexture(GL_TEXTURE_2D, this->texturasID[texID]);

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


void EstruturaImovel::desenhaPoligonoArray(retangulo_t parede){

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

void EstruturaImovel::desenhaImovel()
{
	for (int num_piso = 0; num_piso < this->paredes_pisos.size(); num_piso++)
	{
		for (int num_div = 0; num_div < this->paredes_pisos[num_piso].size(); num_div++)
		{
			for each (retangulo_t parede in paredes_pisos[num_piso][num_div])
			{
				if (!parede.ref) desenhaPoligonoArray(parede);
			}

			for each (retangulo_t chao in chao_pisos[num_piso][num_div])
			{
				desenhaPoligonoArray(chao);
			}

			for each (retangulo_t teto in teto_pisos[num_piso][num_div])
			{
				desenhaPoligonoArray(teto);
			}
		}
	}
}

void EstruturaImovel::desenhaPlanta(int num_piso_atual)
{
	for (int num_piso = 0; num_piso < num_piso_atual; num_piso++)
	{
		for (int num_div = 0; num_div < this->paredes_pisos[num_piso].size(); num_div++)
		{
			for each (retangulo_t parede in paredes_pisos[num_piso][num_div])
			{
				if (!parede.ref) desenhaPoligonoArray(parede);
			}


			for each (retangulo_t chao in chao_pisos[num_piso][num_div])
			{
				desenhaPoligonoArray(chao);
			}


			if (num_piso < num_piso_atual-1)
			{
				for each (retangulo_t teto in teto_pisos[num_piso][num_div])
				{
					desenhaPoligonoArray(teto);
				}
			}
		}
	}
}