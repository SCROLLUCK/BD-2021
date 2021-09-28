#include <stdio.h>
#include <stdlib.h>
#define SIZEOFBLOCO 4096

class No {
	public:
        void ** ponteiros;
        int * chaves;
        No * parente;
        bool folha;
        int num_chaves;
        No * prox;
};

class BP {
    private:
        int ocupacao;
        int ordem;
        No* novo_no_folha();
        No* novo_no();
        No* nova_arvore(int valor, void * ponteiro);
        No* busca_folha(No * const raiz, int chave);
        No* insere_na_folha(No * folha, int chave, void * ponteiro);
        No* insere_na_folha_apos_spit(No * raiz, No * folha, int chave, void* ponteiro);
        No* insere_no_no_apos_split(No * raiz, No * No_antigo, int indice_esquerdo, int chave, No * direita);
        No* insere_no_parente(No * raiz, No * esquerda, int chave, No * direita);
        No* insere_na_nova_raiz(No * esquerda, int chave, No * direita);
        No* insere_no_no(No * raiz, No * n, int indice_esquerdo, int chave, No * direita);
        int split_carga() {
            int aux_ordem = ordem -1;
            if (aux_ordem % 2 == 0)
                return aux_ordem/2;
            else
                return aux_ordem/2 + 1;
        }
        int obtem_indice_esquerdo(No * parente, No * esquerda) {
            int indice_esquerdo = 0;
            while (indice_esquerdo <= parente->num_chaves && parente->ponteiros[indice_esquerdo] != esquerda)
                indice_esquerdo++;
            return indice_esquerdo;
        }
    public:
		No* raiz; 
        BP(int Ordem){
            ordem = Ordem;
            raiz = NULL;
            ocupacao = 0;
        }
        ~BP();
        No* insere(No* No,int chave, void* ponteiro);
        void* busca(No * raiz, int chave);
        void imprime(BP* No);
};

No* BP::novo_no() {
	No * novo_no = (No*) malloc(sizeof(No));
	if (novo_no == NULL) {
		perror("Erro ao criar nó.");
		exit(1);
	}
	novo_no->chaves = (int*) malloc((ordem - 1) * sizeof(int));
	if (novo_no->chaves == NULL) {
		perror("Erro ao criar vetor de chaves");
		exit(1);
	}
	novo_no->ponteiros = (void**) malloc(ordem * sizeof(void *));
	if (novo_no->ponteiros == NULL) {
		perror("Erro ao criar vetor de ponteiros");
		exit(1);
	}
	novo_no->folha = false;
	novo_no->num_chaves = 0;
	novo_no->parente = NULL;
	novo_no->prox = NULL;
	return novo_no;
}

No* BP::novo_no_folha(){
    No* novo_ = novo_no();
    novo_->folha = true;
    return novo_;
}

No* BP::nova_arvore(int chave, void * pointer) {

	raiz = novo_no_folha();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = pointer;
	raiz->ponteiros[ordem - 1] = NULL;
	raiz->parente = NULL;
	raiz->num_chaves++;
	return raiz;
}

No* BP::busca_folha(No * const raiz, int chave) {
	if (raiz == NULL) {
		printf("Árvore vazia.\n");
		return raiz;
	}
	int i = 0;
	No * c = raiz;
	while (!c->folha) {
		i = 0;
		while (i < c->num_chaves) {
			if (chave >= c->chaves[i]) i++;
			else break;
		}
		c = (No *)c->ponteiros[i];
	}
	return c;
}

void* BP::busca(No * raiz, int chave) {
    
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
		return folha->ponteiros[i];
}

No* BP::insere_no_no(No * raiz, No * n, int indice_esquerdo, int chave, No * direita) {

	for (int i = n->num_chaves; i > indice_esquerdo; i--) {
		n->ponteiros[i + 1] = n->ponteiros[i];
		n->chaves[i] = n->chaves[i - 1];
	}
	n->ponteiros[indice_esquerdo + 1] = direita;
	n->chaves[indice_esquerdo] = chave;
	n->num_chaves++;
	return raiz;
}

No* BP::insere_na_folha(No * folha, int chave, void * ponteiro) {

	int i, ponto_insercao = 0;
    // Procura posição certa para inserir
	while (ponto_insercao < folha->num_chaves && folha->chaves[ponto_insercao] < chave) ponto_insercao++;

    // Abre espaço fazendo shift no vetor para poder inserir na posição
	for (i = folha->num_chaves; i > ponto_insercao; i--) {
		folha->chaves[i] = folha->chaves[i - 1];
		folha->ponteiros[i] = folha->ponteiros[i - 1];
	}

	folha->chaves[ponto_insercao] = chave;
	folha->ponteiros[ponto_insercao] = ponteiro;
	folha->num_chaves++;
	return folha;
}

No* BP::insere_na_folha_apos_spit(No * raiz, No * folha, int chave, void * ponteiro) {

	No * nova_folha;
	int * chaves_temporarias;
	void ** ponteiros_temporarios;
	int indice_insercao, split, nova_chave, i, j;

	nova_folha = novo_no_folha();

	chaves_temporarias = (int*) malloc(ordem * sizeof(int));
	if (chaves_temporarias == NULL) {
		perror("Erro ao criar vetor auxliar de chaves.");
		exit(1);
	}

	ponteiros_temporarios = (void**) malloc(ordem * sizeof(void *));
	if (ponteiros_temporarios == NULL) {
		perror("Erro ao criar vetor auxiliar de ponteiros.");
		exit(1);
	}

	indice_insercao = 0;
	while (indice_insercao < ordem - 1 && folha->chaves[indice_insercao] < chave)
		indice_insercao++;

	for (i = 0, j = 0; i < folha->num_chaves; i++, j++) {
		if (j == indice_insercao) j++;
		chaves_temporarias[j] = folha->chaves[i];
		ponteiros_temporarios[j] = folha->ponteiros[i];
	}

	chaves_temporarias[indice_insercao] = chave;
	ponteiros_temporarios[indice_insercao] = ponteiro;

	folha->num_chaves = 0;

	split = split_carga();

	for (i = 0; i < split; i++) {
		folha->ponteiros[i] = ponteiros_temporarios[i];
		folha->chaves[i] = chaves_temporarias[i];
		folha->num_chaves++;
	}

	for (i = split, j = 0; i < ordem; i++, j++) {
		nova_folha->ponteiros[j] = ponteiros_temporarios[i];
		nova_folha->chaves[j] = chaves_temporarias[i];
		nova_folha->num_chaves++;
	}

	free(ponteiros_temporarios);
	free(chaves_temporarias);

	nova_folha->ponteiros[ordem - 1] = folha->ponteiros[ordem - 1];
	folha->ponteiros[ordem - 1] = nova_folha;

	for (i = folha->num_chaves; i < ordem - 1; i++)
		folha->ponteiros[i] = NULL;
	for (i = nova_folha->num_chaves; i < ordem - 1; i++)
		nova_folha->ponteiros[i] = NULL;

	nova_folha->parente = folha->parente;
	nova_chave = nova_folha->chaves[0];

	return insere_no_parente(raiz, folha, nova_chave, nova_folha);
}

No* BP::insere_no_no_apos_split(No * raiz, No * no_antigo, int indice_esquerdo, int chave, No * direita) {

	int i, j, split, k_prime;
	No* novo_no_, *child;

	No** ponteiros_temporarios = (No**) malloc((ordem + 1) * sizeof(No *));
	if (ponteiros_temporarios == NULL) {
		perror("Erro ao criar vetor auxiliar de ponteiros.");
		exit(1);
	}
    int * chaves_temporarias = (int*) malloc(ordem * sizeof(int));
	if (chaves_temporarias == NULL) {
		perror("Erro ao criar vetor auxliar de chaves.");
		exit(1);
	}

	for (i = 0, j = 0; i < no_antigo->num_chaves + 1; i++, j++) {
		if (j == indice_esquerdo + 1) j++;
		ponteiros_temporarios[j] = (No*) no_antigo->ponteiros[i];
	}

	for (i = 0, j = 0; i < no_antigo->num_chaves; i++, j++) {
		if (j == indice_esquerdo) j++;
		chaves_temporarias[j] = no_antigo->chaves[i];
	}

	ponteiros_temporarios[indice_esquerdo + 1] = direita;
	chaves_temporarias[indice_esquerdo] = chave;

	split = split_carga();
	novo_no_ = novo_no();
	no_antigo->num_chaves = 0;
	for (i = 0; i < split - 1; i++) {
		no_antigo->ponteiros[i] = ponteiros_temporarios[i];
		no_antigo->chaves[i] = chaves_temporarias[i];
		no_antigo->num_chaves++;
	}
	no_antigo->ponteiros[i] = ponteiros_temporarios[i];
	k_prime = chaves_temporarias[split - 1];
	for (++i, j = 0; i < ordem; i++, j++) {
		novo_no_->ponteiros[j] = ponteiros_temporarios[i];
		novo_no_->chaves[j] = chaves_temporarias[i];
		novo_no_->num_chaves++;
	}
	novo_no_->ponteiros[j] = ponteiros_temporarios[i];
	free(ponteiros_temporarios);
	free(chaves_temporarias);
	novo_no_->parente = no_antigo->parente;
	for (i = 0; i <= novo_no_->num_chaves; i++) {
		child = (No*) novo_no_->ponteiros[i];
		child->parente = novo_no_;
	}

	return insere_no_parente(raiz, no_antigo, k_prime, novo_no_);
}

No* BP::insere_na_nova_raiz(No * esquerda, int chave, No * direita) {

	No* raiz = novo_no();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = esquerda;
	raiz->ponteiros[1] = direita;
	raiz->num_chaves++;
	raiz->parente = NULL;
	esquerda->parente = raiz;
	direita->parente = raiz;
	return raiz;
}

No* BP::insere_no_parente(No * raiz, No * esquerda, int chave, No * direita) {

	int indice_esquerdo;
	No * parente = esquerda->parente;

	if (parente == NULL)
		return insere_na_nova_raiz(esquerda, chave, direita);

	indice_esquerdo = obtem_indice_esquerdo(parente, esquerda);

	if (parente->num_chaves < ordem - 1)
		return insere_no_no(raiz, parente, indice_esquerdo, chave, direita);

	return insere_no_no_apos_split(raiz, parente, indice_esquerdo, chave, direita);
}

No* BP::insere(No* raiz,int chave, void* endereco){
    
    No* folha = NULL;

    // raiz vazia
	if (raiz == NULL){
		raiz = nova_arvore(chave, endereco);
        return raiz;
    }

    // raiz com espaço
	folha = busca_folha(raiz, chave);
	if (folha->num_chaves < ordem - 1) {
		folha = insere_na_folha(folha, chave, endereco);
        return folha;
	}
    
    //raiz cheia
	return insere_na_folha_apos_spit(raiz, folha, chave, endereco);
}

void BP::imprime(BP* x){};
