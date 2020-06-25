// metodo trapezoide adaptado de <https://github.com/kiwenlau/MPI_PI/blob/master/Trapezoid2/mpi_pi.c>
// O programa calcula PI pelo método de trapezóide usando a biblioteca MPI
// Esse metodo tem como base a soma das ́areas dos trapezios que  compoem  a ́area  sob  uma  função. 
// A partir da função f(x)= 4/(1+x^2), PI é a integral definida de f(x) de 0 a 1

#include <stdio.h>
#include <mpi.h>

#define N 1E8
#define d 1E-8
#define d2 1E-16

int main (int argc, char* argv[])
{
    int rank, size, error, i;
    double pi=0.0, result=0.0, sum=0.0, begin=0.0, end=0.0, x2;
    
    error=MPI_Init (&argc, &argv);
    
    //Pega o rank do processo, ou seja, qual parte do processo está sendo processada em determinado nó
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    
    //Pega a quantidade de processos realizados, a quantidade de vezes que o programa é dividido em numero de processos
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    
    // Pega o nome do nó em que o processo está sendo realizado
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    // Printa a mensagem indicando qual o rpocessador, qual o rank e a quantidade de processos em que o programa está rodando
    printf("Processor %s, rank %d out of %d processors\n", processor_name, rank, size);
    
    // Sincroniza todos os processos para que os nós iniciem em conjunto a processar a mesma parte do codigo.
    MPI_Barrier(MPI_COMM_WORLD);
    // Indica a o inicio da contagem de tempo e pega o valor
    begin = MPI_Wtime();
    
    // Cada processo calcula parte do sum, ou seja, um quarto do valor da área do trapezóide
    for (i=rank; i<N; i+=size)
    {
        //calcula a área do trapezoide da função
        //realizando o somatorio para um ponto específico dentro da área definida conforme o valor do laço da for para o trapezio da função
        // Ou seja, é calculado um valor bruto/inteiro que representa o valor de pi ao final do laço for
        x2=d2*i*i;
        result+=4.0/(1.0+x2);
    }
    
    // Combina todos os elementos presentes no buffer de cada processo do grupo usando a operação definida como parâmetro 
    // e coloca o valor resultante no buffer do processo especificado 
    // neste caso o valor da área da função definida, ou seja, um trapezóide com área equivalente ao valor de pi
    MPI_Reduce(&result, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    //Sincroniza todos os processos antes que o restante do programa seja realizado no nó mestre e pega o tempo final
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    
    //Calcula o PI no nó mestre
    if (rank==0)
    {
        // pi é igual a multiplicação do valor d 1*10^-8 para a 
        // precisão correta do valor.
        pi=d*sum;
        printf("np=%2d; Time=%fs; PI=%0.8f\n", size, end-begin, pi);
    }
    
    error=MPI_Finalize();
    
    return 0;
}
