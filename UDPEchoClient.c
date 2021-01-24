#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <pthread.h>
#define column 100
#define string 107
#define len 21


struct DATA{
	int matrix [column];
	int vector [column];
  int port;
  char servIP[len];
};
typedef struct DATA Data;

int read_file (Data *data) //Ф-ия чтения матрицы из файла
{
  FILE *file;
  file = fopen("My_file.txt", "r"); //Открываю файл на чтение
  int stroki, stolby;
  char propusk [10];
  fscanf(file, "%s", &propusk); // Считываю строку ServIp, тем самы пропуская ее и не ловлю ошибку 
  fscanf(file, "%s", &data[0].servIP);  // Считываю адрес
  fscanf(file, "%s", &propusk); // Считываю строку Port, тем самы пропуская ее и не ловлю ошибку 
  fscanf(file, "%d", &data[0].port);  // Считываю порт
  fscanf(file, "%s", &propusk); // Считываю строку column, тем самы пропуская ее и не ловлю ошибку 
  fscanf(file, "%d", &stolby);  // Считываю кол-во столбцов в матрице
  fscanf(file, "%s", &propusk); // Считываю строку String, тем самы пропуская ее и не ловлю ошибку 
  fscanf(file, "%d", &stroki);  // Считываю кол-во строк в матрице
  fscanf(file, "%s", &propusk); // Считываю строку Vector, тем самы пропуская ее и не ловлю ошибку 
  
  for (int j = 0; j < stolby; j++)
  fscanf(file, "%d ", &data[0].vector[j]); // Считываю значения Vector 
  
  fscanf(file, "%s", &propusk); // Считываю строку Matrix, тем самы пропуская ее и не ловлю ошибку 
  
  for (int i = 0; i < stroki; i++)
    for (int j = 0; j < stolby; j++)
    {
    fscanf(file, "%d ", &data[i].matrix[j]);  // Считываю значения Matrix, и сразу заношу в структуру
    data[i].vector[j] = data[0].vector[j];  // Дополняю структуру вектором
    memcpy(data[i].servIP, data[0].servIP, sizeof(data[0].servIP));
    data[i].port = data[0].port;
    }
  return stroki;
}


void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}


void *create_client (void* arg)
{   
   
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
    Data* a = (Data*)arg;

       /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(a->servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(a->port);     /* Server port */
    
     
    /* Send the string to the server */
    if (sendto(sock, a,sizeof(*a), 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != sizeof(*a))
        DieWithError("sendto() sent a different number of bytes than expected");

    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if ((respStringLen = recvfrom(sock, a, sizeof(*a), 0, 
         (struct sockaddr *) &fromAddr, &fromSize)) < 0)
        DieWithError("recvfrom() failed");
   
    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }
   
   
    
    close(sock);
    
}


int main(int argc, char *argv[]) 
{
  int Kol_potok, result;
  pthread_t threads[string];
  Data data [string];
  Kol_potok = read_file(data);
  pthread_mutex_t mutex;
    for (int i = 0; i < Kol_potok; i++)
    {
    pthread_mutex_lock(&mutex);
    result = pthread_create(&threads[i], NULL, create_client, &data[i]);
        if (result != 0) 
        {
			    perror("Creating the first thread");
			    return EXIT_FAILURE;
        }   
    result = pthread_join(threads[i], NULL);
		if (result != 0) {
			perror("Joining the first thread");
			return EXIT_FAILURE;  
		} 
    printf("[%d] = %d\n", i, data[i].vector[0]);  
    pthread_mutex_unlock(&mutex);
    } 
  
}
