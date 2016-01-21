#include "XMLParser.h"
#include <stdlib.h>
#include <vector>
#include "ZipFileHandling.h"

using namespace std;
using namespace tinyxml2;

XMLParser::XMLParser()
{
	//Carrega o ficheiro usando o caminho para o ficheiro xml que está definido no header
	ZipFileHandling zip;
	xmlDocumentName = zip.extractZipFile();
	xmlDoc.LoadFile(xmlDocumentName.c_str());
}

XMLParser::~XMLParser()
{
}

/*
* Metodo que lê o ficheiro xml e retorna todas as paredes (compostas pelo array "coordenadas_t[8]"),
* dentro de um vetor que agrega todas as paredes de um piso, colocando esse dentro de um vector que contem todos os piso.
*/
void XMLParser::carregarXML()
{

	//variaveis
	bool existem_pisos = true;									//para verificar quando não existem mais pisos
	bool existem_divisoes = true;									//para verificar quando não existem mais paredes naquele piso

	XMLElement* imovel = xmlDoc.RootElement();
	XMLElement* piso_atual = imovel->FirstChild()->FirstChildElement("xsi:piso");	//piso a ser trabalhado
	XMLElement* divisao_atual;

	vector<vector<retangulo_t>> paredes_do_piso;						//todas as paredes do piso atual
	vector<vector<retangulo_t>> chao_do_piso;							//chão do piso atual
	vector<vector<retangulo_t>> teto_do_piso;							//teto do piso atual

	vector<retangulo_t> paredes_da_divisao;
	vector<retangulo_t> chao_da_divisao;
	vector<retangulo_t> teto_da_divisao;

	carregarInformacaoGlut(imovel);

	while (existem_pisos){

		divisao_atual = piso_atual->FirstChild()->FirstChildElement("xsi:divisao");
		existem_divisoes = true;

		while (existem_divisoes){
			paredes_da_divisao = todasParedesDivisao(divisao_atual);

			if (divisao_atual->FirstChildElement("xsi:chao")) chao_da_divisao = chaoDivisao(divisao_atual);

			teto_da_divisao = tetoDivisao(divisao_atual);

			paredes_do_piso.push_back(paredes_da_divisao);
			chao_do_piso.push_back(chao_da_divisao);
			teto_do_piso.push_back(teto_da_divisao);

			divisao_atual = divisao_atual->NextSiblingElement("xsi:divisao");
			(divisao_atual == NULL) ? (existem_divisoes = false) : (existem_divisoes = true);
		}

		this->vector_paredes_pisos.push_back(paredes_do_piso);
		this->vector_chao_pisos.push_back(chao_do_piso);
		this->vector_teto_pisos.push_back(teto_do_piso);

		paredes_do_piso.clear();
		chao_do_piso.clear();
		teto_do_piso.clear();

		piso_atual = piso_atual->NextSiblingElement("xsi:piso");
		(piso_atual == NULL) ? (existem_pisos = false) : (existem_pisos = true);
	}

}

vector<retangulo_t> XMLParser::todasParedesDivisao(tinyxml2::XMLElement* divisao)
{
	//Variaveis
	bool existem_paredes = true;									//para verificar quando não existem mais paredes naquele piso

	XMLElement* parede_atual = divisao->FirstChildElement("xsi:paredes")->FirstChildElement("xsi:parede");
	//parede atual
	vector<retangulo_t> paredes;									//todas as paredes do piso -> retorno
	retangulo_t parede;						//parede atual já em forma de template


	while (existem_paredes){
		parede = converterParedeXML(parede_atual);

		paredes.push_back(parede);

		parede_atual = parede_atual->NextSiblingElement("xsi:parede");
		(parede_atual == NULL) ? (existem_paredes = false) : (existem_paredes = true);
	}

	return paredes;
}

vector<retangulo_t> XMLParser::chaoDivisao(tinyxml2::XMLElement* divisao)
{
	//Variaveis
	bool existem_porcoes = true;			//para verificar quando não existem porcoes de chao naquele piso

	XMLElement* porcao_atual = divisao->FirstChildElement("xsi:chao")->FirstChildElement("xsi:porcao");
	//porcao de chao atual

	vector<retangulo_t> porcoes;			//todas as porções de chão do piso -> retorno
	retangulo_t porcao;						//porção atual já em forma de template


	while (existem_porcoes){
		porcao = converterParedeXML(porcao_atual);

		porcoes.push_back(porcao);

		porcao_atual = porcao_atual->NextSiblingElement("xsi:porcao");
		(porcao_atual == NULL) ? (existem_porcoes = false) : (existem_porcoes = true);
	}

	return porcoes;
}

vector<retangulo_t> XMLParser::tetoDivisao(tinyxml2::XMLElement* divisao)
{
	//Variaveis
	bool existem_porcoes = true;			//para verificar quando não existem mais porcoes de teto naquele piso

	XMLElement* porcao_atual = divisao->FirstChildElement("xsi:teto")->FirstChildElement("xsi:porcao");
	//porcao de teto atual

	vector<retangulo_t> porcoes;			//todas as porções de teto do piso -> retorno
	retangulo_t porcao;						//porção atual já em forma de template


	while (existem_porcoes){
		porcao = converterParedeXML(porcao_atual);

		porcoes.push_back(porcao);

		porcao_atual = porcao_atual->NextSiblingElement("xsi:porcao");
		(porcao_atual == NULL) ? (existem_porcoes = false) : (existem_porcoes = true);
	}

	return porcoes;
}

retangulo_t XMLParser::converterParedeXML(XMLElement* parede_atual)
{
	//Variaveis
	retangulo_t parede;	//parede na estrutura correta para retornar

	if (!parede_atual->Attribute("IDR"))
	{
		XMLElement* posicaoDentro;		//elemento com as coordenadas da parte de dentro de um dos cantos
		XMLElement* posicaoFora;		//elemento com as coordenadas da parte de fora de um dos cantos
		XMLElement* canto_atual = parede_atual->FirstChildElement("xsi:cantos")->FirstChildElement("xsi:canto");
		for (int j = 0; j < 4; j++)
		{
			posicaoDentro = canto_atual->FirstChildElement("xsi:posicaoDentro");
			posicaoFora = canto_atual->FirstChildElement("xsi:posicaoFora");

			parede.canto[j * 2] = converterCoordenadasParaEstrutura(posicaoDentro);
			parede.canto[j * 2 + 1] = converterCoordenadasParaEstrutura(posicaoFora);

			canto_atual = canto_atual->NextSiblingElement("xsi:canto");
		}

		XMLElement* normal_atual = parede_atual->FirstChildElement("xsi:normais")->FirstChildElement("xsi:normal");
		for (int i = 0; i < 6; i++)
		{
			parede.normal[i] = converterCoordenadasParaEstrutura(normal_atual);

			if (normal_atual->Attribute("textura")){
				parede.texturas[i] = atof(normal_atual->Attribute("textura"));
			}
			else{
				parede.texturas[i] = -1;
			}

			normal_atual = normal_atual->NextSiblingElement("xsi:normal");
		}
		parede.id = parede_atual->Attribute("ID");
		parede.ref = false;
	}
	else
	{
		parede.id = parede_atual->Attribute("IDR");
		parede.ref = true;
	}

	return parede;
}

vector<vector<vector<retangulo_t>>> XMLParser::getParedesPisosImovel()
{
	return this->vector_paredes_pisos;
}

vector<vector<vector<retangulo_t>>>XMLParser::getChaoPisosImovel()
{
	return this->vector_chao_pisos;
}

vector<vector<vector<retangulo_t>>> XMLParser::getTetoPisosImovel()
{
	return this->vector_teto_pisos;
}

/*Moveis*/

vector<vector<string>> XMLParser::carregarMoveisImovel(){
	//Variaveis
	bool existem_moveis = true;		//para verificar quando não existem mais moveis
	XMLElement* imovel = xmlDoc.RootElement();
	XMLElement* moveis = imovel->FirstChildElement("xsi:mobilia");
	XMLElement* movel = moveis->FirstChildElement("xsi:movel"); //ir buscar o primeiro movel

	vector<vector<string>> listaMoveis;
	vector<string> peca;

	while (existem_moveis){
		peca = converterMoveisXML(movel);

		listaMoveis.push_back(peca);

		movel = movel->NextSiblingElement("xsi:movel");
		(movel == NULL) ? (existem_moveis = false) : (existem_moveis = true);
	}


	return listaMoveis;
}



vector<string> XMLParser::converterMoveisXML(XMLElement* movel){
	vector<string> info;
	XMLElement* path;
	XMLElement* scale;
	XMLElement* rotate;
	XMLElement* rotate2;
	XMLElement* translate;
	XMLElement* informacao;
	XMLElement* piso;
	string valor;

	path = movel->FirstChildElement("xsi:path");
	scale = movel->FirstChildElement("xsi:scale");
	rotate = movel->FirstChildElement("xsi:rotate");
	rotate2 = movel->FirstChildElement("xsi:rotate2");
	translate = movel->FirstChildElement("xsi:translate");
	informacao = movel->FirstChildElement("xsi:info");
	piso = movel->FirstChildElement("xsi:piso");

	/*path*/
	valor = path->GetText();
	info.push_back(valor);
	/*scale*/
	valor = scale->Attribute("X");
	info.push_back(valor);
	valor = scale->Attribute("Y");
	info.push_back(valor);
	valor = scale->Attribute("Z");
	info.push_back(valor);
	/*rotate*/
	valor = rotate->Attribute("G");
	info.push_back(valor);
	valor = rotate->Attribute("X");
	info.push_back(valor);
	valor = rotate->Attribute("Y");
	info.push_back(valor);
	valor = rotate->Attribute("Z");
	info.push_back(valor);
	/*rotate2*/
	valor = rotate2->Attribute("G");
	info.push_back(valor);
	valor = rotate2->Attribute("X");
	info.push_back(valor);
	valor = rotate2->Attribute("Y");
	info.push_back(valor);
	valor = rotate2->Attribute("Z");
	info.push_back(valor);
	/*translate*/
	valor = translate->Attribute("X");
	info.push_back(valor);
	valor = translate->Attribute("Y");
	info.push_back(valor);
	valor = translate->Attribute("Z");
	info.push_back(valor);
	/*informacao*/
	valor = informacao->GetText();
	info.push_back(valor);

	valor = piso->GetText();
	info.push_back(valor);


	return info;
}

/*texturas*/

vector<string> XMLParser::carregarTexturasImovel(){
	//Variaveis
	bool existem_texturas = true;		//para verificar quando não existem mais texturas
	XMLElement* imovel = xmlDoc.RootElement();
	XMLElement* texturas = imovel->FirstChildElement("xsi:texturas");
	XMLElement* textura = texturas->FirstChildElement("xsi:textura"); //ir buscar a primeira textura

	vector<string> listaNomes;
	string nome;

	while (existem_texturas){
		nome = converterTexturaXML(textura);

		listaNomes.push_back(nome);

		textura = textura->NextSiblingElement("xsi:textura");
		(textura == NULL) ? (existem_texturas = false) : (existem_texturas = true);
	}


	return listaNomes;
}



string XMLParser::converterTexturaXML(XMLElement* movel){
	string nome;

	nome = movel->GetText();

	return nome;
}


coordenadas_t XMLParser::converterCoordenadasParaEstrutura(XMLElement* posicao)
{
	coordenadas_t parede;
	parede.xyz[0] = atof(posicao->Attribute("X"));
	parede.xyz[1] = atof(posicao->Attribute("Y"));
	parede.xyz[2] = atof(posicao->Attribute("Z"));

	if (posicao->Attribute("G"))
	{
		parede.graus = atof(posicao->Attribute("G"));
	}

	return parede;
}


/*Luzes*/

vector<vector<string>> XMLParser::carregarLuzes(){
	//Variaveis
	bool existem_luzes = true;		//para verificar quando não existem mais luzes
	XMLElement* imovel = xmlDoc.RootElement();
	XMLElement* luzes = imovel->FirstChildElement("xsi:luzes");
	XMLElement* luz = luzes->FirstChildElement("xsi:luz"); //ir buscar a primeira Luz

	vector<vector<string>> listaLuzes;
	vector<string> luzDados;

	while (existem_luzes){
		luzDados = converterLuzXML(luz);

		listaLuzes.push_back(luzDados);

		luz = luz->NextSiblingElement("xsi:luz");
		(luz == NULL) ? (existem_luzes = false) : (existem_luzes = true);
	}


	return listaLuzes;
}



vector<string> XMLParser::converterLuzXML(XMLElement* luz){
	vector<string> luzDados;

	XMLElement* xl;
	XMLElement* yl;
	XMLElement* zl;
	XMLElement* xd;
	XMLElement* yd;
	XMLElement* zd;
	XMLElement* xt;
	XMLElement* yt;
	XMLElement* zt;
	XMLElement* at;
	string valor;

	xl = luz->FirstChildElement("xsi:XL");
	yl = luz->FirstChildElement("xsi:YL");
	zl = luz->FirstChildElement("xsi:ZL");
	xd = luz->FirstChildElement("xsi:XD");
	yd = luz->FirstChildElement("xsi:YD");
	zd = luz->FirstChildElement("xsi:ZD");
	xt = luz->FirstChildElement("xsi:XT");
	yt = luz->FirstChildElement("xsi:YT");
	zt = luz->FirstChildElement("xsi:ZT");
	at = luz->FirstChildElement("xsi:AT");


	luzDados.push_back(xl->GetText());
	luzDados.push_back(yl->GetText());
	luzDados.push_back(zl->GetText());
	luzDados.push_back(xd->GetText());
	luzDados.push_back(yd->GetText());
	luzDados.push_back(zd->GetText());
	luzDados.push_back(xt->GetText());
	luzDados.push_back(yt->GetText());
	luzDados.push_back(zt->GetText());
	luzDados.push_back(at->GetText());

	return luzDados;
}



/*Posicoes*/

vector<vector<string>> XMLParser::carregarPosicoes(){
	//Variaveis
	bool existem_posicoes = true;		//para verificar quando não existem mais posicoes
	XMLElement* imovel = xmlDoc.RootElement();
	XMLElement* posicoes = imovel->FirstChildElement("xsi:posicoes");
	XMLElement* posicao = posicoes->FirstChildElement("xsi:posicao"); //ir buscar a primeira Luz

	vector<vector<string>> listaPosicoes;
	vector<string> pos;

	while (existem_posicoes){
		pos = converterPosicaoXML(posicao);

		listaPosicoes.push_back(pos);

		posicao = posicao->NextSiblingElement("xsi:posicao");
		(posicao == NULL) ? (existem_posicoes = false) : (existem_posicoes = true);
	}


	return listaPosicoes;
}



vector<string> XMLParser::converterPosicaoXML(XMLElement* pos){
	vector<string> posicaoDados;

	XMLElement* tx;
	XMLElement* ty;
	XMLElement* tz;
	XMLElement* pa;
	XMLElement* fs;
	XMLElement* fd;
	XMLElement* frase;
	XMLElement* gr;
	string valor;

	tx = pos->FirstChildElement("xsi:TX");
	ty = pos->FirstChildElement("xsi:TY");
	tz = pos->FirstChildElement("xsi:TZ");
	pa = pos->FirstChildElement("xsi:PA");
	fs = pos->FirstChildElement("xsi:FS");
	fd = pos->FirstChildElement("xsi:FD");
	frase = pos->FirstChildElement("xsi:Frase");
	gr = pos->FirstChildElement("xsi:Gr");
	
	valor = pos->Attribute("ID");
	posicaoDados.push_back(valor);

	posicaoDados.push_back(tx->GetText());
	posicaoDados.push_back(ty->GetText());
	posicaoDados.push_back(tz->GetText());
	posicaoDados.push_back(pa->GetText());
	posicaoDados.push_back(fs->GetText());
	posicaoDados.push_back(fd->GetText());
	posicaoDados.push_back(frase->GetText());
	posicaoDados.push_back(gr->GetText());

	return posicaoDados;
}

void XMLParser::carregarInfoInicialPersonagem(XMLElement* imovel)
{

	XMLElement *personagem = imovel->FirstChildElement("xsi:informacao")->FirstChildElement("xsi:personagem");

	this->posicao_inicial_personagem = converterCoordenadasParaEstrutura(personagem->FirstChildElement("xsi:posicaoInicial"));

}

void XMLParser::carregarInfoSkybox(tinyxml2::XMLElement* imovel)
{
	XMLElement *skybox = imovel->FirstChildElement("xsi:informacao")->FirstChildElement("xsi:skybox");
	
	this->skybox.push_back(
		converterCoordenadasParaEstrutura(skybox->FirstChildElement("xsi:centro"))
	);
	this->skybox.push_back(
		converterCoordenadasParaEstrutura(skybox->FirstChildElement("xsi:dimensoes"))
	);
}

void XMLParser::carregarInfoCamaras(tinyxml2::XMLElement* imovel)
{
	XMLElement *camara_planta = imovel->FirstChildElement("xsi:informacao")->FirstChildElement("xsi:camara_planta");
	XMLElement *camara_navegacao = imovel->FirstChildElement("xsi:informacao")->FirstChildElement("xsi:camara_navegacao");

	this->camaras.camara_planta.push_back(
		converterCoordenadasParaEstrutura(camara_planta->FirstChildElement("xsi:eye"))
		);
	this->camaras.camara_planta.push_back(
		converterCoordenadasParaEstrutura(camara_planta->FirstChildElement("xsi:center"))
		);
	this->camaras.camara_planta.push_back(
		converterCoordenadasParaEstrutura(camara_planta->FirstChildElement("xsi:up"))
		);

	this->camaras.zfar_planta = atof(camara_planta->Attribute("zfar"));
	this->camaras.zfar_navegacao = atof(camara_navegacao->Attribute("zfar"));

}

void XMLParser::carregarInformacaoGlut(tinyxml2::XMLElement* imovel)
{
	carregarInfoInicialPersonagem(imovel);
	carregarInfoSkybox(imovel);
	carregarInfoCamaras(imovel);
}

coordenadas_t XMLParser::getInfoInicialPersonagem()
{
	return this->posicao_inicial_personagem;
}

vector<coordenadas_t> XMLParser::getInfoSkybox()
{
	return this->skybox;
}

camaras_t XMLParser::getInfoCamaras()
{
	return this->camaras;
}