#include <stdio.h>
#include <unistd.h>

main(void) {
    int pid; /* id do processo da task filha*/

    /* Tarefa pai envia uma mensagem com o id do processo para a tela. */
    printf("\nParent task active as process %d.\n", getpid());

    /*  fork() cria uma cópia idêntica da task pai. */
    if((pid = fork()) == 0) {
        /* Este bloco é executado apenas pela subtask. */

        /* Task filha envia uma mensagem com o id do processo para a tela. */
        printf("Child task active as process %d.\n", getpid());

        /* Task filha sai normalmente*/
        exit(0);
    }

    /* Pai aguarda a task filha terminar */
    waitpid(pid);
    return;
}
