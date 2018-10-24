/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 620378
 * Aluno: Giovanna Blasco Martin
 * ========================================================================== */

/* Bibliotecas */
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
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)


#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM				"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"


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


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];

/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(int* iprimary, int* ibrand);

 int alterar(int iprimary);

 void buscar(int iprimary, int ibrand);

 void listar(int iprimary, int ibrand);

 /*******************************************************/

 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();

/* (Re)faz o índice */
void criar_indices(int *iprimary, int *ibrand);

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree_ip(node_Btree_ip *salvar, int rrn);
void write_btree_is(node_Btree_is *salvar, int rrn);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
node_Btree_ip *read_btree_ip(int rrn); //feito
node_Btree_is *read_btree_is(int rrn); //feito

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
node_Btree_ip *criar_no_ip(); //feito
node_Btree_is *criar_no_is(); //feito

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no_ip(node_Btree_ip *ip);
void libera_no_is(node_Btree_is *is);

// recupera registro no arquivo de dados a partir do rrn
Produto recuperar_registro(int rrn); //feito

// le entrada do usuario e devolve uma struct com os dados
Produto ler_entrada(char *registro); //feito

/* Exibe o produto */
int exibir_registro(int rrn); //feito

// gera chave a partir dos dados inseridos
void gerar_chave(Produto *produto); //feito

/* Atualiza os dois índices com o novo registro inserido */
void insere_registro_indices(int *iprimary, int *ibrand, Produto p, int rrn);
int divide_no_ip(int rrn_inicial, Chave_ip *chave, int rrn_filhodireito);
int divide_no_is(int rrn_inicial, Chave_is *chave, int rrn_filhodireito);
int insere_registro_aux_ip(int rrn_inicial, Chave_ip *chave);
int insere_registro_aux_is(int rrn_inicial, Chave_is *chave);
void insere_registro_ip(int *iprimary, Chave_ip *chave);
void insere_registro_is(int *ibrand, Chave_is *chave);

int buscar_ip(int rrn, char chave[TAM_PRIMARY_KEY], char print);
char * buscar_is(int rrn, char marca[TAM_STRING_INDICE], char print);

void listar_ip(int raiz, int nivel);
void listar_is(int raiz);

int main()
{
	char *p; /* # */
  /* Arquivo */
	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip*3 + 4 + (-1 + ordem_ip)*14;
	tamanho_registro_is = ordem_is*3 + 4 + (-1 + ordem_is)* (	TAM_STRING_INDICE +9);

	/* Índice primário */
	int raizip = 0;
	/* Índice secundário de nomes dos Produtos */
	int raizis = 0;
	criar_indices(&raizip, &raizis);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&raizip, &raizis);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(raizip))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(raizip, raizis);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(raizip, raizis);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[TAM_REGISTRO] = '\0';
	Produto j;
	p = strtok(temp, "@");
	strcpy(j.pk, p);
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

/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn < 0)
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

// gera a chave a partir dos dados recuperados do registro
void gerar_chave(Produto *produto){
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

node_Btree_ip *read_btree_ip(int rrn){
	char node[tamanho_registro_ip], qtd[4] = "", rrn_aux[5] = "", desc[4] = "";
	int pos = 0;
	if(ARQUIVO_IP == NULL || strlen(ARQUIVO_IP) <= (tamanho_registro_ip*rrn))
		return NULL;
	memset(node, 0, tamanho_registro_ip);
	strncpy(node, ARQUIVO_IP+(tamanho_registro_ip*rrn), tamanho_registro_ip);

	node_Btree_ip *no_ip;
	no_ip = criar_no_ip();
	strncpy(qtd, node+pos, 3);

	no_ip->num_chaves = atoi(qtd);
	pos += 3;

	for(int i = 0; i < no_ip->num_chaves; i++){
		strncpy(no_ip->chave[i].pk, node+pos, 10);
		strncpy(rrn_aux, node+pos+10, 4);
		no_ip->chave[i].rrn = atoi(rrn_aux);
		pos += 14;
	}
	pos = 3+(ordem_ip-1)*14;

	no_ip->folha = node[pos];
	pos++;

	for(int i = 0; i < ordem_ip; i++){
		if(node[pos] != '*'){
			strncpy(desc, node+pos, 3);
			no_ip->desc[i] = atoi(desc);
		} else
			no_ip->desc[i] = -1;
		pos += 3;
	}

	return no_ip;
}

node_Btree_is *read_btree_is(int rrn){
	char node[tamanho_registro_is], qtd[4] = "", string[102] = "", desc[4] = "";
	char aux;
	int pos = 0;
	if(ARQUIVO_IS == NULL || strlen(ARQUIVO_IS) <= (tamanho_registro_is*rrn))
		return NULL;
	memset(node, 0, tamanho_registro_is);
	strncpy(node, ARQUIVO_IS+(tamanho_registro_is*rrn), tamanho_registro_is);

	node_Btree_is *no_is;
	no_is = criar_no_is();
	strncpy(qtd, node+pos, 3);

	no_is->num_chaves = atoi(qtd);
	pos += 3;

	for(int i = 0; i < no_is->num_chaves; i++){
		strncpy(no_is->chave[i].pk, node+pos, 10);
		strcpy(no_is->chave[i].string, strtok(node+pos+10, "#"));
		pos += 111;
	}
	pos = 3 + (ordem_is-1)*111;

	no_is->folha = node[pos];
	pos++;

	for(int i = 0; i < ordem_is; i++){
		if(node[pos] != '*'){
			strncpy(desc, node+pos, 3);
			no_is->desc[i] = atoi(desc);
		} else
				no_is->desc[i] = -1;
		pos += 3;
	}

	return no_is;
}

void write_btree_ip(node_Btree_ip *salvar, int rrn){
	char registro[tamanho_registro_ip], rrn_chave[5], ehfolha[2] = "", folha[4];
	int max;
	if(salvar == NULL)
		return;

	sprintf(registro, "%03d", salvar->num_chaves);
	for(int i = 0; i < salvar->num_chaves; i++){
		strcat(registro, salvar->chave[i].pk);
		sprintf(rrn_chave, "%04d", salvar->chave[i].rrn);
		strcat(registro, rrn_chave);
	}
	max = (ordem_ip-1-salvar->num_chaves-1);
	for(int i = salvar->num_chaves-1; i <= max; i++)
		strcat(registro, "##############");

  ehfolha[0] = salvar->folha;
	strcat(registro, ehfolha);
	for(int j = 0; j < ordem_ip; j++){
		if(salvar->desc[j] == -1){
			strcat(registro, "***");
		} else {
			sprintf(folha, "%03d", salvar->desc[j]);
			strcat(registro, folha);
		}
	}

	memmove(ARQUIVO_IP+(rrn*tamanho_registro_ip), registro, strlen(registro));
}

void write_btree_is(node_Btree_is *salvar, int rrn){
	char registro[tamanho_registro_is], ehfolha[2] = "", folha[4];
	int max;
	if(salvar == NULL)
		return;

	sprintf(registro, "%03d", salvar->num_chaves);
	for(int i = 0; i < salvar->num_chaves; i++){
		strcat(registro, salvar->chave[i].pk);
		strcat(registro, salvar->chave[i].string);
		max = TAM_STRING_INDICE - strlen(salvar->chave[i].string)-1;
		for(int j = 0; j < max; j++){
			strcat(registro, "#");
		}
	}
	max = (ordem_is-1-salvar->num_chaves-1);
	for(int i = salvar->num_chaves-1; i <= max; i++)
		for(int j = 0; j < 111; j++)
			strcat(registro, "#");

	ehfolha[0] = salvar->folha;
	strcat(registro, ehfolha);
	for(int j = 0; j < ordem_is; j++){
		if(salvar->desc[j] == -1){
			strcat(registro, "***");
		} else {
			sprintf(folha, "%03d", salvar->desc[j]);
			strcat(registro, folha);
		}
	}

	memmove(ARQUIVO_IS+(rrn*tamanho_registro_is), registro, strlen(registro));
}

node_Btree_ip *criar_no_ip(){
	Chave_ip * chaves = (Chave_ip *) malloc (sizeof(Chave_ip) * (ordem_ip-1));
	for(int i = 0; i < ordem_ip-1; i++){
		memset(chaves[i].pk, 0, TAM_PRIMARY_KEY);
		chaves[i].rrn = 0;
	}
	int * descendentes = (int *) malloc (sizeof(int) * ordem_ip);
	for(int i = 0; i < ordem_ip; i++){
		descendentes[i] = -1;
	}
	node_Btree_ip * no_ip = (node_Btree_ip *) malloc (sizeof(node_Btree_ip));
	no_ip->num_chaves = 0;
	no_ip->chave = chaves;
	no_ip->desc = descendentes;
	no_ip->folha = 'F';

	return no_ip;
}

node_Btree_is *criar_no_is(){
	Chave_is * chaves = (Chave_is *) malloc (sizeof(Chave_is) * (ordem_is-1));
	for(int i = 0; i < ordem_is-1; i++){
		memset(chaves[i].pk, 0, TAM_PRIMARY_KEY);
		memset(chaves[i].string, 0, TAM_STRING_INDICE);
	}
	int * descendentes = (int *) malloc (sizeof(int) * ordem_is);
	for(int i = 0; i < ordem_is; i++){
		descendentes[i] = -1;
	}
	node_Btree_is * no_is = (node_Btree_is *) malloc (sizeof(node_Btree_is));
	no_is->num_chaves = 0;
	no_is->chave = chaves;
	no_is->desc = descendentes;
	no_is->folha = 'F';

	return no_is;
}

void criar_indices(int *iprimary, int *ibrand){
	Produto p;

	for(int i = 0; i < nregistros; i++){
		p = recuperar_registro(i);
		insere_registro_indices(iprimary, ibrand, p, i);
	}
}

void cadastrar(int* iprimary, int* ibrand){
	Produto p;
	char novo_produto[193] = "";

	p = ler_entrada(novo_produto);

	// se ja existe, não cadastra novamente
	if(buscar_ip((*iprimary), p.pk, 0) != -1){
		printf(ERRO_PK_REPETIDA, p.pk);
		return;
	}

	insere_registro_indices(iprimary, ibrand, p, nregistros);

	// insere entrada no arquivo de dados
	strcat(ARQUIVO, novo_produto);
	nregistros++;
}

void insere_registro_indices(int *iprimary, int *ibrand, Produto p, int rrn){
	Chave_ip chave_ip;
	Chave_is chave_is;

	strcpy(chave_ip.pk, p.pk);
	chave_ip.rrn = rrn;
	insere_registro_ip(iprimary, &chave_ip);

	strcpy(chave_is.string, p.marca);
	strcat(chave_is.string, "$");
	strcat(chave_is.string, p.nome);
	strcpy(chave_is.pk, p.pk);
	insere_registro_is(ibrand, &chave_is);
}

void insere_registro_ip(int *iprimary, Chave_ip *chave){
	node_Btree_ip *no_ip;
	Chave_ip *chave_promovida;
	int rrn_filhodireito = -1;
	no_ip = read_btree_ip((*iprimary));

	if(no_ip == NULL){
		no_ip = criar_no_ip();
		no_ip->folha = 'F';
		no_ip->num_chaves = 1;
		no_ip->chave[0] = (*chave);
		write_btree_ip(no_ip, nregistrosip);
		nregistrosip++;
	} else {
		chave_promovida = chave;
		rrn_filhodireito = insere_registro_aux_ip((*iprimary), chave_promovida);
		if(rrn_filhodireito != -1){
			no_ip = criar_no_ip();
			no_ip->folha = 'N';
			no_ip->num_chaves = 1;
			no_ip->chave[0] = (*chave_promovida);

			no_ip->desc[0] = (*iprimary);
			no_ip->desc[1] = rrn_filhodireito;

			write_btree_ip(no_ip, nregistrosip);
			(*iprimary) = nregistrosip;
			nregistrosip++;
		}
	}
	libera_no_ip(no_ip);
}

void insere_registro_is(int *ibrand, Chave_is *chave){
	node_Btree_is *no_is;
	Chave_is *chave_promovida;
	int rrn_filhodireito = -1;
	no_is = read_btree_is((*ibrand));

	if(no_is == NULL){
		no_is = criar_no_is();
		no_is->folha = 'F';
		no_is->num_chaves = 1;
		no_is->chave[0] = (*chave);
		write_btree_is(no_is, nregistrosis);
		nregistrosis++;
	} else {
		chave_promovida = chave;
		rrn_filhodireito = insere_registro_aux_is((*ibrand), chave_promovida);
		if(rrn_filhodireito != -1){
			no_is = criar_no_is();
			no_is->folha = 'N';
			no_is->num_chaves = 1;
			no_is->chave[0] = (*chave_promovida);

			no_is->desc[0] = (*ibrand);
			no_is->desc[1] = rrn_filhodireito;

			write_btree_is(no_is, nregistrosis);
			(*ibrand) = nregistrosis;
			nregistrosis++;
		}
	}
	libera_no_is(no_is);
}

int insere_registro_aux_ip(int rrn_inicial, Chave_ip *chave){
	Chave_ip *chave_promovida;
	chave_promovida = chave;
	int i, rrn_filhodireito = -1;
	node_Btree_ip *no_ip;
	no_ip = read_btree_ip(rrn_inicial);

	if(no_ip->folha == 'F'){
		if(no_ip->num_chaves < ordem_ip-1){
			i = no_ip->num_chaves-1;
			while(i >= 0 && strcmp(chave->pk, no_ip->chave[i].pk) < 0){
				no_ip->chave[i+1] = no_ip->chave[i];
				i--;
			}
			no_ip->chave[i+1] = (*chave);
			no_ip->num_chaves += 1;

			write_btree_ip(no_ip, rrn_inicial);
			libera_no_ip(no_ip);
			return -1;
		} else {
			libera_no_ip(no_ip);
			return divide_no_ip(rrn_inicial, chave, -1);
		}
	} else {
		i = no_ip->num_chaves-1;
		while(i >= 0 && strcmp(chave->pk, no_ip->chave[i].pk) < 0){
			i--;
		}
		i++;
		rrn_filhodireito = insere_registro_aux_ip(no_ip->desc[i], chave_promovida);

		if(rrn_filhodireito != -1){
			chave = chave_promovida;
			if(no_ip->num_chaves < ordem_ip-1){
				i = no_ip->num_chaves-1;
				while(i >= 0 && strcmp(chave->pk, no_ip->chave[i].pk) < 0){
					no_ip->chave[i+1] = no_ip->chave[i];
					no_ip->desc[i+2] = no_ip->desc[i+1];
					i--;
				}
				no_ip->chave[i+1] = (*chave);
				no_ip->desc[i+2] = rrn_filhodireito;
				no_ip->num_chaves += 1;

				write_btree_ip(no_ip, rrn_inicial);
				libera_no_ip(no_ip);
				return -1;
			}
			else {
				libera_no_ip(no_ip);
				return divide_no_ip(rrn_inicial, chave, rrn_filhodireito);
			}
		} else {
			return -1;
		}
	}
}

int insere_registro_aux_is(int rrn_inicial, Chave_is *chave){
	Chave_is *chave_promovida;
	chave_promovida = chave;
	int i, rrn_filhodireito = -1;
	node_Btree_is *no_is;
	no_is = read_btree_is(rrn_inicial);

	if(no_is->folha == 'F'){
		if(no_is->num_chaves < ordem_is-1){
			i = no_is->num_chaves-1;
			while(i >= 0 && strcmp(chave->string, no_is->chave[i].string) < 0){
				no_is->chave[i+1] = no_is->chave[i];
				i--;
			}
			no_is->chave[i+1] = (*chave);
			no_is->num_chaves += 1;

			write_btree_is(no_is, rrn_inicial);
			libera_no_is(no_is);
			return -1;
		} else {
			libera_no_is(no_is);
			return divide_no_is(rrn_inicial, chave, -1);
		}
	} else {
		i = no_is->num_chaves-1;
		while(i >= 0 && strcmp(chave->string, no_is->chave[i].string) < 0){
			i--;
		}
		i++;
		rrn_filhodireito = insere_registro_aux_is(no_is->desc[i], chave_promovida);

		if(rrn_filhodireito != -1){
			chave = chave_promovida;
			if(no_is->num_chaves < ordem_is-1){
				i = no_is->num_chaves-1;
				while(i >= 0 && strcmp(chave->string, no_is->chave[i].string) < 0){
					no_is->chave[i+1] = no_is->chave[i];
					no_is->desc[i+2] = no_is->desc[i+1];
					i--;
				}
				no_is->chave[i+1] = (*chave);
				no_is->desc[i+2] = rrn_filhodireito;
				no_is->num_chaves += 1;

				write_btree_is(no_is, rrn_inicial);
				libera_no_is(no_is);
				return -1;
			}
			else{
				libera_no_is(no_is);
				return divide_no_is(rrn_inicial, chave, rrn_filhodireito);
			}
		} else {
			libera_no_is(no_is);
			return -1;
		}
	}
}

// retorna rrn do novo nó
int divide_no_ip(int rrn_inicial, Chave_ip *chave, int rrn_filhodireito){
		node_Btree_ip *no_ip, *novo_no;
		int i, chave_alocada = 0;
		no_ip = read_btree_ip(rrn_inicial);

		i = no_ip->num_chaves-1;
		novo_no = criar_no_ip();
		novo_no->folha = no_ip->folha;
		novo_no->num_chaves = floor((ordem_ip-1)/2);

		for(int j = novo_no->num_chaves-1; j >= 0; j--){
			if(!chave_alocada && strcmp(chave->pk, no_ip->chave[i].pk) > 0){
				novo_no->chave[j] = (*chave);
				novo_no->desc[j+1] = rrn_filhodireito;
				chave_alocada = 1;
			} else {
				novo_no->chave[j] = no_ip->chave[i];
				novo_no->desc[j+1] = no_ip->desc[i+1];
				i--;
			}
		}

		if(!chave_alocada){
			while(i >= 0 && strcmp(chave->pk, no_ip->chave[i].pk) < 0){
				no_ip->chave[i+1] = no_ip->chave[i];
				no_ip->desc[i+2] = no_ip->desc[i+1];
				i--;
			}
			no_ip->chave[i+1] = (*chave);
			no_ip->desc[i+2] = rrn_filhodireito;
		}

		(*chave) = no_ip->chave[(int)floor(ordem_ip/2)];
		novo_no->desc[0] = no_ip->desc[(int)floor(ordem_ip/2)+1];
		no_ip->num_chaves = floor(ordem_ip/2);

		write_btree_ip(no_ip, rrn_inicial);
		write_btree_ip(novo_no, nregistrosip);
		libera_no_ip(no_ip);
		libera_no_ip(novo_no);
		nregistrosip++;
		return nregistrosip-1;
}

// retorna rrn do novo nó
int divide_no_is(int rrn_inicial, Chave_is *chave, int rrn_filhodireito){
		node_Btree_is *no_is, *novo_no;
		int i, chave_alocada = 0;
		no_is = read_btree_is(rrn_inicial);

		i = no_is->num_chaves-1;
		novo_no = criar_no_is();
		novo_no->folha = no_is->folha;
		novo_no->num_chaves = floor((ordem_is-1)/2);

		for(int j = novo_no->num_chaves-1; j >= 0; j--){
			if(!chave_alocada && strcmp(chave->string, no_is->chave[i].string) > 0){
				novo_no->chave[j] = (*chave);
				novo_no->desc[j+1] = rrn_filhodireito;
				chave_alocada = 1;
			} else {
				novo_no->chave[j] = no_is->chave[i];
				novo_no->desc[j+1] = no_is->desc[i+1];
				i--;
			}
		}

		if(!chave_alocada){
			while(i >= 0 && strcmp(chave->string, no_is->chave[i].string) < 0){
				no_is->chave[i+1] = no_is->chave[i];
				no_is->desc[i+2] = no_is->desc[i+1];
				i--;
			}
			no_is->chave[i+1] = (*chave);
			no_is->desc[i+2] = rrn_filhodireito;
		}

		(*chave) = no_is->chave[(int)floor(ordem_ip/2)];
		novo_no->desc[0] = no_is->desc[(int)floor(ordem_ip/2)+1];
		no_is->num_chaves = floor(ordem_ip/2);

		write_btree_is(no_is, rrn_inicial);
		write_btree_is(novo_no, nregistrosis);
		libera_no_is(no_is);
		libera_no_is(novo_no);
		nregistrosis++;
		return nregistrosis-1;
}

int alterar(int iprimary){
	char desconto[4] = "", pk[11] = "";
	int rrn;
	scanf("%s%*c", pk);
	rrn = buscar_ip(iprimary, pk, 0);
	if(rrn < 0){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
	scanf("%s%*c", desconto);
	while(strlen(desconto) != 3){
		printf(CAMPO_INVALIDO);
		scanf("%s%*c", desconto);
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
	int pos = 0;
	while(indice <= lim){
		ARQUIVO[indice] = desconto[pos];
		indice++;
		pos++;
	}

	return 1;
}

void buscar(int iprimary, int ibrand){
	int opSearch = 0;
	char pk[TAM_PRIMARY_KEY] = "";
	char marca[TAM_STRING_INDICE] = "";
	char nome[TAM_NOME] = "";
	int rrn;

	scanf("%d%*c", &opSearch);
	switch(opSearch){
		case 1:
			scanf("%s%*c", pk);
			printf(NOS_PERCORRIDOS_IP, pk);
			rrn = buscar_ip(iprimary, pk, 1);
			printf("\n\n");
			if(rrn == -1){
				printf(REGISTRO_N_ENCONTRADO);
			} else {
				exibir_registro(rrn);
			}
		break;
		case 2:
			scanf(" %[^\n]s%*c", marca);
			scanf(" %[^\n]s%*c", nome);
			strcat(marca, "$");
			strcat(marca, nome);
			printf(NOS_PERCORRIDOS_IS, marca);
			strcpy(pk, buscar_is(ibrand, marca, 1));
			printf("\n\n");
			if(!strcmp(pk, "")){
				printf(REGISTRO_N_ENCONTRADO);
			} else {
				rrn = buscar_ip(iprimary, pk, 0);
				exibir_registro(rrn);
			}
		break;
	}
}

int buscar_ip(int rrn, char chave[TAM_PRIMARY_KEY], char print){
	int aux = 0, rrn_res;

	node_Btree_ip *no_ip;

	no_ip = read_btree_ip(rrn);
	if (no_ip == NULL)
		return -1;

	if(print){
		printf("%s", no_ip->chave[aux].pk);
	}

	while (aux < no_ip->num_chaves && strcmp(chave, no_ip->chave[aux].pk) > 0){
		if(print){
			if(aux != 0)
				printf(", %s", no_ip->chave[aux].pk);
		}
		aux++;
	}

	if (aux < no_ip->num_chaves && strcmp(chave, no_ip->chave[aux].pk) == 0){
		if(print){
			if(aux != 0)
				printf(", %s", no_ip->chave[aux].pk);
		}
		rrn_res = no_ip->chave[aux].rrn;
		libera_no_ip(no_ip);
		return rrn_res;
	}

	if (no_ip->folha == 'F'){
		libera_no_ip(no_ip);
		return -1;
	}else{
		rrn_res = no_ip->desc[aux];
		libera_no_ip(no_ip);
		if(print)
			printf("\n");
		buscar_ip(rrn_res, chave, print);
	}
}

char * buscar_is(int rrn, char marca[TAM_STRING_INDICE], char print){
	int aux = 0, rrn_res;
	char *pk_res;

	node_Btree_is *no_is;
	no_is = read_btree_is(rrn);
	if(no_is == NULL)
		return "";

	if(print)
		printf("%s", no_is->chave[aux].string);

	while (aux < no_is->num_chaves && strcmp(marca, no_is->chave[aux].string) > 0){
		if(print){
			if(aux != 0)
				printf(", %s", no_is->chave[aux].string);
		}
		aux++;
	}

	if (aux < no_is->num_chaves && strcmp(marca, no_is->chave[aux].string) == 0){
		if(print){
			if(aux != 0)
				printf(", %s", no_is->chave[aux].string);
		}
		pk_res = (char *) malloc (sizeof(char) * TAM_PRIMARY_KEY);
		sprintf(pk_res, "%s", no_is->chave[aux].pk);
		libera_no_is(no_is);
		return pk_res;
	}

	if (no_is->folha == 'F'){
		libera_no_is(no_is);
		return "";
	} else {
		rrn_res = no_is->desc[aux];
		libera_no_is(no_is);
		if(print)
			printf("\n");
		buscar_is(rrn_res, marca, print);
	}
}

void listar(int iprimary, int ibrand){
	int opList = 0;
	scanf("%d%*c", &opList);
	switch(opList){
		case 1:
			listar_ip(iprimary, 1);
		break;
		case 2:
			listar_is(ibrand);
		break;
	}
}

void listar_ip(int raiz, int nivel){
	node_Btree_ip *no_ip;
	if(nregistrosip == 0){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}
	no_ip = read_btree_ip(raiz);
	printf("%d - ", nivel);
	for(int i = 0; i < no_ip->num_chaves; i++){
		if(i != 0)
			printf(", ");
		printf("%s", no_ip->chave[i].pk);
	}
	printf("\n");
	if(no_ip->folha == 'N'){
		for(int j = 0; j < ordem_ip; j++)
			if(no_ip->desc[j] != -1)
				listar_ip(no_ip->desc[j], nivel+1);
	}
	libera_no_ip(no_ip);
}

void listar_is(int raiz){
	node_Btree_is *no_is;
	char string[TAM_STRING_INDICE];
	if(nregistrosis == 0){
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}
	no_is = read_btree_is(raiz);
	for(int i = 0; i < ordem_ip; i++){
		if(no_is->desc[i] != -1)
			listar_is(no_is->desc[i]);
		if(i < no_is->num_chaves){
			strcpy(string, strtok(no_is->chave[i].string, "$"));
			printf("%s", string);
			for(int i = strlen(string); i < 50; i++)
				printf("-");
			strcpy(string, no_is->chave[i].string+strlen(string)+1);
			printf(" %s", string);
			for(int i = strlen(string); i < 50; i++)
				printf("-");
			printf("\n");
		}
	}
	libera_no_is(no_is);
}

void libera_no_ip(node_Btree_ip *ip){
	free(ip->chave);
	free(ip->desc);
	free(ip);
}

void libera_no_is(node_Btree_is *is){
	free(is->chave);
	free(is->desc);
	free(is);
}
