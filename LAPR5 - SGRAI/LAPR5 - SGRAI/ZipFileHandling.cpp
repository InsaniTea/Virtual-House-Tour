#include <iostream>
#include <stdlib.h>
#include <string>
#include <direct.h>
#include "ZipFileHandling.h"
#include "Librarys\Zip\dirent.h"

using namespace std;

ZipFileHandling::ZipFileHandling()
{
	char cCurrentPath[FILENAME_MAX];

	_getcwd(cCurrentPath, sizeof(cCurrentPath));

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';

	const string tmp(cCurrentPath);
	this->caminho_diretorio_zip = tmp + ZIP_DIRECTORY;

	verZipsDisponiveis();
}


ZipFileHandling::~ZipFileHandling()
{
}

void ZipFileHandling::verZipsDisponiveis()
{
	string ficheiro;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(this->caminho_diretorio_zip.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			ficheiro = string(ent->d_name);
			if (ficheiro.find(".zip") != std::string::npos){
				this->nomes_dos_zips.push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
}

int ZipFileHandling::selecionarZip()
{
	int cont = 1;
	printf("Selecione um ficheiro zip.\nOs disponiveis sao os seguintes:\n");
	for each (string nome_zip in nomes_dos_zips)
	{
		printf("%d - %s;\n", cont, nome_zip.c_str());
		cont++;
	}

	int posicao;
	cin >> posicao;

	while(posicao <= 0 || posicao >= cont)
	{
		printf("Escolha Invalida! -> Insira uma opcao:\n");
		cin >> posicao;
	}

	return posicao;
}

string ZipFileHandling::extractZipFile()
{
	int indice_ficheiro = selecionarZip();
	const std::string caminho_zip = this->caminho_diretorio_zip + "\\" + this->nomes_dos_zips[indice_ficheiro-1];
	const std::string comand(ZIP_COMMAND);

	const std::string comando = comand + "'" + caminho_zip + "', '" + this->caminho_diretorio_zip + "'); }\"";
	const string coma("powershell.exe -nologo -noprofile -Command \"& {cd '");
	const std::string comandoRemover = coma + this->caminho_diretorio_zip + "'; Remove-Item * -exclude *.zip}";
	if (system(NULL))
	{
		
		system(comandoRemover.c_str());
		system(comando.c_str());
	}

	string ficheiro;
	string caminho_xml;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(this->caminho_diretorio_zip.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			ficheiro = string(ent->d_name);
			if (ficheiro.find(".xml") != std::string::npos){
				caminho_xml = this->caminho_diretorio_zip + "\\" + ficheiro;
				
				closedir(dir);

				return caminho_xml;
			}
		}
		closedir(dir);
	}

	return NULL;
}