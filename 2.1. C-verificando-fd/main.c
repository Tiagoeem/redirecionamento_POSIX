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
   int fd_escrita = open("./escrita_texto.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
   
   // Se o valor retornado for menor que zero algum erro ocorreu
   // Caso queira tratar algum erro específico ver lista em https://linux.die.net/man/3/open
   if ((fd_leitura < 0) || (fd_escrita < 0))
      return EXIT_FAILURE;
   

   printf("fd arquivo leitura: %d\n", fd_leitura);
   // Leitura no arquivo apontado pelo file descriptor
   read(fd_leitura, buffer, 128);
   printf("Esse é o conteudo do arquivo:\n");
   printf("%s\n", buffer);

   printf("fd arquivo escrita: %d\n", fd_escrita);
   strcpy(buffer_escrita, "Escrita no documento via código\n");
   // Escrita no arquivo apontado pelo file descriptor
   n_bytes_escritos = write(fd_arquivo_exemplo2, buffer_escrita, strlen(buffer_escrita));
   printf("Número de bytes escritos %d\n", n_bytes_escritos);

   // Quando a aplicação alcançar este ponto, abra um terminal e verifique o fd table do processo
   // $: ps aux
   // Verifique o número do processo do main e procure a pasta /fd dentro da pasta de processo
   // Exemplo: $: la -all /proc/123/fd
   ch = getchar();

   close(fd_leitura);
   close(fd_escrita);

   // Verifique o fd table aqui neste ponto também
   // Exemplo: $: la -all /proc/123/fd
   ch = getchar();

   return EXIT_SUCCESS;
}
