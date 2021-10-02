#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Encadeada.h"
#include "B+.h"
/*  Hash com Encadeamento Aberto
    Funcão Hash : ((valor*7)%(tamanho/3))*3
    Criador: Lucas de Lima Castro
    Data: 01/10/2019
*/

using namespace std;

class Hash {
    private:
        int tamanho;
        int ocupacao;
        int colisoes;
        int NumeroBlocosPorBucket;
        int NumeroELementosPorBloco;
    public:
        void**dados;

        Hash(int Tamanho){
            tamanho = Tamanho;
            ocupacao = 0;
            dados = (void**) malloc(sizeof(void*)*Tamanho);
            for(int i=0;i<tamanho;i++){
                dados[i] = NULL;
            }
        };
        ~Hash();
        int insere(int valor,Elemento* elem);
        int espalha(int valor);
        void busca(int valor);
        void imprime();
        void gera_arquivos();
        void estatisticas();
};

int Hash::espalha(int valor){
    int chave = ((valor*7)/3)%(tamanho);
    return chave;
};

int  Hash::insere(int valor, Elemento* NovoElem){

    int chave = espalha(valor);
    if((chave <= tamanho) && (chave >= 0)){ // vefica se é uma chave válida
        if(dados[chave] == NULL){
            dados[chave] = new Bucket(chave); // Cria um novo Bucket caso a chave gerada acesse uma posicao ainda vazia
            ocupacao = ocupacao+1; //atualiza ocupacao na hash
        }else{
            colisoes = colisoes+1; // se nao for um campo vazio, atualiza o numero de colisoes
        }
        Bucket* NBucket = (Bucket*) dados[chave]; //acesso o bucket da posicao(chave)
        NBucket->insere(NovoElem); // insere o registro no Bucket
    }else{
        printf("\n Chave: %d extrapolou o tamanho da Hash:%d",chave,tamanho);
        return -1;
    }
    return chave;
};

void Hash::busca(int valor){ //busca usando ID

    int chave = espalha(valor);
    if((chave <= tamanho) && (chave >= 0)){ //verifica se a chave gerada é uma chave valida
        Bucket* NBucket = (Bucket*) dados[chave]; // pego o Bucket onde o registro possa estar
        if(NBucket == NULL){ // verifica se existe um bucket na posicao da chave
            printf("\n\n Valor: %d nao foi encontrado ",valor);
        }else{ //se a chave é valida, faz a busca do valor nesse Bucket
            Elemento* Encontrado = (Elemento*) NBucket->busca(valor);
            if(Encontrado){ // verifica se o valor retornado pela busca é valido
                Encontrado->imprime();
            }else{
                printf("\n\n Valor: %d nao foi encontrado \n",valor);
            }
        }
    }else{
        printf("\n Este valor: %d nao mapeia uma chave valida na Hash! \n",valor);
    }
};

void Hash::imprime(){
    for(int i=0; i<tamanho; i++){
        if(dados[i]){
            printf("\n| ----  posicao na hash: %d  --- |\n",i);
            Bucket* NBucket = (Bucket*) dados[i];
            NBucket->imprime();
            printf("\n| ------------------------------ |");
        }
    }
}

void Hash::estatisticas(){

    printf("\n ESTATISTICAS SOBRE A HASH");
    printf("\n - Tamanho: %d", tamanho);
    printf("\n - Ocupacao: %d",ocupacao);
    printf("\n - Numero de Colisoes: %d", colisoes);
    int maximo = 0;
    for(int i=0;i<tamanho;i++){
       if(dados[i]){
            Bucket* BBucket = (Bucket*) dados[i];
            if((BBucket->NumeroBlocos-1) > maximo){
                maximo = BBucket->NumeroBlocos-1;
            }
        }
    }
    printf("\n - Numero Maximo de Blocos de Overflow: %d\n",maximo);
};

void Hash::gera_arquivos(){

    ofstream output ("blocos.dat", ios::out | ios::binary);
    int ordem=4, endereco_invalido=-1;
    BP* bp = new BP(ordem);

    // escreve tamanho da hash
    ofstream map ("map.dat", ios::out | ios::binary);
    map.write(reinterpret_cast<char *>(&tamanho), sizeof(int));

    for(int i=0; i < tamanho; i++){ // cada posicao da hash
        if( dados[i] != NULL){
            Bucket* bucket = (Bucket*) dados[i]; 
            Bloco* bloco = bucket->Prim;
            int posicao = output.tellp();
            while(bloco){ // cada bloco
                // escreve offset do bloco chave_na_hash e ocupacao do bloco
                output.write(reinterpret_cast<char *>(&i), sizeof(int));
                output.write(reinterpret_cast<char *>(&bloco->ocupacao), sizeof(int));

                for(int k=0; k< bloco->ocupacao; k++){ // cada elemento no bloco
                    Elemento* Elem = (Elemento*) bloco->dados[k];
                    char *buf;
                    int size=0;
                    output.write(reinterpret_cast<char *>(&Elem->ID), sizeof(int));
                    
                    size = Elem->Titulo.length();
                    output.write(reinterpret_cast<char *>(&size), sizeof(int));
                    output.write(Elem->Titulo.c_str(), size);
                    
                    output.write(reinterpret_cast<char *>(&Elem->Ano), sizeof(int));
                    
                    size = Elem->Autores.length();
                    output.write(reinterpret_cast<char *>(&size), sizeof(int));
                    output.write(Elem->Autores.c_str(), size);
                    
                    output.write(reinterpret_cast<char *>(&Elem->Citacoes), sizeof(int));
                    
                    output.write(Elem->Atualizacao.c_str(), Elem->Atualizacao.length());
                    
                    size = Elem->Snipet.length();
                    output.write(reinterpret_cast<char *>(&size), sizeof(int));
                    output.write(Elem->Snipet.c_str(), size);
                    
                    output.flush(); 
                    bp->ROOT = bp->insere(bp->ROOT,Elem->ID,posicao);

                }    
                bloco = bloco->prox;
            }
            map.write(reinterpret_cast<char *>(&i), sizeof(int));
            map.write(reinterpret_cast<char *>(&posicao), sizeof(int));
            map.flush();
        }else {
            map.write(reinterpret_cast<char *>(&i), sizeof(int));
            map.write(reinterpret_cast<char *>(&endereco_invalido), sizeof(int));
            map.flush();
        }  
    }
    system("cat map.dat blocos.dat > dados.dat");
    system("rm map.dat blocos.dat");
    printf(" - Arquivo de dados criado.\n");
    bp->cria_indice(1);
}