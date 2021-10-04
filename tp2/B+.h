#include <stdio.h>
#include <stdlib.h>
#define SIZEOFBLOCO 4096

typedef struct endereco{
	int valor;
}Endereco;

class No {
	public:
        void ** ponteiros;
        int * chaves;
        No* pai;
        bool folha;
        int num_chaves;
        No* prox;
};

class BP {
    private:
        int ordem;
        No* cria_folha();
        No* cria_no(void);
        No* inicia_nova_arvore(int chave, Endereco* ponteiro);
        No* busca_folha(No* const raiz, int chave);
        No* insere_na_folha(No* folha, int chave, Endereco* ponteiro);
		No* insere_no_no(No* raiz, No* n, int indice_esquerdo, int chave, No* direita); 
        No* insere_no_folha_apos_split(No* raiz, No* folha, int chave, Endereco* ponteiro);
        No* insere_no_no_apos_split(No* raiz, No* no_antigo, int indice_esquerdo, int chave, No* direita);
        No* insere_no_pai(No* raiz, No* esquerda, int chave, No* direita);
        No* insere_na_nova_raiz(No* esquerda, int chave, No* direita);
        int split_carga(int tamanho) {
            if (tamanho % 2 == 0)
                return tamanho/2;
            else
                return tamanho/2 + 1;
        }
        int obtem_indice_esquerdo(No* pai, No* esquerda) {
			int indice_esquerdo = 0;
			while (indice_esquerdo <= pai->num_chaves && 
					pai->ponteiros[indice_esquerdo] != esquerda)
				indice_esquerdo++;
			return indice_esquerdo;
		}
		Endereco* cria_endereco(int valor) {
			Endereco* novo_endereco = (Endereco*) malloc(sizeof(Endereco));
			if (novo_endereco == NULL) {
				perror("Erro ao criar endereço");
				exit(1);
			}else {
				novo_endereco->valor = valor;
			}
			return novo_endereco;
		}
    public:
		No* RAIZ;
		No* queue = NULL;
        BP(int Ordem){
            ordem = Ordem;
            RAIZ = NULL;
        }
        ~BP();
        No* insere(No* raiz, int chave, int valor);
        Endereco* busca(No* raiz, int chave);
		void cria_indice();
		void inicia_BP_atraves_de_arquivo(string diretorio);
};

/*
	Encontra a folha correspondente a chave buscada
	@param raiz ponto incial da busca
	@param chave valor buscado
*/
No* BP::busca_folha(No* const raiz, int chave) {
	if (raiz == NULL) {
		printf("Arvore vazia.\n");
		return raiz;
	}
	int i = 0;
	No* c = raiz;
	while (!c->folha) {
		i = 0;
		while (i < c->num_chaves) {
			if (chave >= c->chaves[i]) i++;
			else break;
		}
		c = (No*)c->ponteiros[i];
	}
	return c;
}

/*
	Busca uma chave na BP
	@param raiz ponto incial da busca
	@param chave valor buscado
*/
Endereco* BP::busca(No* raiz, int chave) {
    if (raiz == NULL) {
        return NULL;
    }

	int i = 0;
    No* folha = NULL;

	folha = busca_folha(raiz, chave);

	for (i = 0; i < folha->num_chaves; i++)
		if (folha->chaves[i] == chave) break;
	if (i == folha->num_chaves)
		return NULL;
	else
		return (Endereco*) folha->ponteiros[i];
}
/*
	Cria novo no e o retorna
*/
No* BP::cria_no(void) {
	No* novo_no;
	novo_no = (No*) malloc(sizeof(No));
	if (novo_no == NULL) {
		perror("Erro ao criar novo No.");
		exit(1);
	}
	novo_no->chaves = (int*) malloc((ordem - 1) * sizeof(int));
	if (novo_no->chaves == NULL) {
		perror("Erro ao criar ponteiro para vetor de chaves auxiliar");
		exit(1);
	}
	novo_no->ponteiros = (void**) malloc(ordem * sizeof(void *));
	if (novo_no->ponteiros == NULL) {
		perror("Erro ao criar ponteiro para vetor de ponteiros auxiliar");
		exit(1);
	}
	novo_no->folha = false;
	novo_no->num_chaves = 0;
	novo_no->pai = NULL;
	novo_no->prox = NULL;
	return novo_no;
}
/*
	Cria novo no folha e o retorna
*/
No* BP::cria_folha(void) {
	No* folha = cria_no();
	folha->folha = true;
	return folha;
}
/*
	Insere na folha
	@param folha folha onde o novo elemento será inserido
	@param chave valor que será inserido
	@param ponteiro endereco no arquivo de dados
*/
No* BP::insere_na_folha(No* folha, int chave, Endereco* ponteiro) {

	int i, ponto_insersao;

	ponto_insersao = 0;
	while (ponto_insersao < folha->num_chaves && folha->chaves[ponto_insersao] < chave)
		ponto_insersao++;

	for (i = folha->num_chaves; i > ponto_insersao; i--) {
		folha->chaves[i] = folha->chaves[i - 1];
		folha->ponteiros[i] = folha->ponteiros[i - 1];
	}
	folha->chaves[ponto_insersao] = chave;
	folha->ponteiros[ponto_insersao] = ponteiro;
	folha->num_chaves++;
	return folha;
}

/*
	Faz split na folha cheia, insere no novo no folha e o retorna
	@param raiz ponto de pardida da insercao
	@param folha endereco da folha que está cheia
	@param chave valor que será inserido
	@param ponteiro endereco no arquivo de dados
*/
No* BP::insere_no_folha_apos_split(No* raiz, No* folha, int chave, Endereco* ponteiro) {

	No* nova_folha;
	int * temp_chaves;
	void ** temp_ponteiros;
	int indice_insercao, split, nova_chave, i, j;

	nova_folha = cria_folha();

	temp_chaves = (int*) malloc(ordem * sizeof(int));
	if (temp_chaves == NULL) {
		perror("Erro ao criar vetor de chaves temporario.");
		exit(1);
	}

	temp_ponteiros = (void**) malloc(ordem * sizeof(void *));
	if (temp_ponteiros == NULL) {
		perror("Erro ao criar vetor de ponteiros temporario.");
		exit(1);
	}

	indice_insercao = 0;
	while (indice_insercao < ordem - 1 && folha->chaves[indice_insercao] < chave)
		indice_insercao++;

	for (i = 0, j = 0; i < folha->num_chaves; i++, j++) {
		if (j == indice_insercao) j++;
		temp_chaves[j] = folha->chaves[i];
		temp_ponteiros[j] = folha->ponteiros[i];
	}

	temp_chaves[indice_insercao] = chave;
	temp_ponteiros[indice_insercao] = ponteiro;

	folha->num_chaves = 0;

	split = split_carga(ordem - 1);

	for (i = 0; i < split; i++) {
		folha->ponteiros[i] = temp_ponteiros[i];
		folha->chaves[i] = temp_chaves[i];
		folha->num_chaves++;
	}

	for (i = split, j = 0; i < ordem; i++, j++) {
		nova_folha->ponteiros[j] = temp_ponteiros[i];
		nova_folha->chaves[j] = temp_chaves[i];
		nova_folha->num_chaves++;
	}

	free(temp_ponteiros);
	free(temp_chaves);

	nova_folha->prox = folha->prox;
	folha->prox = nova_folha;

	nova_folha->ponteiros[ordem - 1] = folha->ponteiros[ordem - 1];
	folha->ponteiros[ordem - 1] = nova_folha;

	for (i = folha->num_chaves; i < ordem - 1; i++)
		folha->ponteiros[i] = NULL;
	for (i = nova_folha->num_chaves; i < ordem - 1; i++)
		nova_folha->ponteiros[i] = NULL;

	nova_folha->pai = folha->pai;
	nova_chave = nova_folha->chaves[0];

	return insere_no_pai(raiz, folha, nova_chave, nova_folha);
}

/*
	Insere no nó sem precisar fazer split ou balanceamento
	@param n no atual
	@param indice_esquerdo posicao referente ao indice esquerdo
	@param chave valor a ser inserido
	@param direita o no mais a direita do no atual
*/
No* BP::insere_no_no(No* raiz, No* n, int indice_esquerdo, int chave, No* direita) {

	for (int i = n->num_chaves; i > indice_esquerdo; i--) {
		n->ponteiros[i + 1] = n->ponteiros[i];
		n->chaves[i] = n->chaves[i - 1];
	}
	n->ponteiros[indice_esquerdo + 1] = direita;
	n->chaves[indice_esquerdo] = chave;
	n->num_chaves++;
	return raiz;
}

/*
	Faz split do nó
	@param raiz referencia para a raiz
	@param indice_esquerdo posicao referente ao indice esquerdo
	@param direita o no mais a direita do no atual
	@param no_antigo posicao referente ao indice esquerdo
	@param chave valor a ser inserido
	@param direita o no mais a direita do no atual
*/
No* BP::insere_no_no_apos_split(No* raiz, No* no_antigo, int indice_esquerdo, int chave, No* direita) {

	int i, j, split, k_prime;
	No* novo_no, * child;
	int * temp_chaves;
	No** temp_ponteiros;

	temp_ponteiros = (No**) malloc((ordem + 1) * sizeof(No*));
	if (temp_ponteiros == NULL) {
		perror("Erro ao criar vetor temporario de ponteiros");
		exit(1);
	}
	temp_chaves = (int*) malloc(ordem * sizeof(int));
	if (temp_chaves == NULL) {
		perror("Erro ao criar vetor de chaves temporario.");
		exit(1);
	}

	for (i = 0, j = 0; i < no_antigo->num_chaves + 1; i++, j++) {
		if (j == indice_esquerdo + 1) j++;
		temp_ponteiros[j] = (No*) no_antigo->ponteiros[i];
	}

	for (i = 0, j = 0; i < no_antigo->num_chaves; i++, j++) {
		if (j == indice_esquerdo) j++;
		temp_chaves[j] = no_antigo->chaves[i];
	}

	temp_ponteiros[indice_esquerdo + 1] = direita;
	temp_chaves[indice_esquerdo] = chave;

	split = split_carga(ordem);
	novo_no = cria_no();
	no_antigo->num_chaves = 0;
	for (i = 0; i < split - 1; i++) {
		no_antigo->ponteiros[i] = temp_ponteiros[i];
		no_antigo->chaves[i] = temp_chaves[i];
		no_antigo->num_chaves++;
	}
	no_antigo->ponteiros[i] = temp_ponteiros[i];
	k_prime = temp_chaves[split - 1];
	for (++i, j = 0; i < ordem; i++, j++) {
		novo_no->ponteiros[j] = temp_ponteiros[i];
		novo_no->chaves[j] = temp_chaves[i];
		novo_no->num_chaves++;
	}
	novo_no->ponteiros[j] = temp_ponteiros[i];
	free(temp_ponteiros);
	free(temp_chaves);
	novo_no->pai = no_antigo->pai;
	for (i = 0; i <= novo_no->num_chaves; i++) {
		child = (No*) novo_no->ponteiros[i];
		child->pai = novo_no;
	}

	return insere_no_pai(raiz, no_antigo, k_prime, novo_no);
}
/*
	Insere no pai subindo a chave
	@param raiz referencia para o no raiz
	@param chave valor a ser inserido
	@param esquerda o no mais a esquerda do no atual
	@param direita o no mais a direita do no atual
*/
No* BP::insere_no_pai(No* raiz, No* esquerda, int chave, No* direita) {

	int indice_esquerdo;
	No* pai;

	pai = esquerda->pai;

	if (pai == NULL)
		return insere_na_nova_raiz(esquerda, chave, direita);

	indice_esquerdo = obtem_indice_esquerdo(pai, esquerda);

	if (pai->num_chaves < ordem - 1)
		return insere_no_no(raiz, pai, indice_esquerdo, chave, direita);

	return insere_no_no_apos_split(raiz, pai, indice_esquerdo, chave, direita);
}

/*
	Insere na nova raiz e a retorna
	@param chave valor a ser inserido
	@param esquerda o no mais a esquerda do no atual
	@param direita o no mais a direita do no atual
*/
No* BP::insere_na_nova_raiz(No* esquerda, int chave, No* direita) {

	No* raiz = cria_no();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = esquerda;
	raiz->ponteiros[1] = direita;
	raiz->num_chaves++;
	raiz->pai = NULL;
	esquerda->pai = raiz;
	direita->pai = raiz;
	return raiz;
}

/*
	Cria nova arvore, insere o elemento e retorna a nova arvore para a raiz
	@param chave valor a ser inserido
	@param endereco endereco do bloco
*/
No* BP::inicia_nova_arvore(int chave, Endereco* ponteiro) {

	No* raiz = cria_folha();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = ponteiro;
	raiz->ponteiros[ordem - 1] = NULL;
	raiz->pai = NULL;
	raiz->num_chaves++;
	return raiz;
}

/*
	Insere na BP e atualiza a RAIZ
	@param raiz referencia para o no raiz
	@param chave valor a ser inserido
	@param valor endereco do bloco
*/
No* BP::insere(No* raiz, int chave, int valor) {

	Endereco* endereco = NULL;
	No* folha = NULL;

	endereco = busca(raiz, chave);
    if (endereco != NULL) {
        endereco->valor = valor;
        return raiz;
    }

	endereco = cria_endereco(valor);

	if (raiz == NULL) 
		return inicia_nova_arvore(chave, endereco);

	folha = busca_folha(raiz, chave);

	if (folha->num_chaves < ordem - 1) {
		folha = insere_na_folha(folha, chave, endereco);
		return raiz;
	}

	return insere_no_folha_apos_split(raiz, folha, chave, endereco);
}

/*
	Gera indice primário procurando o menor valor na arvore e encontrando sua folha.
*/
void BP::cria_indice(){
	No* No_menor = RAIZ;
	No* folha;
	int menor = No_menor->chaves[0];
	while(!No_menor->folha){
		menor = No_menor->chaves[0];
		No_menor = (No*) No_menor->ponteiros[0];
	}

	if(No_menor->folha) folha = (No*) No_menor;

	ofstream output ("indice.dat", ios::out | ios::binary);
	while(folha){
		for(int i=0;i<folha->num_chaves;i++){
			output.write(reinterpret_cast<char *>(&folha->chaves[i]), sizeof(int));
			output.write(reinterpret_cast<char *>(&((Endereco*) folha->ponteiros[i])->valor), sizeof(int));
			output.flush();
		}
		folha = folha->prox;
	}
	printf(" - Arquivo de indice criado.\n");
}

/*
	Inicia uma nova BP apartir do arquivo indice.dat
	@param diretorio diretorio do arquivo indice.dat
*/
void BP::inicia_BP_atraves_de_arquivo(string diretorio){
	BP* bp = new BP(4);
	if(!bp){
		perror("Erro ao criar BP.");
		exit(1);
	}
	ifstream input(diretorio, ios::in | ios::binary);
	int chave, endereco;
	if(!input){
		printf(" - Arquivo de indice não encontrado! Rode o programa \"upload.cpp\" para cria-lo.");
		exit(1);
	}
	while (!input.eof()) {
		input.read(reinterpret_cast<char *>(&chave), sizeof(int));
		input.read(reinterpret_cast<char *>(&endereco), sizeof(int));
		RAIZ = bp->insere(RAIZ, chave, endereco);
	}
}