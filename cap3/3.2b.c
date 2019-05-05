#include <stdio.h>
#include <unistd.h>

main(void) {
    /* Task filha envia mensagem com id do processo para a tela */
    printf("Child task active as process %d.\n", getpid());

    /* Task filha sai normalmente */
    exit(0);
}
