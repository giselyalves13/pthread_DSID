#include <stdio.h>
#include <unistd.h>

main(void) {
    int pid; /* id do processo da task filha*/

    /* Tarefa pai envia uma mensagem com o id do processo para a tela. */
    printf("\nParent task active as process %d.\n", getpid());

    /*  fork() cria uma cópia idêntica da task pai. */
    if((pid = fork()) == 0) {
        /* Este bloco é executado apenas pela subtask. */

        /* Task filha se substitui com a imagem em disco da filha */
        execl("/home/user/bin/Listing3.2b", "Listing3.2b", NULL);
        printf("A successful execl() call will never return!\n");
        exit(-1);
    }

    /* Pai aguarda a task filha terminar */
    waitpid(pid);
    return;
}
