#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main() {

   char buffer_escrita[128];
   unsigned char n_bytes = 0;
   unsigned char n_bytes_escritos = 0;
   char buffer[128];
   unsigned char size_b = 0;
   
   // Abre o arquivo no "Open files table"
   // O retorno é o file descriptor para esse processo que aponta para o arquivo na tabela "Open files table"
   int fd_arquivo_exemplo = open("./texto.txt", O_RDWR | O_CREAT);
   int fd_arquivo_exemplo2 = open("./escrita_texto.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
   
   // Se o valor retornado for menor que zero algum erro ocorreu
   // Caso queira tratar algum erro específico ver lista em https://linux.die.net/man/3/open
   if (fd_arquivo_exemplo > 0) {
      printf("fd arquivo exemplo: %d\n", fd_arquivo_exemplo);
      read(fd_arquivo_exemplo, buffer, 128);
      printf("Esse é o conteudo do arquivo:\n");
      printf("%s\n", buffer);
   }

   if (fd_arquivo_exemplo2 > 0) {
      printf("fd arquivo exemplo: %d\n", fd_arquivo_exemplo2);
      strcpy(buffer_escrita, "Escrita no documento via código\n");
      n_bytes = strlen(buffer_escrita);
      n_bytes_escritos = write(fd_arquivo_exemplo2, buffer_escrita, n_bytes);
      printf("Número de bytes escritos %d\n", n_bytes_escritos);
   }

   close(fd_arquivo_exemplo);
   close(fd_arquivo_exemplo2);

   return 0;
}
