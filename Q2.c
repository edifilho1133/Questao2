#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>


/*Declarando variaveis*/
int N, T, A; //numero de usuarios,quantidade de threads usadas,quantidade de arquivos
int ler[100];  //aux para ler os arquivos
int sel = 0; //aux para uso das threads
double Vtotal = 0;  //valor total a ser exibido no final
pthread_mutex_t arqler = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t arqop = PTHREAD_MUTEX_INITIALIZER;


/*Estrutura com os dados de cada usuario*/
typedef struct user{
    char nome[50];
    char id[50];
    int ultimo;
    double pont;
}user;


/*Transformando int em char para operacoes*/
char* transintchar(int x, int y){
	int i = 30;
	static char aux[32] = {0};
	
	for(; x && i ; --i, x /= y)
		aux[i] = "0123456789abcdef" [x % y];
	return &aux[i+1];
}

void *princ(){   /*Funcao principal*/
    
    
    double auxArq = 0;    /*variavel para a quantidade de arquivos*/
        int i;            /*auxiliar para loop*/
        int espera = -1;  /*auxiliar para existencia de arquivos*/

    while(1){
        
        /*Procurando o arquivo que deve ler*/
        for(i=0; i<A; i++){
            pthread_mutex_lock(&arqler);         /*trava mutex*/
            if(ler[i] == 0){                     /*selecionando o arquivo*/
                ler[i] = 1;
                espera = i;
                pthread_mutex_unlock(&arqler);    /*destrava mutex*/
                break;
            }
            pthread_mutex_unlock(&arqler);       /*destrava mutex*/
        }

        /*Verificando se ainda existe algum arquivo*/
        if(espera == -1){                
            pthread_mutex_lock(&arqop);            /*trava mutex*/
            Vtotal += auxArq;                     /*Adicionando na soma total*/
            pthread_mutex_unlock(&arqop);         /*destrava mutex*/
            pthread_exit(NULL);         
        }
        espera++;
        FILE *arquivox, *arquivoy;
        //Trecho de conversão para ler o arquivo certo
        int c;
        char name[100] = "banco";
        char aux[100];
        char *z;
        z = transintchar(espera, 10);
        strcat(name, z);
        strcpy(aux, name);
        strcat(name, ".txt");
        arquivox = fopen(name, "r+");//Abrimos o arquivo de leitura
        strcat(aux, "reserva.txt");
        arquivoy = fopen(aux, "w");//Abrimos o arquivo de backup
        user lido; // Variavel que guarda usuário lido

        while(fscanf(arquivox, " %s %s %i %lf", lido.nome, lido.id, &lido.ultimo, &lido.pont) != EOF){//Enquanto estiver algo para ler, lemos
            if(sel == 0){//Se o seletor for 0, iremos apenas ler os valores e somar à variável local
                auxArq += lido.ultimo/(lido.pont * lido.pont);
            }
            else{//Se for 1, verificamos esse usuário
                if((double)lido.ultimo/(lido.pont * lido.pont) > 2*Vtotal){// Se a conta dele for maior, printamos e não colocamos no arquivo
                    printf("%s\n", lido.nome);
                }
                else{//Escrevemos o usuário no arquivo de backup
                    fprintf(arquivoy,"%s %s %i %.2lf\n", lido.nome, lido.id, lido.ultimo, lido.pont);
                }
            }
        }
        fclose(arquivox);//Fechamos o arquivo
        fclose(arquivoy);
    }
}


/////////////////////////////////////////////////////////////////////////////////////// MAIN /////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]){
    int a;
    

   
    printf("Digite a qauntidade de arquivos:");
    scanf("%i", &A);
    printf("Digite a qauntidade de threads:");
    scanf("%i", &T);
    printf("Digite a qauntidade de usuarios:");
    scanf("%i", &N); 

    
    pthread_t threads[T];                                           /*Uso de thread*/
    pthread_attr_t var;                                            /*Uso de thread*/
    pthread_attr_init(&var);                                       /*Uso de thread*/
    pthread_attr_setdetachstate(&var, PTHREAD_CREATE_JOINABLE);    /*Uso de thread*/

    int*taskids[T];                                                /*Variaveis auxiliares para criar as threads*/
    int rc;                                                        /*Variaveis auxiliares para criar as threads*/



for(a=1; a<T+1; a++){                                    /*Criando as threads*/   
   taskids[a] = (int *) malloc(sizeof(int));                    
   *taskids[a] = a;
   printf("No main: criando thread %d\n", a);            /*Informa criacao da thread*/   
   rc = pthread_create(&threads[a], &var, princ, NULL);  /* Cria a Thread, e passo seu ID como parametro pra rotina que ela vai executar*/     
    if (rc){         
      printf("ERRO; codigo de retorno %d\n", rc);        /*Informa erro na criacao da thread*/      
      exit(-1);      
    }   
  
  }   

    
    for(a=0; a<T; a++){
        pthread_join(threads[a], NULL);               /*Threads sao finalizadas*/
    }
    
    for(a=0; a<T; a++){                               
        ler[a] = 0;                                   /*Variavel de leitura zerada*/
    }
    
    sel = 1;

    Vtotal /= N;   /*Calculo do valor final*/
    
 for(a=1; a<T+1; a++){                                    /*Recriando as threads para remocao*/   
   taskids[a] = (int *) malloc(sizeof(int));                    
   *taskids[a] = a;
   printf("No main: criando thread %d\n", a);            /*Informa criacao da thread*/   
   rc = pthread_create(&threads[a], &var, princ, NULL);  /* Cria a Thread, e passo seu ID como parametro pra rotina que ela vai executar*/     
    if (rc){         
      printf("ERRO; codigo de retorno %d\n", rc);        /*Informa erro na criacao da thread*/      
      exit(-1);      
    }   
  
  } 
    for(a=0; a<T; a++){
        pthread_join(threads[a], NULL);                 /*Threads sao finalizadas*/
    }
    for(a=1; a<=A; a++){                                /*Alterando arquivos*/
        char name[100] = "banco";
        char aux[100];
        char *w;
        
        FILE *arquivox, *arquivoy;                      /*Iniciando arquivos*/
        
        w = transintchar(a, 10);
        strcat(name, w);
        strcpy(aux, name);
        strcat(name, ".txt");
        strcat(aux, "reserva.txt");
        rename(aux, name);
    }
    printf("%.2lf\n", Vtotal);
    pthread_exit(NULL);
   
}
