#include <iostream>
#include <fstream>
#include <string>
#include "Encadeada.h"
#include "B+.h"
using namespace std;


int main( ){

	char *buf;
	string titulo = "", autores="", atualizacao="", snipet ="";
	int id, ano, citacoes, size=0, chave = 9397;

	BP* bp = new BP(4);
	bp->inicia_BP_atravez_de_arquivo(string("indice.dat"));

	printf("Digite um ID para busca-lo no indice.\n");
	while (scanf("%d", &chave) != EOF) {
		Endereco* endereco = bp->busca(bp->ROOT,chave);
		if(endereco){

			while(chave != id){

				ifstream input("blocos.dat", ios::in | ios::binary);
				input.seekg(endereco->value);
				input.read(reinterpret_cast<char *>(&id), sizeof(int));
				
				input.read(reinterpret_cast<char *>(&size), sizeof(int));
				buf = new char[size];
				input.read( buf, size);
				titulo.append(buf, size);

				input.read(reinterpret_cast<char *>(&ano), sizeof(int));
				
				input.read(reinterpret_cast<char *>(&size), sizeof(int));
				buf = new char[size];
				input.read( buf, size);
				autores.append(buf, size);

				input.read(reinterpret_cast<char *>(&citacoes), sizeof(int));

				buf = new char[19];
				input.read( buf, 19);
				atualizacao.append(buf, 19);

				input.read(reinterpret_cast<char *>(&size), sizeof(int));
				buf = new char[size];
				input.read( buf, size);
				snipet.append(buf, size);

			}

			Elemento* NovoElem = new Elemento(id,titulo,ano,autores,citacoes,atualizacao,snipet);
			NovoElem->imprime();

		}else {
			printf("Chave \"%d\" nao retornou um valor válido.",chave);
		}
		
		printf("\nDigite um ID para busca-lo no indice.\n");
	}

	return 0;
}