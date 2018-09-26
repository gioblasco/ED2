/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 620378
 * Aluno: Giovanna Blasco Martin
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY	11
#define TAM_NOME 		51
#define TAM_MARCA 		51
#define TAM_DATA 		11
#define TAM_ANO 		3
#define TAM_PRECO 		8
#define TAM_DESCONTO 	4
#define TAM_CATEGORIA 	51


#define TAM_REGISTRO 	192
#define MAX_REGISTROS 	1000
#define MAX_CATEGORIAS 	30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas para o usuario */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!\n"
#define INICIO_BUSCA 		 		"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  			"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"



/* Registro do Produto */
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


/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;

/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;

/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;

/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);

void gerarChave(Produto *produto);

/* (Re)faz o índice respectivo */
void insere_indices(int pos, Produto produto, Ip *iprimary, Is *iproduct, Is *ibrand, Isf *iprice, Ir *icategory);

void insere_iprimary(Ip *indice_primario, int pos, Produto prod);

void insere_isecondary(Is *indice_secundario, int pos, int produto, int marca, Produto prod);

void insere_iprice(Isf *indice_preco, int pos, Produto prod);

void insere_ireverse(Ir *indice_reverso, int pos, Produto prod);

/* Realiza os scanfs na struct Produto */
Produto ler_entrada(char *registro);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);

// Rotina para liberar indice reverso e suas listas ligadas
void libera_listareversa(Ir *indice_reverso, int ncategorias);

// rotina de ordenação
void ordena_indices(int nregistros, int ncategorias, Ip *iprimary, Is *iproduct, Is *ibrand, Isf *iprice, Ir *icategory);

// busca secundarios
int procura_rrn(Ip *iprimary, char pk[TAM_PRIMARY_KEY], int nregistros);

// rotinas de comparação
int compara_primario(const void *a, const void *b);
int compara_secundario(const void *a, const void *b);
int compara_preco(const void *a, const void *b);
int compara_categoria(const void *a, const void *b);

// rotinas de listagem
void listar(Ip *iprimary, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);

// rotina de busca
void buscar(Ip *iprimary, Is* ibrand, Is* iproduct, Ir *icategory, int nregistros, int ncat);

// rotina de remoção
char remover(Ip *iprimary, int nregistros);

// rotina de alteracao
char alterar(Ip *primary, Isf *iprice, int nregistros);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
	int carregarArquivo = 0, nregistros = 0, ncat = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	/*Alocar e criar índices secundários*/
	// indice secundario de produto
	Is *iproduct = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
		if(!iproduct) {
			perror(MEMORIA_INSUFICIENTE);
			exit(1);
		}

	// indice secundario de marca
	Is *ibrand = (Is *) malloc (MAX_REGISTROS * sizeof(Is));
		if(!ibrand) {
			perror(MEMORIA_INSUFICIENTE);
			exit(1);
		}

	// indice secundário de preço
	Isf *iprice = (Isf *) malloc (MAX_REGISTROS * sizeof(Isf));
		if(!iprice) {
			perror(MEMORIA_INSUFICIENTE);
			exit(1);
		}

	// indice reverso
	Ir *icategory = (Ir *) malloc (MAX_REGISTROS * sizeof(Ir));
		if(!icategory) {
			perror(MEMORIA_INSUFICIENTE);
			exit(1);
		}

	Produto produto;
	if(nregistros > 0){
		for(int i = 0; i < nregistros; i++){
			produto = recuperar_registro(i);
			insere_indices(i, produto, iprimary, iproduct, ibrand, iprice, icategory);
		}

		ordena_indices(nregistros, ncat, iprimary, iproduct, ibrand, iprice, icategory);
	}

	/* Execução do programa */
	Produto novo;
	char novo_produto[193] = "";
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao)
		{
			case 1:
				/*cadastro*/
				// le entrada
				novo = ler_entrada(novo_produto);

				// insere entrada nos indices
				if(procura_rrn(iprimary, novo.pk, nregistros) != -1){
					printf(ERRO_PK_REPETIDA, novo.pk);
				}
				else {
					insere_indices(nregistros, novo, iprimary, iproduct, ibrand, iprice, icategory);
					nregistros++;
					ordena_indices(nregistros, ncat, iprimary, iproduct, ibrand, iprice, icategory);

					// insere entrada no arquivo de dados
					strcat(ARQUIVO, novo_produto);
				}
			break;
			case 2:
				/*alterar desconto*/
				printf(INICIO_ALTERACAO);
				if(alterar(iprimary, iprice, nregistros))
					printf(SUCESSO);
				else
					printf(FALHA);
			break;
			case 3:
				/*excluir produto*/
				printf(INICIO_EXCLUSAO);
				if(remover(iprimary, nregistros))
					printf(SUCESSO);
				else
					printf(FALHA);
			break;
			case 4:
				/*busca*/
				printf(INICIO_BUSCA);
				buscar(iprimary, ibrand, iproduct, icategory, nregistros, ncat);
			break;
			case 5:
				/*listagens*/
				printf(INICIO_LISTAGEM);
				listar(iprimary, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 6:
				/*libera espaço*/
			break;
			case 7:
				/*imprime o arquivo de dados*/
				printf(INICIO_ARQUIVO);
				if(!nregistros)
					printf(ARQUIVO_VAZIO);
				else
					printf("%s\n", ARQUIVO);
			break;
			case 8:
				/*imprime os índices secundários*/
				imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
			break;
			case 9:
	      /*Liberar memória e finalizar o programa */
				free(iprimary);
				free(iproduct);
				free(ibrand);
				free(iprice);
				// libera_listareversa(icategory, ncat);
				return 0;
			break;
			default:
				printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}


/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
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
	if(!com_desconto)
	{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}
	else
	{
		sscanf(j.desconto,"%d",&desconto);
		sscanf(j.preco,"%f",&preco);
		preco = preco *  (100-desconto);
		preco = ((int) preco)/ (float) 100 ;
		printf("%.2f\n",  preco);

	}
	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL){
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}

int compara_primario(const void *a, const void *b){
	Ip *ia = (Ip *)a;
  Ip *ib = (Ip *)b;
  return strcmp(ia->pk, ib->pk);
}

int compara_secundario(const void *a, const void *b){
	Is *ia = (Is *)a;
  Is *ib = (Is *)b;
	if (strcmp(ia->string, ib->string) == 0)
		return strcmp(ia->pk, ib->pk);
	return strncmp(ia->string, ib->string, strlen(ia->string));
}

int compara_preco(const void *a, const void *b){
	Isf *ia = (Isf *)a;
  Isf *ib = (Isf *)b;
	if(ia->price < ib->price)
		return -1;
	if(ia->price > ib->price)
		return 1;
	return strcmp(ia->pk, ib->pk);
}

int compara_categoria(const void *a, const void *b){
	Ip *ia = (Ip *)a;
  Ip *ib = (Ip *)b;

}

void listar(Ip *iprimary, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opList = 0, sem_registro = 0;
	int rrn;
	scanf("%d%*c", &opList);
	switch (opList) {
		case 1:
			for(int i = 0; i < nregistros; i++){
				exibir_registro(iprimary[i].rrn, 0);
				if(iprimary[i].rrn != -1){
					sem_registro++;
					printf("\n");
				}
			}
		break;
		case 2:
		break;
		case 3:
			for(int i=0; i < nregistros; i++){
				rrn = procura_rrn(iprimary, ibrand[i].pk, nregistros);
				exibir_registro(rrn, 0);
				if(rrn != -1){
					sem_registro++;
					printf("\n");
				}
			}
		break;
		case 4:
			for(int i=0; i < nregistros; i++){
				rrn = procura_rrn(iprimary, iprice[i].pk, nregistros);
				exibir_registro(rrn, 1);
				if(rrn != -1){
					sem_registro++;
					printf("\n");
				}
			}
		break;
	}
	if(!sem_registro)
		printf(REGISTRO_N_ENCONTRADO);
}

int procura_rrn(Ip *iprimary, char pk[TAM_PRIMARY_KEY], int nregistros){
	Ip *registro;
	registro = bsearch(pk, iprimary, nregistros, sizeof(Ip), compara_primario);
	if(!nregistros || !registro){
		return -1;
	}
	return registro->rrn;
}

void insere_indices(int pos, Produto produto, Ip *iprimary, Is *iproduct, Is *ibrand, Isf *iprice, Ir *icategory){
	insere_iprimary(iprimary, pos, produto);
	insere_isecondary(iproduct, pos, 1, 0, produto);
	insere_isecondary(ibrand, pos, 0, 1, produto);
	insere_iprice(iprice, pos, produto);
	//insere_ireverse(icategory, i, produto);
}

void ordena_indices(int nregistros, int ncategorias, Ip *iprimary, Is *iproduct, Is *ibrand, Isf *iprice, Ir *icategory){
	qsort(iprimary, nregistros, sizeof(Ip), compara_primario);
	qsort(iproduct, nregistros, sizeof(Is), compara_secundario);
	qsort(ibrand, nregistros, sizeof(Is), compara_secundario);
	qsort(iprice, nregistros, sizeof(Isf), compara_preco);
}

int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

// gera a chave a partir dos dados recuperados do registro
void gerarChave(Produto *produto){
	char chave[11] = "";
	strncpy(chave, produto->nome, 2);
	char marca[3] = "";
	strncpy(marca, produto->marca, 2);
	strcat(chave, marca);
	char data[11] = "";
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
	scanf(" %51[^\n]s%*c", novo.nome);
	scanf(" %51[^\n]s%*c", novo.marca);
	scanf(" %[^\n]s%*c", novo.data);
	scanf(" %[^\n]s%*c", novo.ano);
	scanf(" %[^\n]s%*c", novo.preco);
	scanf(" %[^\n]s%*c", novo.desconto);
	scanf(" %51[^\n]s%*c", novo.categoria);
	gerarChave(&novo);

	// grava infos em um registro só
	sprintf(registro, "%s@%s@%s@%s@%s@%s@%s@", novo.nome, novo.marca, novo.data, novo.ano, novo.preco, novo.desconto, novo.categoria);
	int tam = strlen(registro);
	while(tam < 192){
		registro[tam] = '#';
		tam++;
	}
	registro[tam] = '\0';

	return novo;
}

/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Produto j;
	p = strtok(temp,"@");
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
	gerarChave(&j);
	return j;
}

char alterar(Ip *iprimary, Isf *iprice, int nregistros){
	char desconto[4] = "", pk[11] = "";
	int rrn;
	scanf("%s%*c", pk);
	rrn = procura_rrn(iprimary, pk, nregistros);
	if(rrn == -1){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	scanf("%s%*c", desconto);
	while(strlen(desconto) != 3){
		printf(CAMPO_INVALIDO);
		scanf("%s%*c", desconto);
	}

	int indice = rrn*192;
	int lim = indice + 192;
	int conta_arroba = 0;

	// encontra onde esta o desconto no arquivo
	while(indice < lim){
		if(ARQUIVO[indice] == '@')
			conta_arroba++;
		indice++;
		if(conta_arroba == 5)
			break;
	}

	// modifica o arquivo
	lim = indice + 2;
	int pos = 0;
	while(indice <= lim){
		ARQUIVO[indice] = desconto[pos];
		indice++;
		pos++;
	}

	// atualiza indice
	Produto produto;
	for (int i = 0; i < nregistros; i++){
		produto = recuperar_registro(i);
		insere_iprice(iprice, i, produto);
	}

	qsort(iprice, nregistros, sizeof(Isf), compara_preco);

	return 1;
}

char remover(Ip *iprimary, int nregistros){
	char pk[11] = "";
	scanf("%s%*c", pk);
	Ip *registro;
	registro = bsearch(pk, iprimary, nregistros, sizeof(Ip), compara_primario);
	if(!registro){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	ARQUIVO[(registro->rrn)*192] = '*';
	ARQUIVO[((registro->rrn)*192)+1] = '|';
	registro->rrn = -1;

	return 1;
}

void buscar(Ip *iprimary, Is* ibrand, Is* iproduct, Ir *icategory, int nregistros, int ncat){
	int opSearch = 0, rrn;
	scanf("%d%*c", &opSearch);
	switch(opSearch){
		Ip *registro_p;
		Is *registro_s;
		char pk[11] = "";
		char nome[51] = "";
		case 1:
			scanf("%s%*c", pk);
			registro_p = bsearch(pk, iprimary, nregistros, sizeof(Ip), compara_primario);
			if(registro_p != NULL && registro_p->rrn != -1)
				exibir_registro(registro_p->rrn, 0);
			else
				printf(REGISTRO_N_ENCONTRADO);
		break;
		case 2:
			scanf("%[^\n]s%*c", nome);
			registro_s = bsearch(nome, iproduct, nregistros, sizeof(Is), compara_secundario);
			if(registro_s == NULL)
				printf(REGISTRO_N_ENCONTRADO);
			else{
				rrn = procura_rrn(iprimary, registro_s->pk, nregistros);
				if(rrn != -1)
					exibir_registro(rrn, 0);
	 			else
					printf(REGISTRO_N_ENCONTRADO);
			}
		break;
		case 3:
		break;
	}
}

/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d%*c", &opPrint);
	if(!nregistros){
		printf(ARQUIVO_VAZIO);
		return;
	}
	switch (opPrint) {
		case 1:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
			}
		break;
		case 2:
			for(int i = 0; i < nregistros; i++){
				printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
			}
		break;
		case 3:
			for(int i = 0; i < ncat; i++){
				printf("%s", icategory[i].cat);
				aux =  icategory[i].lista;
				while(aux != NULL){
					printf(" %s", aux->pk);
					aux = aux->prox;
				}
				printf("\n");
			}
		break;

		case 4:
		for(int i = 0; i < nregistros; i++){
			printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
		}
		break;
	}
}

void insere_iprimary(Ip *indice_primario, int pos, Produto p){
	strcpy(indice_primario[pos].pk, p.pk);
	indice_primario[pos].rrn = pos;
}

void insere_isecondary(Is *indice_secundario, int pos, int product, int brand, Produto p){
	strcpy(indice_secundario[pos].pk, p.pk);
	if(product)
		strcpy(indice_secundario[pos].string, p.nome);
	else if(brand)
		strcpy(indice_secundario[pos].string, p.marca);
}

void insere_iprice(Isf *indice_preco, int pos, Produto p){
	strcpy(indice_preco[pos].pk, p.pk);
	indice_preco[pos].price = atof(p.preco) - (atof(p.preco)*atof(p.desconto)/100);
}

void insere_ireverse(Ir *indice_reverso, int pos, Produto p){

}