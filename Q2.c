#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>


//DECLARA AS VARIAVEIS UTILIZADAS
int N, T, A; //numero de usuarios,quantidade de threads usadas,quantidade de arquivos
int ler[100];  //aux para ler os arquivos
int sel = 0; //aux para uso das threads
double Vtotal = 0;  //valor total a ser exibido no final
pthread_mutex_t arqler = PTHREAD_MUTEX_INITIALIZER;  //mutex
pthread_mutex_t arqop = PTHREAD_MUTEX_INITIALIZER;   //mutex

//ESTRUTURA USADA PARA ARMAZENAR OS DADOS DE CADA USUARIO
typedef struct user{
    char nome[50];   //nome
    char id[50];     //id
    int ultimo;      //ultimo acesso
    double pont;     //pontuacao
}user;


//FUNCAO QUE AJUDA A TRANSFORMAR INTEIRO EM CARACTER
char* transintchar(int x, int y){
	int a = 30;
	static char aux[32] = {0};
	for(; x && a ; --a, x /= y)
		aux[a] = "0123456789abcdef" [x % y];
	return &aux[a+1];
}

void *princ(){   //Funcao principal
    
    
    double auxArq = 0;    //variavel para a quantidade de arquivos
        int a;            //auxiliar para loop
        int espera = -1;  //auxiliar para existencia de arquivos

    while(1){
        
        //LOOP PARA LER OS ARQUIVOS EM ORDEM SEM O USO DE DADOS POR THREADS SIMULTANEAMENTE
        for(a=0; a<A; a++){                      //loop
            pthread_mutex_lock(&arqler);         //trava mutex
            if(ler[a] == 0){                     //selecionando o arquivo
                ler[a] = 1;
                espera = a;
                pthread_mutex_unlock(&arqler);    //destrava mutex
                break;
            }
            pthread_mutex_unlock(&arqler);       //destrava mutex
        }

        //VERIFICANDO SE TODOS OS ARQUIVOS JA FORAM LIDOS 
        if(espera == -1){                          //condicao             
            pthread_mutex_lock(&arqop);            //trava mutex
            Vtotal += auxArq;                     //Adicionando na soma total
            pthread_mutex_unlock(&arqop);         //destrava mutex
            pthread_exit(NULL);         
        }
        espera++;
        FILE *arquivox, *arquivoy;
        

        int c;
        
        char *z;
        char name[100] = "banco";
        char aux[100];     
        z = transintchar(espera, 10);
        strcat(name, z);
        strcpy(aux, name);
        strcat(name, ".txt");
        
        //ABERTURA DOS ARQUIVOS
        arquivox = fopen(name, "r+");
        strcat(aux, "reserva.txt");
        arquivoy = fopen(aux, "w");
        
        
        user lido;             //Armazenando os usuarios na estrutura

        
        //FUNCAO PARA AVALIAR SE USUARIO DEVE SER EXCLUIDO DO ARQUIVO OU NAO
        while(fscanf(arquivox, " %s %s %i %lf", lido.nome, lido.id, &lido.ultimo, &lido.pont) != EOF){     //leitura ate o final
            if(sel == 0){                      //Determina se ja foi lido antes
                auxArq += lido.ultimo/(lido.pont * lido.pont);
            }
            else{
                if((double)lido.ultimo/(lido.pont * lido.pont) > 2*Vtotal){     //Exibe o nome do usuario se nao atender as condicoes para continuar no arquivo sem salvar seu nome
                    printf("%s\n", lido.nome);
                }
                else{
                    fprintf(arquivoy,"%s %s %i %.2lf\n", lido.nome, lido.id, lido.ultimo, lido.pont);    //salva as informacoes do usuario se ele atender as condicoes para continuar
                }
            }
        }
        
        //FECHA OS ARQUIVOS ABERTOS
        fclose(arquivox);
        fclose(arquivoy);
    }
}


/////////////////////////////////////////////////////////////////////////////////////// MAIN /////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]){
    int a, t;                                                      //variavel auxiliar
    int rc;                                                     //Variaveis auxiliares para criar as threads                                                  
    int*taskids[T];                                                 // Variavel de uso pra identificao de Threads
   
    printf("Digite a quantidade de arquivos:");
    scanf("%i", &A);
    printf("Digite a quantidade de threads:");
    scanf("%i", &T);
    printf("Digite a quantidade de usuarios:");
    scanf("%i", &N); 

    
    pthread_t threads[T];                                           //Uso de thread
    
    
    pthread_attr_t atributtes;                                            //Uso de thread
    pthread_attr_init(&atributtes);                                       //Uso de thread
    pthread_attr_setdetachstate(&atributtes, PTHREAD_CREATE_JOINABLE);    //Uso de thread
    
    //PROCESSO DE CRIACAO DAS THREADS 
    for(t=0; t<T; t++){        
    taskids[t] = (double *) malloc(sizeof(double));      //Alocando memoria                 
    *taskids[t] = t;
    printf("No main: criando thread %d\n", t);      
    rc = pthread_create(&threads[t], &atributtes, princ, NULL);  // Crio a Thread, e passo seu ID como parametro pra rotina que ela vai executar.     
     if (rc){         
      printf("ERRO; codigo de retorno %d\n", rc);         
      exit(-1);      
    }
    }   
    
    
    //FINALIZA AS THREADS
    for(a=0; a<T; a++){
        pthread_join(threads[a], NULL);               //Threads sao finalizadas
    }
    
    
    //ZERA VARIAVEIS
    for(a=0; a<T; a++){                               
        ler[a] = 0;                                   //Variavel de leitura zerada
    }
    
    sel = 1;
    
    Vtotal /= N;                                       //Valor final que vai ser exibido no finnal
 
   //RECRIA AS THREADS PARA REFAZER OS ARQUIVOS
   for(t=0; t<T; t++){ 
        
   taskids[t] = (double *) malloc(sizeof(double));      //Alocando memoria                 
   *taskids[t] = t;
   printf("No main: criando thread %d\n", t);      
   rc = pthread_create(&threads[t], &atributtes, princ, NULL);  // Crio a Thread, e passo seu ID como parametro pra rotina que ela vai executar.     
    if (rc){         
      printf("ERRO; codigo de retorno %d\n", rc);         
      exit(-1);      
    }
   }   

    //FINALIZA NOVAMENTE AS THREADS
    for(a=0; a<T; a++){
        pthread_join(threads[a], NULL);                 //Threads sao finalizadas
    }



    //EH PRECISO CRIAR MAIS UMA FUNCAO PARA REFAZER OS ARQUIVOS SEM OS USUARIOS QUE DEVERAO SER EXCLUIDOS//
        
    
    pthread_exit(NULL);

   
}
