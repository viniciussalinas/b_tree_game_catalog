/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 	11
#define TAM_NOME 			51
#define TAM_DESENVOLVEDORA 	51
#define TAM_DATA 			11
#define TAM_CLASSIFICACAO 	3
#define TAM_PRECO 			8
#define TAM_DESCONTO 		4
#define TAM_CATEGORIA 		51
#define TAM_STRING_INDICE 	(TAM_DESENVOLVEDORA + TAM_NOME)


#define TAM_REGISTRO 		192
#define MAX_REGISTROS 		1000
#define MAX_ORDEM 			150
#define TAM_ARQUIVO 		(MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 			 "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE	 "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 	 "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 			 "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 		 "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 			 "Arquivo vazio!"
#define INICIO_BUSCA			 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM 		 "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO		 "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO			 "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO	 "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 		 "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 		 "Busca por %s.\nNos percorridos:\n"


/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char desenvolvedora[TAM_DESENVOLVEDORA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char classificacao[TAM_CLASSIFICACAO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Jogo;


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */

char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO];
char ARQUIVO_IP[2000*sizeof(Chave_ip) + 40];
char ARQUIVO_IS[2000*sizeof(Chave_is) + 40];
/*Ordem das árvores*/
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/*Variavel auxiliar de descendente*/
int desc_aux = 0;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* idev);

 int alterar(Indice iprimary);

 void buscar(Indice iprimary,Indice idev);

 void listar(Indice iprimary,Indice idev);

 /*******************************************************/

 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de jogos  */
void criar_idev(Indice *idev);

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *idev, Jogo j);

/* Exibe o jogo */
int exibir_registro(int rrn);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na struct Jogo */
Jogo recuperar_registro(int rrn);

/* Funcao auxiliar de inserção que retorna uma possivel chave promovida*/
Chave_ip *inserir_registro_aux_ip(Indice *iprimary, Chave_ip *chave);

/* Funcao que divide nó quando o número de chaves é maior que m-1 - Funcao inspirada no pseudocogido do pdf 'arvore B' disponibilizada no ava para auxilio do projeto*/
Chave_ip *divide_no_ip(node_Btree_ip *no, Chave_ip *chave, node_Btree_ip *filho_direito);

int main()
{

	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = 3 + ((ordem_ip - 1) * 14) + 1 + (ordem_ip * 3);
	tamanho_registro_is = 3 + ((ordem_is - 1) * 111) + 1 + (ordem_is * 3);

	/* Índice primário */
	Indice iprimary;
	iprimary.raiz = -1;
	if(nregistros>0)
		criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Jogos */
	Indice idev;
	idev.raiz = -1;
	if(nregistros>0)
		criar_idev(&idev);

	/* Execução do programa */
	int opcao = 0;
	char *p;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo jogo */
			cadastrar(&iprimary, &idev);
			break;
		case 2: /* Alterar o desconto de um Jogo */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um jogo */
			printf(INICIO_BUSCA);
			buscar(iprimary, idev);
			break;
		case 4: /* Listar todos os Jogos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, idev);
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
					fwrite(p , 1 ,tamanho_registro_ip,stdout);
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
					fwrite(p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* Exibe mensagem de erro */
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
int carregar_arquivo(){
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Exibe o jogo */
int exibir_registro(int rrn){
	if(rrn<0)
		return 0;

	float preco;
	int desconto;

	Jogo j = recuperar_registro(rrn);

  	char *cat, categorias[TAM_CATEGORIA];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.desenvolvedora);
	printf("%s\n", j.data);
	printf("%s\n", j.classificacao);

	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);

	strncpy(categorias, j.categoria, strlen(j.categoria));

  	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na struct Jogo */
Jogo recuperar_registro(int rrn){

	Jogo *recuperado = (Jogo *) malloc (sizeof(Jogo));
	char *AUX_ARQUIVO = (char *) malloc (sizeof(char) * TAM_REGISTRO);
	char aux2[TAM_ARQUIVO];

	strcpy(aux2, ARQUIVO);

	AUX_ARQUIVO = strtok(aux2+(rrn*192), "@#");
	strcpy(recuperado->pk, AUX_ARQUIVO);
	AUX_ARQUIVO = strtok(NULL, "@#");
	strcpy(recuperado->nome, AUX_ARQUIVO);
	AUX_ARQUIVO = strtok(NULL, "@#");
	strcpy(recuperado->desenvolvedora, AUX_ARQUIVO);
	AUX_ARQUIVO = strtok(NULL, "@#");
	strcpy(recuperado->data, AUX_ARQUIVO);
	AUX_ARQUIVO = strtok(NULL, "@#");
	strcpy(recuperado->classificacao, AUX_ARQUIVO);
	AUX_ARQUIVO = strtok(NULL, "@#");
	strcpy(recuperado->preco, AUX_ARQUIVO);
	AUX_ARQUIVO = strtok(NULL, "@#");
	strcpy(recuperado->desconto, AUX_ARQUIVO);
	AUX_ARQUIVO = strtok(NULL, "@#");
	strcpy(recuperado->categoria, AUX_ARQUIVO);

	return *recuperado;
}

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary){

}

/* (Re)faz o índice de jogos  */
void criar_idev(Indice *idev){

}

void cadastrar(Indice* iprimary, Indice* idev){

	Jogo *novo = malloc(sizeof(Jogo));;

	/* Le dados do titulo */
				scanf("%[^\n]s", novo->nome);
	getchar();	scanf("%[^\n]s", novo->desenvolvedora);
	getchar();	scanf("%[^\n]s", novo->data);
	getchar();	scanf("%[^\n]s", novo->classificacao);
	getchar();	scanf("%[^\n]s", novo->preco);
	getchar();	scanf("%[^\n]s", novo->desconto);
	getchar();	scanf("%[^\n]s", novo->categoria);


	/* Gera a chave primaria */	
	novo->pk[0] = novo->nome[0];
	novo->pk[1] = novo->nome[1];
	novo->pk[2] = novo->desenvolvedora[0];
	novo->pk[3] = novo->desenvolvedora[1];
	novo->pk[4] = novo->data[0];
	novo->pk[5] = novo->data[1];
	novo->pk[6] = novo->data[3];
	novo->pk[7] = novo->data[4];
	novo->pk[8] = novo->classificacao[0];
	novo->pk[9] = novo->classificacao[1];
	novo->pk[10] = '\0';

//	if(comparapk(novo, iprimary))
//		printf(ERRO_PK_REPETIDA, novo->pk);

//	else{
	
		/* Escreve novo registro no arquivo de dados */
		sprintf(ARQUIVO+(nregistros*192), "%s@%s@%s@%s@%s@%s@%s@%s@", novo->pk, novo->nome, novo->desenvolvedora, novo->data, novo->classificacao, novo->preco, novo->desconto, novo->categoria);
		
		/* Calcula quantos # serao necessarios para completar 192 bytes */
		int a, j;
		a = strlen(novo->nome) + strlen(novo->desenvolvedora) + strlen(novo->categoria);
		j = 154 - a;

		int i;
		for(i = 0; i<j; i++)	
			strncat(ARQUIVO, "#", 1);
	
		sprintf(ARQUIVO+((nregistros+1)*192), '\0');

		inserir_registro_indices(iprimary, idev, *novo);
	
		//write_btree(novo, nregistros, 'p');
		//write_btree(novo, nregistros, 's');



		nregistros++;
//	}

	free(novo);
}

int alterar(Indice iprimary){

}

void buscar(Indice iprimary,Indice idev){

}

void listar(Indice iprimary,Indice idev){

}

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip){
	
	if(ip == 'p'){



	}

	if(ip == 's'){
	
	}
}

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip){

}

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip){
	
	if(ip == 'p'){

		node_Btree_ip *No = malloc (sizeof(node_Btree_ip));

		No->num_chaves = 0;
		No->chave[0].rrn = -1;
		No->chave[0].pk[0] = '\0';
		No->desc[0] = '\0';
		No->folha = '\0';

		//return No;
	}
}

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip){

}

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *idev, Jogo j){

	Chave_ip *chave = malloc(sizeof(Chave_ip));
	chave->rrn = nregistros;
	strcpy(chave->pk, j.pk);

	if(iprimary->raiz==(-1)){

		node_Btree_ip *no = malloc (sizeof(node_Btree_ip));
		//no = criar_no('p'); 
		
		no->folha = 'F';
		no->num_chaves = 1;
		no->chave = chave;
		
		sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip), "%03d", no->num_chaves);
		sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) + 3, "%s%04d", no->chave->pk, no->chave->rrn);
		int i, j;
		for(i=1;i<(ordem_ip-1);i++)
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+14, "##############");
		sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14, "%c", no->folha);
		for(j = 0; j<(ordem_ip);j++)
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14+1+3*j, "***");
		
		//sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip), "%03d%s%04d##############%c*********", no->num_chaves, no->chave->pk, no->chave->rrn, no->folha);

		iprimary->raiz = nregistrosip;

		nregistrosip++;

		free(no);
	}

	else{

		Chave_ip *promovida = malloc(sizeof(Chave_ip));
		promovida = NULL;
		promovida = inserir_registro_aux_ip(iprimary, chave);

		if(promovida != NULL){

			node_Btree_ip *no = malloc (sizeof(node_Btree_ip));
			//no = criar_no('p'); 
			
			no->folha = 'N';
			no->num_chaves = 1;
			no->chave = promovida;

			no->desc[0] = iprimary->raiz;
			no->desc[1] = desc_aux;

			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip), "%03d", no->num_chaves);
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) + 3, "%s%04d", no->chave->pk, no->chave->rrn);
			int i, j;
			for(i=1;i<(ordem_ip-1);i++)
				sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+14, "##############");
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14, "%c", no->folha);
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14+1+3*0, "%03d", no->desc[0]);
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14+1+3*1, "%03d", no->desc[1]);
			for(j = 2; j<(ordem_ip);j++)
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14+1+3*j, "***");

			//sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip), "001%s%04d##############N%03d%03d***", promovida->pk, nregistros, iprimary->raiz, desc_aux);

			iprimary->raiz = nregistrosip;
			nregistrosip++;
			
			free(no);
		}
		free(promovida);
	}
	free(chave);
}

/* Funcao auxiliar de inserção que retorna uma possivel chave promovida*/
Chave_ip *inserir_registro_aux_ip(Indice *iprimary, Chave_ip *chave){
	
	node_Btree_ip *no = malloc (sizeof(node_Btree_ip));
	char auxiliar[tamanho_registro_ip];

	// no = iprimary->raiz;
	sscanf(ARQUIVO_IP + iprimary->raiz*tamanho_registro_ip, "%c%c%c", &auxiliar[0], &auxiliar[1], &auxiliar[2]);
	auxiliar[3] = '\0';
	sscanf(auxiliar, "%d", &no->num_chaves);
	int i;
	for(i=0; i < no->num_chaves; i++){
		sscanf(ARQUIVO_IP + iprimary->raiz*tamanho_registro_ip +3+14*i, "%c%c%c%c%c%c%c%c%c%c", &auxiliar[0], &auxiliar[1], &auxiliar[2], &auxiliar[3], &auxiliar[4], &auxiliar[5], &auxiliar[6], &auxiliar[7], &auxiliar[8], &auxiliar[9]);
		auxiliar[10] = '\0';
		sscanf(auxiliar, "%s", &no->chave->pk);
		sscanf(ARQUIVO_IP + iprimary->raiz*tamanho_registro_ip +3+10*(i+1), "%c%c%c%c", &auxiliar[0], &auxiliar[1], &auxiliar[2], &auxiliar[3]);
		auxiliar[4] = '\0';
		sscanf(auxiliar, "%d", &no->chave->rrn);
	}
	sscanf(ARQUIVO_IP + iprimary->raiz*tamanho_registro_ip +3+(14*ordem_ip-1), "%c", &auxiliar[0]);
	auxiliar[1] = '\0';
	sscanf(auxiliar, "%c", &no->folha);	
	for(i=0;i<ordem_ip;i++){
		sscanf(ARQUIVO_IP + iprimary->raiz*tamanho_registro_ip +3+(14*ordem_ip-1)+1+3*i, "%c%c%c", &auxiliar[0], &auxiliar[1], &auxiliar[2]);
		auxiliar[3] = '\0';
		if(auxiliar[0] != '*')
			sscanf(auxiliar, "%d", &no->desc[i]);
		else no->desc[i] = -1;
	}


	Indice *p;
	Chave_ip *promovida = NULL;

	if(no->folha == 'F'){

		if(no->num_chaves < ordem_ip-1){
			for(i = no->num_chaves-1; i>=0; i--){
				if(strcmp(chave->pk, no->chave[i-1].pk) < 1)
					break;
				strcpy(no->chave[i].pk, no->chave[i-1].pk);
				no->chave[i].rrn = no->chave[i-1].rrn;
			}
			strcpy(no->chave[i].pk, chave->pk);
			no->chave[i].rrn = chave->rrn;
			no->num_chaves++;
		
			desc_aux = -1;
			return NULL;
		}

		else{
			return divide_no_ip(no, chave, NULL);
		}
	}
	else{
		for(i = no->num_chaves-1; i>=0; i--){
			if(strcmp(chave->pk, no->chave[i].pk) < 1)
				break;
		}
		//i++;
		p->raiz = no->desc[i];
		return promovida = inserir_registro_aux_ip(p, chave);

		if(promovida != NULL){
			
			//chave = promovida;
			strcpy(chave->pk, promovida->pk);
			chave->rrn = promovida->rrn;
			
			if(no->num_chaves < ordem_ip-1){
				for(i = no->num_chaves-1; i>=0; i--){
					if(strcmp(chave->pk, no->chave[i].pk) < 1)
						break;
					strcpy(no->chave[i].pk, no->chave[i-1].pk);
					no->chave[i].rrn = no->chave[i-1].rrn;	
					no->desc[i+1] = no->desc[i];
				}
				strcpy(no->chave[i].pk, chave->pk);
				no->chave[i].rrn = chave->rrn;
				no->desc[i+1] = desc_aux;
				no->num_chaves++;

				desc_aux = -1;
				return NULL;
			}
			else{
				return divide_no_ip(no, chave, NULL);
			}
		}
		else{
			desc_aux = -1;
			return NULL;
		}
	}
	free(no);
}

/* Funcao que divide nó quando o número de chaves é maior que m-1 - Funcao inspirada no pseudocogido do pdf 'arvore B' disponibilizada no ava para auxilio do projeto*/
Chave_ip *divide_no_ip(node_Btree_ip *no, Chave_ip *chave, node_Btree_ip *filho_direito){

	int j, i = no->num_chaves, flag_aloc = 0;
	Chave_ip *promovida = malloc(sizeof(Chave_ip));

	node_Btree_ip *novo = malloc(sizeof(node_Btree_ip));
	//novo = criar_no('p');
	novo->desc = no->desc;
	novo->num_chaves = (int) (ordem_ip-1)/2;

	for(j = novo->num_chaves-1; j>=0; j--){
		if(!flag_aloc && strcmp(chave->pk, no->chave[i].pk) < 1){
			strcpy(novo->chave[j].pk, chave->pk);
			novo->chave[j].rrn = chave->rrn;
			novo->desc[i+j] = desc_aux;
			flag_aloc = 1;
		}
		else{
			strcpy(novo->chave[j].pk, no->chave[i].pk);
			novo->chave[j].rrn = no->chave[i].rrn;
			novo->desc[j+1] = no->desc[i+1];
			i--;
		}
	}

	if(!flag_aloc){
		for(;i>=0;i--){
			if(strcmp(chave->pk, no->chave[i].pk) < 1)
				break;
			strcpy(no->chave[i].pk, no->chave[i-1].pk);
			no->chave[i].rrn = no->chave[i-1].rrn;	
			no->desc[i+1] = no->desc[i];
		}
		strcpy(no->chave[i].pk, chave->pk);
		no->chave[i].rrn = chave->rrn;
		no->desc[i+1] = desc_aux;
	}
	i = (int) ordem_ip/2;
	i++;
	strcpy(promovida->pk, no->chave[i].pk);
	promovida->rrn = no->chave[i].rrn;	
	novo->desc[1] = no->desc[i+1];
	no->num_chaves = i-1;

	sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip), "%03d", novo->num_chaves);
	for(i=0;i<(ordem_ip-1);i++){
		if(!(novo->chave[i].rrn < nregistros && novo->chave[i].rrn > 0))
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+14, "##############");
		else
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) + 3, "%s%04d", novo->chave[i].pk, novo->chave[i].rrn);
	}
	sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14, "%c", novo->folha);
	for(j = 0; j<(ordem_ip);j++){
		if(novo->desc[j]!=-1)
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14+1+3*j, "%03d", novo->desc[j]);
		else
			sprintf(ARQUIVO_IP + (nregistrosip*tamanho_registro_ip) +3+i*14+1+3*j, "***");
	}

	desc_aux = nregistrosip;
	nregistrosip++;

	return promovida;
}

/* Structs, bibliotecas e DEFINE's necessarios para a funcao qsort - funcao padrao da biblioteca stdlib */
#include <alloca.h>
#include <limits.h>
typedef struct
  {
    char *lo;
    char *hi;
} stack_node;

#define SWAP(a, b, size)                                                      \
  do                                                                          \
    {                                                                         \
      size_t __size = (size);                                                 \
      char *__a = (a), *__b = (b);                                            \
      do                                                                      \
        {                                                                     \
          char __tmp = *__a;                                                  \
          *__a++ = *__b;                                                      \
          *__b++ = __tmp;                                                     \
        } while (--__size > 0);                                               \
    } while (0)

#define MAX_THRESH 4

#define 	STACK_SIZE  	      (CHAR_BIT * sizeof(size_t))
#define 	PUSH(low, high)       ((void) ((top->lo = (low)), (top->hi = (high)), ++top))
#define     POP(low, high)        ((void) (--top, (low = top->lo), (high = top->hi)))
#define     STACK_NOT_EMPTY       (stack < top)

/* Funcao quicksort extraida da biblioteca padrao stdlib */
void qsort (void *const pbase, size_t total_elems, size_t size, __compar_fn_t cmp){

  char *base_ptr = (char *) pbase;
  const size_t max_thresh = MAX_THRESH * size;
  if (total_elems == 0)
    return;
  if (total_elems > MAX_THRESH)
    {
      char *lo = base_ptr;
      char *hi = &lo[size * (total_elems - 1)];
      stack_node stack[STACK_SIZE];
      stack_node *top = stack;
      PUSH (NULL, NULL);
      while (STACK_NOT_EMPTY)
        {
          char *left_ptr;
          char *right_ptr;
          
          char *mid = lo + size * ((hi - lo) / size >> 1);
          if ((*cmp) ((void *) mid, (void *) lo) < 0)
            SWAP (mid, lo, size);
          if ((*cmp) ((void *) hi, (void *) mid) < 0)
            SWAP (mid, hi, size);
          else
            goto jump_over;
          if ((*cmp) ((void *) mid, (void *) lo) < 0)
            SWAP (mid, lo, size);
        jump_over:;
          left_ptr  = lo + size;
          right_ptr = hi - size;

          do
            {
              while ((*cmp) ((void *) left_ptr, (void *) mid) < 0)
                left_ptr += size;
              while ((*cmp) ((void *) mid, (void *) right_ptr) < 0)
                right_ptr -= size;
              if (left_ptr < right_ptr)
                {
                  SWAP (left_ptr, right_ptr, size);
                  if (mid == left_ptr)
                    mid = right_ptr;
                  else if (mid == right_ptr)
                    mid = left_ptr;
                  left_ptr += size;
                  right_ptr -= size;
                }
              else if (left_ptr == right_ptr)
                {
                  left_ptr += size;
                  right_ptr -= size;
                  break;
                }
            }
          while (left_ptr <= right_ptr);
          if ((size_t) (right_ptr - lo) <= max_thresh)
            {
              if ((size_t) (hi - left_ptr) <= max_thresh)
                POP (lo, hi);
              else
                lo = left_ptr;
            }
          else if ((size_t) (hi - left_ptr) <= max_thresh)
            hi = right_ptr;
          else if ((right_ptr - lo) > (hi - left_ptr))
            {
              PUSH (lo, right_ptr);
              lo = left_ptr;
            }
          else
            {
              PUSH (left_ptr, hi);
              hi = right_ptr;
            }
        }
    }

#define min(x, y) ((x) < (y) ? (x) : (y))
  {
    char *const end_ptr = &base_ptr[size * (total_elems - 1)];
    char *tmp_ptr = base_ptr;
    char *thresh = min(end_ptr, base_ptr + max_thresh);
    char *run_ptr;

    for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size)
      if ((*cmp) ((void *) run_ptr, (void *) tmp_ptr) < 0)
        tmp_ptr = run_ptr;
    if (tmp_ptr != base_ptr)
      SWAP (tmp_ptr, base_ptr, size);
    run_ptr = base_ptr + size;
    while ((run_ptr += size) <= end_ptr)
      {
        tmp_ptr = run_ptr - size;
        while ((*cmp) ((void *) run_ptr, (void *) tmp_ptr) < 0)
          tmp_ptr -= size;
        tmp_ptr += size;
        if (tmp_ptr != run_ptr)
          {
            char *trav;
            trav = run_ptr + size;
            while (--trav >= run_ptr)
              {
                char c = *trav;
                char *hi, *lo;
                for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo)
                  *hi = *lo;
                *hi = c;
              }
          }
      }
  }
}
