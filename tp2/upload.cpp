#include <sstream>
#include <string>
#include <fstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

/*Programa upload<file> Gera 3 Arquivos: Arquivo de Dados, Indice Prim�rio, Indice Secund�rio.

    Criador: Lucas de Lima Castro
    Data do inicio: 28/09/2019
    Data atualização: 02/10/2021;

    Consiste em 3 passos:

    1. Gera o arquivo de dados organizado pelo pela chave hash //SUCCESS

        1.1: Lê os dados do arquivo artigo.csv
        1.2: Insere cada Registro do arquivo no seu devido Bucket usando o ID para gerar a chave da Hash

    2. Gera um Indice Primario  usando B+ tree //SUCCESS
    3. Gera um Indice Secund�rio usando B+ tree //FAIL

*/

using namespace std;


/*
    Retorna um vetor de strings apartir do delimitador informado
    @param s string a ser dividida
    @param delimeter delimitador que será usado na divisão da string

*/
vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

/*
    Retorna a string sem o caractere fornecido.
    @param s string a ser limpa
    @param c caractere a ser removido
*/
string remove_caractere(string s, char c) {
   string result;
   for (size_t i = 0; i < s.size(); i++) {
        char currentChar = s[i];
        if (currentChar != c) result += currentChar;
   }
    return result;
}

/*
    Retorna a string sem a substring fornecida.
    @param de string ocorencia a ser substituida
    @param to string que ira substituir a ocorrencia
*/
std::string substitui_ocorrencias(std::string s, const std::string& de, const std::string& para) {
    size_t pos_inicial = 0;
    while((pos_inicial = s.find(de, pos_inicial)) != std::string::npos) {
        s.replace(pos_inicial, de.length(), para);
        pos_inicial += para.length();
    }
    return s;
}

/*
    Retorna a string truncada de acordo com a largura fornecida
    @param s string fornecida
    @param largura o tamanho desejado da string
*/
std::string trunca_string(std::string s, size_t largura){
    if (s.length() > largura) return s.substr(0, largura -3) + " ..";
    return s;
}

/*
    Funcão geradora dos arquivos de dados, índice primário e índice secundário
*/
void upload(){

    Hash* myhash = new Hash(1043937); //(1043937)hash com 81% de uso
    BP* bp = new BP(4);
    std::ifstream infile("artigo.csv");
    std::string line;
    if (!infile){
        printf(" - Arquivo não encontrado!\n");
        exit(1);
    }
    while (std::getline(infile, line)){
        
        std::istringstream iss(line);
        if (line.find(";;") != std::string::npos) {
            line = substitui_ocorrencias(line, std::string(";;"), std::string(";\"\";"));
        }
        if (line.find("NULL") != std::string::npos) {
            line = substitui_ocorrencias(line, std::string("NULL"), std::string("\"NULL\""));
        }
        vector<string> lineS = split(line.c_str(), "\";\"");
        
        for (int i =0;i< lineS.size();i++) lineS[i] = remove_caractere(lineS[i],'"');
        int id = atoi(lineS[0].c_str());
        string titulo = lineS[1];
        titulo = trunca_string(titulo,300);
        int ano = atoi(lineS[2].c_str());
        string autores = lineS[3];
        autores = trunca_string(autores,150);
        int citacoes = atoi(lineS[4].c_str());
        string atualizacao = lineS[5];
        string snipet = lineS[6];
        snipet = trunca_string(snipet,100);
    
        // printf("%d,%s,%d,%s,%d,%s,%s",id,titulo.c_str(),ano,autores.c_str(),citacoes,atualizacao.c_str(),snipet.c_str());
        Elemento* NovoElemento = new Elemento(id,titulo,ano,autores,citacoes,atualizacao,snipet); // Cria um novo elemento e guarda os dados do registro lido
        // NovoElemento->imprime(); //Exibe os dados do Elemento para verificação
        myhash->insere(NovoElemento->ID,NovoElemento); //Insere o Elemento na hash com base no ID
    }

    myhash->gera_arquivos();
    myhash->estatisticas(); // Exibe informaçẽes sobre a hash

}

int main(){
    printf(" Fazendo Leitura do arquivo e inserindo registros na hash..\n");
    upload();
    printf("\nTudo pronto para rodar o <findrec> e <seek1>! :)\n");
}