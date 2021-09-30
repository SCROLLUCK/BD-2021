#include <stdio.h>
#include <stdlib.h>
#define SIZEOFBLOCO 4096

typedef struct rec{
	int value;
}Endereco;

class No {
	public:
        void ** ponteiros;
        int * chaves;
        No* parente;
        bool folha;
        int num_chaves;
        No* prox;
};

class BP {
    private:
        int ocupacao;
        int ordem;
        No* cria_folha();
        No* cria_no(void);
        No* inicia_nova_arvore(int chave, Endereco* ponteiro);
        No* busca_folha(No* const raiz, int chave);
        No* insere_na_folha(No* folha, int chave, Endereco* ponteiro);
		No* insere_no_no(No* raiz, No* n, int indice_esquerdo, int chave, No* direita); 
        No* insere_no_folha_apos_split(No* raiz, No* folha, int chave, Endereco* ponteiro);
        No* insere_no_no_apos_split(No* raiz, No* old_node, int indice_esquerdo, int chave, No* direita);
        No* insere_no_parente(No* raiz, No* left, int chave, No* direita);
        No* insere_na_nova_raiz(No* left, int chave, No* direita);
        int cut(int tamanho) {
            if (tamanho % 2 == 0)
                return tamanho/2;
            else
                return tamanho/2 + 1;
        }
        int obtem_indice_esquerdo(No* parente, No* left) {
			int indice_esquerdo = 0;
			while (indice_esquerdo <= parente->num_chaves && 
					parente->ponteiros[indice_esquerdo] != left)
				indice_esquerdo++;
			return indice_esquerdo;
		}
		Endereco* cria_endereco(int value) {
			Endereco* novo_endereco = (Endereco*) malloc(sizeof(Endereco));
			if (novo_endereco == NULL) {
				perror("Erro ao criar endereço");
				exit(1);
			}else {
				novo_endereco->value = value;
			}
			return novo_endereco;
		}
    public:
		No* ROOT;
		No* queue = NULL;
        BP(int Ordem){
            ordem = Ordem;
            ROOT = NULL;
            ocupacao = 0;
        }
        ~BP();
        No* insere(No* raiz, int chave, int value);
        Endereco* busca(No* raiz, int chave);
		void cria_indice(int chave);
		void inicia_BP_atravez_de_arquivo(string diretorio);
};

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

Endereco* make_record(int value) {
	Endereco* novo_endereco = (Endereco*)malloc(sizeof(Endereco));
	if (novo_endereco == NULL) {
		perror("Endereco creation.");
		exit(1);
	}
	else {
		novo_endereco->value = value;
	}
	return novo_endereco;
}

No* BP::cria_no(void) {
	No* new_node;
	new_node = (No*) malloc(sizeof(No));
	if (new_node == NULL) {
		perror("No creation.");
		exit(1);
	}
	new_node->chaves = (int*) malloc((ordem - 1) * sizeof(int));
	if (new_node->chaves == NULL) {
		perror("New No chaves array.");
		exit(1);
	}
	new_node->ponteiros = (void**) malloc(ordem * sizeof(void *));
	if (new_node->ponteiros == NULL) {
		perror("New No ponteiros array.");
		exit(1);
	}
	new_node->folha = false;
	new_node->num_chaves = 0;
	new_node->parente = NULL;
	new_node->prox = NULL;
	return new_node;
}

No* BP::cria_folha(void) {
	No* folha = cria_no();
	folha->folha = true;
	return folha;
}

No* BP::insere_na_folha(No* folha, int chave, Endereco* ponteiro) {

	int i, insereion_point;

	insereion_point = 0;
	while (insereion_point < folha->num_chaves && folha->chaves[insereion_point] < chave)
		insereion_point++;

	for (i = folha->num_chaves; i > insereion_point; i--) {
		folha->chaves[i] = folha->chaves[i - 1];
		folha->ponteiros[i] = folha->ponteiros[i - 1];
	}
	folha->chaves[insereion_point] = chave;
	folha->ponteiros[insereion_point] = ponteiro;
	folha->num_chaves++;
	return folha;
}

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

	split = cut(ordem - 1);

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

	folha->prox = nova_folha;

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

No* BP::insere_no_no(No* raiz, No* n, int indice_esquerdo, int chave, No* direita) {
	int i;

	for (i = n->num_chaves; i > indice_esquerdo; i--) {
		n->ponteiros[i + 1] = n->ponteiros[i];
		n->chaves[i] = n->chaves[i - 1];
	}
	n->ponteiros[indice_esquerdo + 1] = direita;
	n->chaves[indice_esquerdo] = chave;
	n->num_chaves++;
	return raiz;
}

No* BP::insere_no_no_apos_split(No* raiz, No* old_node, int indice_esquerdo, int chave, No* direita) {

	int i, j, split, k_prime;
	No* new_node, * child;
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

	for (i = 0, j = 0; i < old_node->num_chaves + 1; i++, j++) {
		if (j == indice_esquerdo + 1) j++;
		temp_ponteiros[j] = (No*) old_node->ponteiros[i];
	}

	for (i = 0, j = 0; i < old_node->num_chaves; i++, j++) {
		if (j == indice_esquerdo) j++;
		temp_chaves[j] = old_node->chaves[i];
	}

	temp_ponteiros[indice_esquerdo + 1] = direita;
	temp_chaves[indice_esquerdo] = chave;

	split = cut(ordem);
	new_node = cria_no();
	old_node->num_chaves = 0;
	for (i = 0; i < split - 1; i++) {
		old_node->ponteiros[i] = temp_ponteiros[i];
		old_node->chaves[i] = temp_chaves[i];
		old_node->num_chaves++;
	}
	old_node->ponteiros[i] = temp_ponteiros[i];
	k_prime = temp_chaves[split - 1];
	for (++i, j = 0; i < ordem; i++, j++) {
		new_node->ponteiros[j] = temp_ponteiros[i];
		new_node->chaves[j] = temp_chaves[i];
		new_node->num_chaves++;
	}
	new_node->ponteiros[j] = temp_ponteiros[i];
	free(temp_ponteiros);
	free(temp_chaves);
	new_node->parente = old_node->parente;
	for (i = 0; i <= new_node->num_chaves; i++) {
		child = (No*) new_node->ponteiros[i];
		child->parente = new_node;
	}

	return insere_no_parente(raiz, old_node, k_prime, new_node);
}

No* BP::insere_no_parente(No* raiz, No* left, int chave, No* direita) {

	int indice_esquerdo;
	No* parente;

	parente = left->parente;

	if (parente == NULL)
		return insere_na_nova_raiz(left, chave, direita);

	indice_esquerdo = obtem_indice_esquerdo(parente, left);

	if (parente->num_chaves < ordem - 1)
		return insere_no_no(raiz, parente, indice_esquerdo, chave, direita);

	return insere_no_no_apos_split(raiz, parente, indice_esquerdo, chave, direita);
}

No* BP::insere_na_nova_raiz(No* left, int chave, No* direita) {

	No* raiz = cria_no();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = left;
	raiz->ponteiros[1] = direita;
	raiz->num_chaves++;
	raiz->parente = NULL;
	left->parente = raiz;
	direita->parente = raiz;
	return raiz;
}

No* BP::inicia_nova_arvore(int chave, Endereco* ponteiro) {

	No* raiz = cria_folha();
	raiz->chaves[0] = chave;
	raiz->ponteiros[0] = ponteiro;
	raiz->ponteiros[ordem - 1] = NULL;
	raiz->parente = NULL;
	raiz->num_chaves++;
	return raiz;
}

No* BP::insere(No* raiz, int chave, int value) {

	Endereco* record_ponteiro = NULL;
	No* folha = NULL;

	record_ponteiro = busca(raiz, chave);
    if (record_ponteiro != NULL) {

        record_ponteiro->value = value;
        return raiz;
    }

	record_ponteiro = make_record(value);

	if (raiz == NULL) 
		return inicia_nova_arvore(chave, record_ponteiro);

	folha = busca_folha(raiz, chave);

	if (folha->num_chaves < ordem - 1) {
		folha = insere_na_folha(folha, chave, record_ponteiro);
		return raiz;
	}

	return insere_no_folha_apos_split(raiz, folha, chave, record_ponteiro);
}

void BP::cria_indice(int chave){
	No* folha = busca_folha(ROOT,chave);
	ofstream output ("indice.dat", ios::out | ios::binary);
	while(folha){
		for(int i=0;i<folha->num_chaves;i++){
			output.write(reinterpret_cast<char *>(&folha->chaves[i]), sizeof(int));
			output.write(reinterpret_cast<char *>(&((Endereco*) folha->ponteiros[i])->value), sizeof(int));
			output.flush();
		}
		folha = folha->prox;
	}
	printf(" - Arquivo de indice criado.\n");
}

void BP::inicia_BP_atravez_de_arquivo(string diretorio){
	BP* bp = new BP(4);
	if(!bp){
		perror("Erro ao criar BP.");
		exit(1);
	}
	ifstream input(diretorio, ios::in | ios::binary);
	int chave, endereco;
	if(!input){
		printf("Arquivo de indice não encontrado!");
		exit(1);
	}
	while (!input.eof()) {
		input.read(reinterpret_cast<char *>(&chave), sizeof(int));
		input.read(reinterpret_cast<char *>(&endereco), sizeof(int));
		ROOT = bp->insere(ROOT, chave, endereco);
	}
}