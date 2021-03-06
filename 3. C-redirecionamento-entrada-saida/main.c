#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main() {

   char ch;
   char buffer_escrita[128];
   unsigned char n_bytes_escritos = 0;
   char buffer[128];
   
   // Abre o arquivo no "Open files table"
   // O retorno é o file descriptor para esse processo que aponta para o arquivo na tabela "Open files table"
   int fd_leitura = open("./texto.txt", O_RDWR | O_CREAT);
   int fd_escrita = open("./escrita_texto.txt", O_WRONLY| O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
   
   // Se o valor retornado for menor que zero algum erro ocorreu
   // Caso queira tratar algum erro específico ver lista em https://linux.die.net/man/3/open
   if ((fd_leitura < 0) || (fd_escrita < 0))
      return EXIT_FAILURE;

   // Quando a aplicação alcançar este ponto, abra um terminal e verifique o fd table do processo
   // $: ps aux
   // Verifique o número do processo do main e procure a pasta /fd dentro da pasta de processo
   // Exemplo: $: la -all /proc/123/fd
   ch = getchar();

   // https://linux.die.net/man/2/dup2
   dup2(fd_escrita, STDOUT_FILENO);

   ch = getchar();

   printf("Escrito a partir de um printf, teste de valor %d\n", 90);
   puts("Escrito por um puts");

   ch = getchar();

   close(fd_leitura);
   close(fd_escrita);

   // Verifique o fd table aqui neste ponto também
   // Exemplo: $: la -all /proc/123/fd
   ch = getchar();

   return EXIT_SUCCESS;
}