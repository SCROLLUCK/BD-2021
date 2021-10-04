#include <iostream>
#include <fstream>
#include <string>
#include "hash.h"
using namespace std;


int main( ){

	char *buf;
	string titulo="", autores="", atualizacao="", snipet ="";
	int id, ano, citacoes, size=0, chave = 9397, ocupacao, chave_no_arquivo, tamanho_hash, tamanho_map;

	BP* bp = new BP(4);
	bp->inicia_BP_atraves_de_arquivo(string("indice.dat"));
	ifstream input("dados.dat", ios::in | ios::binary);
	input.read(reinterpret_cast<char *>(&tamanho_hash), sizeof(int));
	tamanho_map = (tamanho_hash*(sizeof(int)*2))+sizeof(int);

	printf("Digite um ID para busca-lo no indice: ");
	while (scanf("%d", &chave) != EOF) {
		Endereco* endereco = bp->busca(bp->RAIZ,chave);
		if(endereco){
			input.seekg(endereco->valor+tamanho_map);
			//Le posicao na hash e ocupacao do bloco
            input.read(reinterpret_cast<char *>(&chave_no_arquivo), sizeof(int));
            input.read(reinterpret_cast<char *>(&ocupacao), sizeof(int));
			while(chave != id){

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
			printf("\n  Blocos lidos: 1");
			NovoElem->imprime();
			titulo = "";
			autores="";
			atualizacao="";
			snipet ="";

		}else {
			printf("Chave \"%d\" nao retornou um valor válido.",chave);
		}
		printf("\nDigite um ID para busca-lo no indice: ");
	}

	return 0;
}