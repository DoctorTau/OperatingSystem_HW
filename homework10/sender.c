#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

__pid_t this_pid, resiever_pid;

/// @brief Sends bit to resiever. If bit is 0, sends SIGUSR1, if bit is 1, sends SIGUSR2
/// @param bit Bit to send.
void sendBit(__int8_t bit) {
    if (bit == 0) {
        kill(resiever_pid, SIGUSR1);
    } else {
        kill(resiever_pid, SIGUSR2);
    }
}

void confirmationHandler(int sig) {
}

int main(int argc, char *argv[]) {
    this_pid = getpid();
    int sending_integer;
    printf("PID: %d\n", this_pid);
    printf("Enter PID of resiever: ");
    scanf("%d", &resiever_pid);

    (void)signal(SIGUSR1, confirmationHandler);

    printf("Enter integer to send: ");
    scanf("%d", &sending_integer);

    // Wait for confirmation from resiever
    pause();

    printf("Sending integer %d to PID %d\n", sending_integer, resiever_pid);

    for (int i = 0; i < 32; i++) {
        sendBit(sending_integer & 1);
        sending_integer >>= 1;

        // Wait for confirmation
        pause();
    }

    printf("Done sending integer.\n");

    return 0;
}