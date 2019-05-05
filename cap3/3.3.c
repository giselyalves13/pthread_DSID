#include <sys/signal.h>
#include <stdio.h>
#include <unistd.h>

int count = 0; /* contador para o número de eventos ^C manipulados */

main(void) {
    void control_C();

    /* inicializa ^C handler */
    signal(SIGINT, control_C);

    while(count < 3) {
        /* conta ao usuário interativo o que fazer */
        printf("\n\tDigite um ^C quando estiver pronto...");
        fflush(stdout);
        pause();
        printf("Eventos lidados #%d.\n", count);
    }
    printf("\nTrês eventos foram manipulados. Tchau!\n\n");
}

void control_C() {
    /* reestabiliza o handler */
    signal(SIGINT, control_C);

    /* conta ^Cs */
    count++;
}
