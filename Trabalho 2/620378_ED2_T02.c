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

typedef struct {
	int raiz;
} Indice;

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
 void cadastrar(Indice* iprimary, Indice* ibrand);

 int alterar(Indice iprimary);

 void buscar(Indice iprimary,Indice ibrand);

 void listar(Indice iprimary,Indice ibrand);

 /*******************************************************/

 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();

/* (Re)faz o índice */
void criar_indices(Indice *iprimary, Indice *ibrand);

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
void libera_no_ip(node_Btree_ip ip);
void libera_no_is(node_Btree_is is);

// recupera registro no arquivo de dados a partir do rrn
Produto recuperar_registro(int rrn); //feito

// le entrada do usuario e devolve uma struct com os dados
Produto ler_entrada(char *registro); //feito

// gera chave a partir dos dados inseridos
void gerar_chave(Produto *produto); //feito

// busca rrn no indice primario
int procura_rrn(Indice* iprimary, char chave[TAM_PRIMARY_KEY]);

/*
*   Caro aluno,
*   caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*   void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*   void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/

/* Atualiza os dois índices com o novo registro inserido */
void insere_registro_indices(Indice *iprimary, Indice *ibrand, Produto p, int rrn);

/* Exibe o jogo */
int exibir_registro(int rrn);

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
	Indice iprimary;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Produtos */
	Indice ibrand;
	criar_ibrand(&ibrand);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, ibrand);
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
	char node[193] = "", qtd[4] = "", rrn[5] = "", desc[4] = "";
	int pos = 0;
	strncpy(node, ARQUIVO_IP+(tamanho_registro_ip*rrn), tamanho_registro_ip);

	node_Btree_ip *no_ip;
	no_ip = criar_no_ip();
	strncpy(qtd, node+pos, 3);

	no_ip->num_chaves = atoi(qtd);
	pos += 3;

	for(int i = 0; i < no_ip->num_chaves; i++){
		strncpy(no_ip->chaves[i].pk, node+pos, 10);
		strncpy(rrn, node+pos+10, 4);
		no_ip->chaves[i].rrn = atoi(rrn);
		pos += 14;
	}
	pos = 3+(ordem_ip-1)*14;

	no_ip->folha = node[pos];
	pos++;

	for(int i = 0; i < no_ip->num_chaves; i++){
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
	char node[193] = "", qtd[4] = "", string[102] = "", desc[4] = "";
	char aux;
	int pos = 0;
	strncpy(node, ARQUIVO_IS+(tamanho_registro_is*rrn), tamanho_registro_is);

	node_Btree_is *no_is;
	no_is = criar_no_is();
	strncpy(qtd, node+pos, 3);

	no_is->num_chaves = atoi(qtd);
	pos += 3;

	for(int i = 0; i < no_is->num_chaves; i++){
		strncpy(no_is->chaves[i].pk, node+pos, 10);
		no_is->chaves[i].string = strtok(node+pos+10, "#");
		pos += 111;
	}
	pos = 3+(ordem_is-1)*111;

	no_is->folha = node[pos];
	pos++;

	for(int i = 0; i < no_is->num_chaves; i++){
		if(node[pos] != '*'){
			strncpy(desc, node+pos, 3);
			no_is->desc[i] = atoi(desc);
		pos += 3;
	}

	return no_is;
}

void write_btree_ip(node_Btree_ip *salvar, int rrn){

}

void write_btree_is(node_Btree_is *salvar, int rrn){

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

void criar_indices(Indice *iprimary, Indice *ibrand){
	Produto p;

	for(int i = 0; i < nregistros; i++){
		p = recuperar_registro(i);
		insere_registro_indices(iprimary, ibrand, p, rrn);
	}
}

void insere_registro_indices(Indice *iprimary, Indice *ibrand, Produto p, int rrn){
	node_Btree_ip *no_ip;
	node_Btree_is *no_is;
	if(strlen(ARQUIVO_IP) == 0){ // cria nó raiz ip
		iprimary.raiz = 0;
		no_ip = criar_no_ip();
		strcpy(no_ip->chave[0].pk, p.pk);
		no_ip->chave[0].rrn = rrn;
		write_btree_ip(no_ip, 0);
	} else {


	}
	if(strlen(ARQUIVO_IS) == 0){ // cria nó raiz is
		ibrand.raiz = 0;
		no_is = criar_no_is();
		strcpy(no_is->chave[0].pk, p.pk);
		strcat(no_is->chave[0].string, p.nome);
		strcat(no_is->chave[0].string, "$");
		strcat(no_is->chave[0].string, p.marca);
		write_btree_is(no_is, 0);
	} else {

	}
}

void cadastrar(Indice* iprimary, Indice* ibrand){
	Produto p;
	char novo_produto[193] = "";

	p = ler_entrada(novo_produto);

	// se ja existe, não cadastra novamente
	if(procura_rrn(iprimary, p.pk)){
		printf(ERRO_PK_REPETIDA, p.pk);
		return;
	}

	inserir_registro_indices(iprimary, ibrand, p);

	// insere entrada no arquivo de dados
	strcat(ARQUIVO, novo_produto);
	nregistros++;
}

void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto j){

	nregistrosip++;
	nregistrosis++;
}

int procura_rrn(Indice* iprimary, char chave[TAM_PRIMARY_KEY]){

}

int alterar(Indice iprimary){

}

void buscar(Indice iprimary,Indice ibrand){

}

void listar(Indice iprimary,Indice ibrand){

}
