/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com encadeamento
 *
 * RA: 620378
 * Aluno: Giovanna Blasco Martin
 * ========================================================================== */

/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO  "**********************EXCLUIR*********************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"



/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave {
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
int carregar_arquivo();


/* Exibe o jogo */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);


/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
void criar_tabela(Hashtable *tabela, int tam);
Produto recuperar_registro(int rrn);
void gerar_chave(Produto *produto);
Produto ler_entrada(char *registro);
int buscar_rrn(Hashtable tabela, char *pk);
void imprimir_tabela(Hashtable tabela);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */

int main()
{

	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo)
		carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
		scanf("%d%*c", &opcao);
		switch(opcao) {

		case 1:
			cadastrar(&tabela);
			break;
		case 2:
			printf(INICIO_ALTERACAO);
			if(alterar(tabela))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3:
			printf(INICIO_BUSCA);
			buscar(tabela);
			break;
		case 4:
			printf(INICIO_EXCLUSAO);
			printf("%s", (remover(&tabela)) ? SUCESSO : FALHA );
			break;
		case 5:
			printf(INICIO_LISTAGEM);
			imprimir_tabela(tabela);
			break;
		case 6:
			liberar_tabela(&tabela);
			break;
		case 10:
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}


/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

/* Recebe do usuário uma string simulando o arquivo completo. */
int carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);

	return strlen(ARQUIVO)/TAM_REGISTRO;
}

short hash(const char* chave, int tam){
	short i, pos = 0;
	short aux;

	for(i = 0; i < 8; i++){
		pos = (pos + ((i+1) * f(chave[i]))) % tam;
	}

	return pos;
}

/*Auxiliar para a função de hash*/
short f(char x)
{
	return (x < 59) ? x - 48 : x - 54;
}

int prox_primo(int a){
	int i, primo;
	if((a <= 1) || ((a % 2 == 0) && a > 2))
		a++;
	while(1){
		primo = 1;
		for(i = 3; i <= floor(sqrt(a)); i++){
			if(a%i == 0){
				a++;
				primo = 0;
				break;
			}
		}
		if(a == 2 || primo == 1)
			return a;
		a++;
	}
}

/* Exibe o jogo */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
  char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, j.categoria);
	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");
	return 1;
}

/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
	char temp[TAM_REGISTRO], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[TAM_REGISTRO] = '\0';
	Produto j;
	p = strtok(temp,"@");
	strcpy(j.pk,p);
	p = strtok(NULL,"@");
	strcpy(j.nome,p);
	p = strtok(NULL,"@");
	strcpy(j.marca,p);
	p = strtok(NULL,"@");
	strcpy(j.data,p);
	p = strtok(NULL,"@");
	strcpy(j.ano,p);
	p = strtok(NULL,"@");
	strcpy(j.preco,p);
	p = strtok(NULL,"@");
	strcpy(j.desconto,p);
	p = strtok(NULL,"@");
	strcpy(j.categoria,p);

	return j;
}

// gera a chave a partir dos dados recuperados do registro
void gerar_chave(Produto *produto){
	char chave[11], marca[3], data[11];
	memset(chave, 0, 11);
	strncpy(chave, produto->nome, 2);
	memset(marca, 0, 3);
	strncpy(marca, produto->marca, 2);
	strcat(chave, marca);
	memset(data, 0, 11);
	strcpy(data, produto->data);
	strcat(chave, strtok(data, "/"));
	strcat(chave, strtok(NULL, "/"));
	strcat(chave, produto->ano);
	strcat(chave, "\0");

	// salva a chave gerada na pk do produto
	strcpy(produto->pk, chave);
}

Produto ler_entrada(char *registro){
	Produto novo;
	// leituras
	scanf(" %[^\n]s%*c", novo.nome);
	scanf(" %[^\n]s%*c", novo.marca);
	scanf(" %[^\n]s%*c", novo.data);
	scanf(" %[^\n]s%*c", novo.ano);
	scanf(" %[^\n]s%*c", novo.preco);
	scanf(" %[^\n]s%*c", novo.desconto);
	scanf(" %[^\n]s%*c", novo.categoria);
	gerar_chave(&novo);

	// grava infos em um registro só
	sprintf(registro, "%s@%s@%s@%s@%s@%s@%s@%s@", novo.pk, novo.nome, novo.marca, novo.data, novo.ano, novo.preco, novo.desconto, novo.categoria);
	int tam = strlen(registro);
	while(tam < TAM_REGISTRO){
		registro[tam] = '#';
		tam++;
	}
	registro[tam] = '\0';

	return novo;
}

void criar_tabela(Hashtable *tabela, int tam){
	tabela->tam = tam;
	tabela->v = (Chave **) calloc (sizeof(Chave*), tam);
}

void carregar_tabela(Hashtable* tabela){
	int i;
	short pos;
	Produto p;
	Chave *novo, *aux, *anterior;
	for(i = 0; i < nregistros; i++){
		p = recuperar_registro(i);
		pos = hash(p.pk, tabela->tam);
		novo = (Chave *) malloc (sizeof(Chave));
		strcpy(novo->pk, p.pk);
		novo->rrn = i;
		if(tabela->v[pos] == NULL){
			novo->prox = NULL;
			tabela->v[pos] = novo;
		} else {
			aux = tabela->v[pos];
			if(strcmp(p.pk, aux->pk) < 0){
				novo->prox = aux;
				tabela->v[pos] = novo;
			} else {
				while(aux != NULL && strcmp(aux->pk, p.pk) < 0){
					anterior = aux;
					aux = aux->prox;
				}
				novo->prox = aux;
				anterior->prox = novo;
			}
		}
	}
}

void cadastrar(Hashtable* tabela){
	short pos;
	Produto p;
	Chave *novo, *aux, *anterior;
	char registro[TAM_REGISTRO];
	memset(registro, 0, TAM_REGISTRO);

	p = ler_entrada(registro);
	pos = hash(p.pk, tabela->tam);
	if(pos < 0){
		return;
	}
	novo = (Chave *) malloc (sizeof(Chave));
	strcpy(novo->pk, p.pk);
	novo->rrn = strlen(ARQUIVO)/TAM_REGISTRO;
	if(tabela->v[pos] == NULL){
		novo->prox = NULL;
		tabela->v[pos] = novo;
	} else {
		aux = tabela->v[pos];
		if(strcmp(p.pk, aux->pk) < 0){
			novo->prox = aux;
			tabela->v[pos] = novo;
		} else {
			while(aux != NULL && strcmp(aux->pk, p.pk) < 0){
				anterior = aux;
				aux = aux->prox;
			}
			if(aux != NULL && strcmp(aux->pk, p.pk) == 0){
				printf(ERRO_PK_REPETIDA, p.pk);
				free(novo);
				return;
			}
			novo->prox = aux;
			anterior->prox = novo;
		}
	}

	strcat(ARQUIVO, registro);
	nregistros++;

	printf(REGISTRO_INSERIDO, p.pk);
}

int alterar(Hashtable tabela){
	char desconto[TAM_DESCONTO], pk[TAM_PRIMARY_KEY];
	int rrn, pos;
	char ehdesc;
	Chave *aux;
	memset(pk, 0, TAM_PRIMARY_KEY);
	memset(desconto, 0, TAM_DESCONTO);
	scanf(" %[^\n]s%*c", pk);
	pos = hash(pk, tabela.tam);
	if(pos < 0){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	aux = tabela.v[pos];
	while(aux != NULL && strcmp(aux->pk, pk) < 0){
		aux = aux->prox;
	}
	if(aux == NULL || strcmp(aux->pk, pk) != 0){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	} else
		rrn = aux->rrn;

	scanf(" %[^\n]s%*c", desconto);
	while(1){
		ehdesc = 1;
		if(strlen(desconto) != 3)
			ehdesc = 0;
		if(ehdesc && !isdigit(desconto[0]) && !isdigit(desconto[1]) && !isdigit(desconto[2]))
			ehdesc = 0;
		if(ehdesc && (atoi(desconto) < 0 || atoi(desconto) > 100))
			ehdesc = 0;
		if(!ehdesc){
			printf(CAMPO_INVALIDO);
			memset(desconto, 0, TAM_DESCONTO);
			scanf(" %[^\n]s%*c", desconto);
		} else {
			break;
		}
	}

	int indice = rrn*TAM_REGISTRO;
	int lim = indice + TAM_REGISTRO;
	int conta_arroba = 0;

	// encontra onde esta o desconto no arquivo
	while(indice < lim){
		if(ARQUIVO[indice] == '@')
			conta_arroba++;
		indice++;
		if(conta_arroba == 6)
			break;
	}

	// modifica o arquivo
	lim = indice + 2;
	pos = 0;
	while(indice <= lim){
		ARQUIVO[indice] = desconto[pos];
		indice++;
		pos++;
	}

	return 1;
}

void buscar(Hashtable tabela){
	char pk[TAM_PRIMARY_KEY];
	int pos, rrn;
	Chave *aux;
	memset(pk, 0, TAM_PRIMARY_KEY);
	scanf(" %[^\n]s%*c", pk);

	pos = hash(pk, tabela.tam);
	if(pos < 0){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}
	aux = tabela.v[pos];
	while(aux != NULL && strcmp(aux->pk, pk) < 0){
		aux = aux->prox;
	}
	if(aux == NULL || strcmp(aux->pk, pk) != 0){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	} else
		rrn = aux->rrn;

	exibir_registro(rrn);
}

void imprimir_tabela(Hashtable tabela){
	int i;
	Chave *aux;

	for(i = 0; i < tabela.tam; i++){
		printf("[%d]", i);
		aux = tabela.v[i];
		while(aux != NULL){
			printf(" %s", aux->pk);
			aux = aux->prox;
		}
		printf("\n");
	}
}

int remover(Hashtable* tabela){
	char pk[TAM_PRIMARY_KEY];
	int pos, rrn;
	Chave *aux, *anterior;
	memset(pk, 0, TAM_PRIMARY_KEY);
	scanf(" %[^\n]s%*c", pk);

	pos = hash(pk, tabela->tam);
	if(pos < 0){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	aux = tabela->v[pos];
	if(aux != NULL && strcmp(aux->pk, pk) == 0){
		tabela->v[pos] = aux->prox;
	} else {
		while(aux != NULL && strcmp(aux->pk, pk) < 0){
			anterior = aux;
			aux = aux->prox;
		}
		if(aux == NULL || strcmp(aux->pk, pk) != 0){
			printf(REGISTRO_N_ENCONTRADO);
			return 0;
		} else {
			anterior->prox = aux->prox;
		}
	}

	rrn = aux->rrn;

	ARQUIVO[(rrn)*TAM_REGISTRO] = '*';
	ARQUIVO[((rrn)*TAM_REGISTRO)+1] = '|';

	free(aux);
	return 1;
}

void liberar_tabela(Hashtable* tabela){
	int i;
	Chave *aux;
	for(i = 0; i < tabela->tam; i++){
		while(tabela->v[i] != NULL){
			aux = tabela->v[i];
			tabela->v[i] = tabela->v[i]->prox;
			free(aux);
		}
	}
	free(tabela->v);
}
