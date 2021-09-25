
/*  Lista Encadeada
    Criador: Lucas de Lima Castro
    Data: 01/10/2019
*/
#define SIZEOFBLOCO 4096
#define SIZEOFELEMENTO sizeof(Elemento)


using namespace std;

class Elemento { //Tipo ELemento que sera usado

    public:

        int  ID;
        string Titulo;
        int  Ano;
        string Autores;
        int  Citacoes;
        string Atualizacao;
        string Snipet;
        Elemento(int id,string titulo,int ano,string autores,int citacoes,string atualizacao,string snipet){
            ID = id;
            Titulo = titulo; //trunca para caber no campo Titulo
            Ano = ano;
            Autores = autores; //trunca para caber no campo Autores
            Citacoes = citacoes;
            Atualizacao = atualizacao; //trunca para caber no campo Atualizacao
            Snipet = snipet; //trunca para caber no campo Snipet
        };
        /*
            Função que imprime um elemento da lista encadeada.
        */
        void imprime(){
            printf("\n  Id: %d\n",ID);
            printf("  Ano: %d\n",Ano);
            printf("  Autores: %s\n",Autores.c_str());
            printf("  Citacoes:%d\n",Citacoes);
            printf("  Titulo: %s\n",Titulo.c_str());
            printf("  Atualizacao: %s\n",Atualizacao.c_str());
            printf("  Snipet: %s\n",Snipet.c_str());
        };
        ~Elemento();

};

class Bloco {

    public:

        int fatorBloco;
        int ocupacao;
        void* *dados; //vetor para guardar os elementos
        Bloco* prox; // ponteiro para o bloco de Overflow
        Bloco(){
            fatorBloco = SIZEOFBLOCO/(SIZEOFELEMENTO);
            dados = (void**) malloc(sizeof(void*)*fatorBloco);
            prox = NULL;
            ocupacao = 0;
            for(int i=0; i< fatorBloco;i++){ //limpa lixo de memoria
                dados[i] = NULL;
            }
        }
        ~Bloco();
        void insere(Elemento* NovoElemento){
            dados[ocupacao] = NovoElemento;
            ocupacao = ocupacao+1;
        };
        void imprime(){
            for(int i = 0; i < ocupacao; i++){
                Elemento* Atual = (Elemento*) dados[i];
                Atual->imprime();
            }
        };
        void* busca(int valor){
            Elemento* Aux = (Elemento*) dados[0];
            Elemento* Encontrado = NULL;
            if(Aux){
                for(int i = 0; i < ocupacao; i++){
                    Elemento* Aux = (Elemento*) dados[i];
                    if(Aux->ID == valor){
                        Encontrado = Aux;
                    }
                }
            }
            return Encontrado;
        };
};

class Bucket {

     public:

        Bloco* Prim;
        int NumeroBlocos;
        void insere(Elemento* NovoElemento);
        Bloco* removee();
        void* busca(int valor);
        void imprime();
        Bucket(){
            Prim = NULL;
            NumeroBlocos = 0;
        };
        ~Bucket();

};

void Bucket::insere(Elemento* NovoElem){
    Bloco* Aux = Prim;
    bool inserido = false;
    if(Prim == NULL){ // Se o bucket está vazio, cria um bloco
        Prim = new Bloco();
        Prim->insere(NovoElem);
        NumeroBlocos = NumeroBlocos+1;
        inserido = true;
    }
    while(!inserido){
        if(Aux->ocupacao < Aux->fatorBloco){ // vefica se tem espaco no bloco.. Se tiver, insere o elemento no bloco
            Aux->insere(NovoElem);
            inserido =1;
        }
        if(Aux->prox == NULL && !inserido){
            Aux->prox = new Bloco();
            NumeroBlocos = NumeroBlocos+1;
        }
        Aux = Aux->prox;
    }
};

void Bucket::imprime(){

    Bloco* Aux = Prim;
    while(Aux){
        Aux->imprime();
        Aux = Aux->prox;
    }
};

void* Bucket::busca(int valor){
    Bloco* Aux = (Bloco*) Prim;
    Elemento* Encontrado = NULL;
    bool achou = false;
    while(!achou && Aux){
        Encontrado = (Elemento*) Aux->busca(valor);
        if(Encontrado){
            achou = true;
        }
        Aux = Aux->prox;
    }
    return Encontrado;
};

