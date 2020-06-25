//Codigo adaptado de: https://github.com/kiwenlau/MPI_PI método de montecarlo
//Arquivo fonte : https://raw.githubusercontent.com/kiwenlau/MPI_PI/master/Montecarlo/mpi_pi.c
// O programa calcula PI pelo método de montecarlo.
// Este método calcula pi pela escolha aleatória de numeros que representam pontos em um quadrado
// e somente conta a proporção dos pontos que estiverem dentro de um raio de circulo.
// O valore de PI é calculado por meio da área que corresponde a estes pontos dentro do circulo, a partir de um quadrante, onde a area total do circulo é igual a pi
// assim por meio do calculo de um quadrante do circulo, encontra-se 1/4 de pi, depois multiplica-se esse valor por 4 para encontrar a aproximação de PI.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N 1E8 //valor do loop for para que a soma seja realizada 100 milhões de vezes em valores inteiros
#define d 1E-8 //valor para transformação em ponto flutuante para multiplicação pelo valor inteiro da aproximação de pi

int main (int argc, char* argv[])
{
    int rank, size, error, i, result=0, sum=0;
    double pi=0.0, begin=0.0, end=0.0, x, y;
    
    error=MPI_Init (&argc, &argv);
    
    //Pega o rank do processo, ou seja, qual parte do processo está sendo processada em determinado nó
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    
    //Pega a quantidade de processos realizados, a quantidade de vezes que o programa é dividido em numero de processos
    MPI_Comm_size (MPI_COMM_WORLD, &size);
	
    // Pega o nome do nó em que o processo está sendo realizado
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    //Printa a mensagem indicando qual o rpocessador, qual o rank e a quantidade de processos em que o programa está rodando
    printf("Processor %s, rank %d out of %d processors\n", processor_name, rank, size);
    
    //Sincroniza todos os processos para que os nós iniciem em conjunto a processar a mesma parte do codigo.
    MPI_Barrier(MPI_COMM_WORLD);
    // Indica a o inicio da contagem de tempo
    begin = MPI_Wtime();
    // Inicia a rolagem de numeros aleatórios
    srand((int)time(0));
    
    //Cada processo calcula parte do sum, ou seja, parte da soma dos pontos
    //para cada rank(posição do processo), soma-se o valor total de divisões do processo para que nunca realize o mesmo calculo que outro nó
    for (i=rank; i<N; i+=size)
    {
        x=rand()/(RAND_MAX+1.0);
        y=rand()/(RAND_MAX+1.0);
	// Se o ponto x e y estiver dentro do circulo
        if(x*x+y*y<1.0)
        	result++;
    }
    
    // Combina todos os elementos presentes no buffer de cada processo do grupo usando a operação definida como parâmetro 
    // e coloca o valor resultante no buffer do processo especificado 
    // neste caso 1/4 da área do circulo calculada com valores inteiros
    MPI_Reduce(&result, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    //Sincroniza todos os processos antes que o restante do programa seja realizado no nó mestre
    MPI_Barrier(MPI_COMM_WORLD);
    // Finaliza a contagem de tempo
    end = MPI_Wtime();
    
    //Calcula o PI no nó mestre
    if (rank==0)
    {
	// pi é igual a multiplicação da aproximação do quadrante calculado por 4, multiplicando-se tambem o valor d para a precisão correta do valor.
        pi=4*d*sum;
        printf("np=%2d; Time=%fs; PI=%0.6f\n", size, end-begin, pi);
    }
    
    //finaliza se ocorrer erro
    error=MPI_Finalize();
    
    return 0;
}

