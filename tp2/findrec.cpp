#include <iostream>
#include <fstream>
#include <string>
#include "hash.h"
using namespace std;

int main( ){

	char *buf;
	string titulo="", autores="", atualizacao="", snipet="";
	int id, ano, citacoes, size=0, valor, ocupacao, chave_no_arquivo, tamanho_hash,tamanho_map, endereco;
	ifstream input("dados.dat", ios::in | ios::binary);
	input.read(reinterpret_cast<char *>(&tamanho_hash), sizeof(int));
	tamanho_map = (tamanho_hash*(sizeof(int)*2))+sizeof(int);
	Hash* myHash = new Hash(tamanho_hash);

	printf("\nDigite um ID para busca-lo na hash: ");

	while (scanf("%d", &valor) != EOF) {
		int chave = myHash->espalha(valor);
		if(!(chave <= tamanho_hash) && (chave >= 0)){ // vefica se é uma chave válida
			printf("\n Chave: %d extrapolou o tamanho da Hash:%d",chave,tamanho_hash);
			chave = -1;
		}
		if(chave != -1){
			
			// pula para a posicao no arquivo onde está a chave do valor buscado para pegar o endereco dos dados
         	input.seekg((sizeof(int)*2)*(chave+1) - (sizeof(int)));
			input.read(reinterpret_cast<char *>(&chave_no_arquivo), sizeof(int));
			input.read(reinterpret_cast<char *>(&endereco), sizeof(int));
			
			// printf("Chave:%d Chave no arquivo: %d Endereco: %d\n",chave,chave_no_arquivo,endereco);
			if(endereco != -1){

			input.seekg(endereco+tamanho_map);
			input.read(reinterpret_cast<char *>(&chave_no_arquivo), sizeof(int));
			input.read(reinterpret_cast<char *>(&ocupacao), sizeof(int));
			// printf("Chave: %d ocupacao: %d\n",chave_no_arquivo,ocupacao);
		
			while(valor != id){

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
			printf("\n  Blocos lidos: 3");
			NovoElem->imprime();
			titulo = "";
			autores="";
			atualizacao="";
			snipet ="";
			}else{
				printf("Registro com Id:\"%d\" nao encontrado.",valor);
			}

		}else{
			printf("Chave \"%d\" nao retornou um valor válido.",chave);
		}
		printf("\nDigite um ID para busca-lo na hash: ");
	}

	return 0;
}