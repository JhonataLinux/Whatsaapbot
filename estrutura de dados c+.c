#include <stdio.h>   // entrada/saída (printf, scanf, getchar, etc.)
#include <stdlib.h>  // alocação dinâmica (malloc, free, exit)
#include <string.h>  // funções de string (strncpy, strcmp)

/*
  Estrutura que representa um nó da lista encadeada.
  Cada nó guarda:
    - nome: uma string com espaço para 29 caracteres + '\0' (total 30)
    - prox: ponteiro para o próximo nó da lista (ou NULL se for o último)
*/
typedef struct No{
	char nome[30];        // espaço para o nome (suficiente para 29 caracteres + terminador)
	struct No* prox;      // ponteiro para o próximo nó
}No;

/*
  criar:
  - recebe uma string (texto) e cria um novo nó na memória.
  - retorna o ponteiro para o novo nó.
  - em caso de falha na alocação, imprime erro e encerra o programa.
*/
No* criar(const char *texto){
	/* malloc para alocar o espaço necessário para um No */
	No* novo = (No*)malloc(sizeof(No));
	if(!novo){                      // verifica se malloc retornou NULL (sem memória)
		printf("Erro de memória!\n");
		exit(1);                    // finaliza o programa porque não tem como continuar
	}

	/*
	  Copia a string passada (texto) para novo->nome de forma segura:
	  - strncpy evita overflow porque limita a cópia ao tamanho do destino.
	  - garantimos explicitamente o terminador '\0' no final.
	*/
	strncpy(novo->nome, texto, sizeof(novo->nome));
	novo->nome[sizeof(novo->nome)-1] = '\0';  // garante terminação da string

	novo->prox = NULL;  // novo nó ainda não aponta para nenhum outro
	return novo;        // retorna o ponteiro para o nó criado
}

/*
  inserir:
  - recebe o endereço do ponteiro cabeça (No** cabeca) para permitir alterar
    a cabeça da lista quando a lista estava vazia.
  - adiciona o novo nó ao final da lista.
*/
void inserir(No** cabeca, const char *texto){
	No* novo = criar(texto);     // cria o novo nó com o texto

	if(*cabeca == NULL){
		/* se a lista está vazia, o novo nó vira a cabeça */
		*cabeca = novo;
	}else{
		/* caso contrário, percorremos a lista até encontrar o último nó */
		No* atual = *cabeca;
		while(atual->prox != NULL)   // enquanto existir próximo, avança
			atual = atual->prox;
		/* ao sair do loop, 'atual' aponta para o último nó; ligamos o novo */
		atual->prox = novo;
	}
}

/*
  remover:
  - encontra e remove o nó cujo nome é igual ao 'alvo_rem'.
  - recebe o endereço de 'cabeca' para poder atualizar a cabeça se necessário.
  - retorna 1 se removeu com sucesso, ou 0 se não encontrou.
*/
int remover(No** cabeca, const char *alvo_rem){
	/* 'atual' percorre a lista; 'anterior' guarda o nó anterior a 'atual' */
	No *atual = *cabeca, *anterior = NULL;
	/*
	  percorre enquanto 'atual' não for NULL (fim) e o nome for diferente do alvo.
	  strcmp retorna 0 quando as strings são iguais, então usamos != 0 para continuar.
	*/
	while(atual && strcmp(atual->nome, alvo_rem) != 0){
		anterior = atual;        // salva o anterior antes de avançar
		atual = atual->prox;     // avança para o próximo nó
	}

	/* se chegou ao fim da lista sem encontrar (atual == NULL), retorna 0 (falha) */
	if(!atual) return 0;

	/* se anterior é NULL, estamos removendo a cabeça da lista */
	if(!anterior)
		*cabeca = atual->prox;   // cabeça passa a ser o próximo nó
	else
		anterior->prox = atual->prox; // "pula" o nó removido, ligando anterior -> atual->prox

	free(atual);  // libera a memória do nó removido
	return 1;     // sucesso
}

/*
  listar:
  - percorre a lista imprimindo o conteúdo de cada nó.
  - se a lista estiver vazia (cabeca == NULL) exibe mensagem apropriada.
*/
void listar(No* cabeca){
	if(!cabeca){
		printf("Lista vazia.\n");
		return;
	}
	/* percorre e imprime cada nó no formato [nome]-> */
	for(No* dado = cabeca; dado != NULL; dado = dado->prox){
		printf("[%s]->", dado->nome);
	}
	printf("NULL\n");  // indica o fim da lista
}

/*
  limpar:
  - percorre a lista liberando cada nó com free para evitar vazamento de memória.
  - após chamar limpar, nenhum nó da lista deve permanecer alocado.
*/
void limpar(No* cabeca){
	while(cabeca){
		No* prox = cabeca->prox;  // salva o próximo antes de liberar
		free(cabeca);             // libera o nó atual
		cabeca = prox;            // continua com o próximo
	}
}

/*
  main:
  - menu simples para interagir com a lista:
    1) cadastrar nome
    2) listar nomes
    3) remover nome
    0) sair
*/
int main(void){
	No* lista = NULL;    // cabeça da lista (inicialmente vazia)
	int opcao = -1;      // opção do menu
	char nome[30];       // buffer para ler nomes (29 chars + '\0')

	while(opcao != 0){
		/* mostra o menu */
		puts("--- Lista de Nomes ");
		puts("1) Cadastrar nome ");
		puts("2) Lista nomes ");
		puts("3) Remover nome ");
		puts("0) Sair ");
		printf("Opcao: ");

		/* lê a opção; scanf retorna 1 quando consegue ler um inteiro corretamente */
		if(scanf("%d", &opcao) != 1){
			puts("Entrada inválida.");  // entrada que não é número
			break;                     // sai do loop principal
		}
		getchar(); // consome o '\n' deixado por scanf para não atrapalhar próximas leituras

		switch(opcao){
			case 1:
				/* cadastra um novo participante:
				   - usamos scanf("%29[^\n]", nome) para ler até 29 caracteres
				   - [^\n] lê até encontrar um newline (permite espaços)
				   - garantimos assim que não haverá overflow do buffer 'nome'
				*/
				printf("Nome do participante: ");
				if(scanf("%29[^\n]", nome) != 1){
					puts("Entrada inválida");
					break;
				}
				getchar();            // consome o '\n' que ficou no buffer
				inserir(&lista, nome); // chama a função para inserir no final da lista
				puts("Cadastrado");
				break;

			case 2:
				/* lista todos os nomes */
				listar(lista);
				break;

			case 3:
				/* remove um nome informado pelo usuário */
				printf("Nome a ser removido: ");
				if(scanf("%29[^\n]", nome) != 1){
					puts("Entrada invalida");
					break;
				}
				getchar();  // consome '\n'
				/* chama a função remove (retorna 1 se encontrou e removeu) */
				if(remover(&lista, nome))
					puts("Nome removido!");
				else
					puts("Nome não encontrado!");
				break;

			case 0:
				puts("Encerrado");  // opção para sair do programa
				break;

			default:
				puts("Opcao invalida"); // qualquer valor não previsto
				break;
		}
	}

	/* antes de terminar o programa, liberamos toda a memória da lista */
	limpar(lista);

	return 0;
}