#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


int N, numTh;

int** grid;
int** newgrid;


int** constroiMatriz(){
    int i, j;
    int** matriz = (int**) malloc(N * sizeof(int*));
    
    for(i=0;i<N;i++){
        matriz[i] = (int*) malloc(N * sizeof(int));
        for(j=0;j<N;j++){
            matriz[i][j] = 0;
        }
    }
    return matriz;
}


void tabuleiroInicial(){
    int lin = 1, col = 1;
    //inicializa o Glider
    grid[lin  ][col+1] = 1;
    grid[lin+1][col+2] = 1;
    grid[lin+2][col  ] = 1;
    grid[lin+2][col+1] = 1;
    grid[lin+2][col+2] = 1;
    //-------------------
    lin = 10;
    col = 30;
    //inicializa o R-pentonimo
    grid[lin  ][col+1] = 1;
    grid[lin  ][col+2] = 1;
    grid[lin+1][col  ] = 1;
    grid[lin+1][col+1] = 1;
    grid[lin+2][col+1] = 1;
    //-------------------
}


int getLivingNeighbors(int i, int j){
    int nVizinhos = 0;
    
    // confere vizinho de cima e esquerda
    if(i == 0){
        if (j == 0){
            nVizinhos += grid[N-1][N-1];
        } else {
            nVizinhos += grid[N-1][j-1];
        }
    } else {
        if (j == 0){
            nVizinhos += grid[i-1][N-1];
        } else {
            nVizinhos += grid[i-1][j-1];
        }
    }


    // confere vizinho de cima
    if(i == 0){
        nVizinhos += grid[N-1][j];
    } else {
        nVizinhos += grid[i-1][j];
    }


    // confere vizinho de cima e direita
    if(i == 0){
        if (j == N-1){
            nVizinhos += grid[N-1][0];
        } else {
            nVizinhos += grid[N-1][j+1];
        }
    } else {
        if (j == N-1){
            nVizinhos += grid[i-1][0];
        } else {
            nVizinhos += grid[i-1][j+1];
        }
    }

    // confere vizinho da direita
    if(j == N-1){
        nVizinhos += grid[i][0];
    } else {
        nVizinhos += grid[i][j+1];
    }


    // confere vizinho de baixo e direita
    if(i == N-1){
        if (j == N-1){
            nVizinhos += grid[0][0];
        } else {
            nVizinhos += grid[0][j+1];
        }
    } else {
        if (j == N-1){
            nVizinhos += grid[i+1][0];
        } else {
            nVizinhos += grid[i+1][j+1];
        }
    }


    // confere vizinho de baixo
    if(i == N-1){
        nVizinhos += grid[0][j];
    } else {
        nVizinhos += grid[i+1][j];
    }


    // confere vizinho de baixo e esquerda
    if(i == N-1){
        if (j == 0){
            nVizinhos += grid[0][N-1];
        } else {
            nVizinhos += grid[0][j-1];
        }
    } else {
        if (j == 0){
            nVizinhos += grid[i+1][N-1];
        } else {
            nVizinhos += grid[i+1][j-1];
        }
    }


    // confere vizinho da esquerda
    if(j == 0){
        nVizinhos += grid[i][N-1];
    } else {
        nVizinhos += grid[i][j-1];
    }


    return nVizinhos;
}


 //Regras do jogo
int defineStatus(int neighbors, int currentStatus){

    if(currentStatus == 1){
        if(neighbors == 2 || neighbors == 3){
            return 1;
        }
    } else {
        if(neighbors == 3){
            return 1;
        }
    }

    return 0;
}


void* iterateGenerations(void * arg){
    int i, j, neighbors, thid;
    thid = (int) arg;

    int slice = N/numTh;
    int endSlice = slice*(thid+1)-1;
    int startSlice = slice*thid;

    //criacao da regiao paralela para o for
    for(i=startSlice;i<=endSlice;i++){
        for(j=0;j<N;j++){
            neighbors = getLivingNeighbors(i, j);
            newgrid[i][j] = defineStatus(neighbors, grid[i][j]);
        }
    }
    
    pthread_exit(NULL);
}


void updateBoard(){
    int i, j;

    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            grid[i][j] = newgrid[i][j]; 
        }
    }
}


void getGeneration(){
    int j, join_errorTrack, create_errorTrack;
    void *status;
    pthread_t p[numTh];

    //pthread_create
    for(j=0;j<numTh;j++){
        create_errorTrack = pthread_create(&p[j], NULL, iterateGenerations, (void *) (j));

        if (create_errorTrack) {
            printf("ERROR:create=%d\n", create_errorTrack);
            exit(-1); 
        }
    } 

    for(j=0;j<numTh;j++){
        join_errorTrack = pthread_join(p[j], &status);

        if (join_errorTrack) {
            printf("\nERROR:join code=%d\n", join_errorTrack);
            exit(-1); 
        }
    }

    updateBoard();
}


int getLivingCells(){
    int i, j, sum = 0;

    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            sum += grid[i][j]; 
        }
    }

    return sum;
}


void printTabuleiro(int n){
    
    int i, j;

    printf("Tabuleiro %d\n", n);

    for(i=0;i<50;i++){
        for(j=0;j<50;j++){
            if(grid[i][j] == 1){
                printf("#");    
            }else{
                printf(".");
            }
            
        }
        printf("\n");
    }
    printf("Quantidade de celulas vivas: %d\n", getLivingCells());
    printf("\n\n");
}


int main() {
    printf("N: ");
    scanf("%d", &N);

    grid = constroiMatriz();
    newgrid = constroiMatriz();

    tabuleiroInicial();

    int iterations, i;
    float tmili = 0.000;

    printf("Generations: ");
    scanf("%d", &iterations);

    printf("Número de threads: ");
    scanf("%d", &numTh);

    printTabuleiro(0);

    struct timeval start, end;
   
    gettimeofday(&start, NULL);

    for(i = 1; i<=iterations; i++){
        getGeneration();
        if(i<=5){
            printTabuleiro(i);
        }
    }
    
    gettimeofday(&end, NULL);

    tmili += (float)
    (1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000);

    printTabuleiro(i);
    printf("\nTempo de execucao parte paralela: %.4fms\n", tmili);
    return 0;
}
