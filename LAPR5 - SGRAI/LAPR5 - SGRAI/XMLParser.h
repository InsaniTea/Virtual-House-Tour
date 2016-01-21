#pragma once
#include <XML\tinyxml2.h>
#include <vector>
#include <string>

typedef struct coordenadas_t{
	float xyz[3];
	float graus;
}coordenadas_t;

typedef struct parede_t{
	coordenadas_t canto[8];
	coordenadas_t normal[6];
	int texturas[6];
	const char* id;
	bool ref;
}retangulo_t;

typedef struct camaras_t{
	std::vector<coordenadas_t> camara_planta;
	float zfar_planta;
	float zfar_navegacao;
}camaras_t;

class XMLParser
{
private:
	std::string xmlDocumentName;
	tinyxml2::XMLDocument xmlDoc;

	std::vector<std::vector<std::vector<retangulo_t>>> vector_paredes_pisos;
	std::vector<std::vector<std::vector<retangulo_t>>> vector_chao_pisos;
	std::vector<std::vector<std::vector<retangulo_t>>> vector_teto_pisos;

	coordenadas_t posicao_inicial_personagem;

	std::vector<coordenadas_t> skybox;
	camaras_t camaras;

	std::vector<std::vector<retangulo_t>> todasParedesPiso(tinyxml2::XMLElement* piso);

	std::vector<retangulo_t> todasParedesDivisao(tinyxml2::XMLElement* divisao);
	std::vector<retangulo_t> chaoDivisao(tinyxml2::XMLElement* divisao);
	std::vector<retangulo_t> tetoDivisao(tinyxml2::XMLElement* divisao);

	retangulo_t converterParedeXML(tinyxml2::XMLElement* parede_atual);
	coordenadas_t converterCoordenadasParaEstrutura(tinyxml2::XMLElement* posicao);

	std::vector<std::string> converterMoveisXML(tinyxml2::XMLElement* movel);
	std::string converterTexturaXML(tinyxml2::XMLElement* movel);
	std::vector<std::string> converterLuzXML(tinyxml2::XMLElement* luz);
	std::vector<std::string> converterPosicaoXML(tinyxml2::XMLElement* pos);

	void carregarInformacaoGlut(tinyxml2::XMLElement* imovel);

	void carregarInfoInicialPersonagem(tinyxml2::XMLElement* imovel);
	void carregarInfoSkybox(tinyxml2::XMLElement* imovel);
	void carregarInfoCamaras(tinyxml2::XMLElement* imovel);

public:
	XMLParser();
	~XMLParser();

	std::vector<std::vector<std::vector<retangulo_t>>> getParedesPisosImovel();
	std::vector<std::vector<std::vector<retangulo_t>>> getChaoPisosImovel();
	std::vector<std::vector<std::vector<retangulo_t>>> getTetoPisosImovel();

	void carregarXML();

	std::vector<std::vector<std::string>> carregarMoveisImovel();
	std::vector<std::string> carregarTexturasImovel();
	std::vector<std::vector<std::string>> carregarLuzes();
	std::vector<std::vector<std::string>> carregarPosicoes();

	coordenadas_t getInfoInicialPersonagem();
	std::vector<coordenadas_t> getInfoSkybox();
	camaras_t getInfoCamaras();
};
 
