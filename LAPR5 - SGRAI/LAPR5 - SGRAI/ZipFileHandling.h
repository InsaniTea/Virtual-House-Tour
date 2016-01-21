#pragma once
#include <vector>
#include <string>
class ZipFileHandling
{

#define FILENAME_MAX 516
#define ZIP_DIRECTORY "\\ZipFiles"
#define ZIP_COMMAND "powershell.exe -nologo -noprofile -command \"& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::ExtractToDirectory(" 

private:
	std::string caminho_diretorio_zip;
	std::vector<std::string> nomes_dos_zips;
	void verZipsDisponiveis();
	int selecionarZip();

public:
	ZipFileHandling();
	virtual ~ZipFileHandling();
	
	std::string extractZipFile();
	void apagarFicheirosExtraidos();
};