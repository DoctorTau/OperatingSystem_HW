#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

__pid_t this_pid, sender_pid;
__int8_t received_bit = -1;

/**
 * @brief This function is a signal handler that receives signals from the parent process.
 * 
 * @param sig The integer representation of the signal received by the process.
 */
void resiveBitHandler(int sig) {
    // The SIGUSR1 signal means a received bit equals 0
    if (sig == SIGUSR1) {
        received_bit = 0;
    } 
    // The SIGUSR2 signal means a received bit equals 1
    else if (sig == SIGUSR2) {
        received_bit = 1;
    }
}


void sendConfirmation() {
    kill(sender_pid, SIGUSR1);
}

int main(int argc, char *argv[]) {
    this_pid = getpid();
    printf("PID: %d\n", this_pid);
    printf("Enter PID of sender: ");
    scanf("%d", &sender_pid);

    sendConfirmation();

    printf("Waiting for integer from PID %d\n", sender_pid);

    (void)signal(SIGUSR1, resiveBitHandler);
    (void)signal(SIGUSR2, resiveBitHandler);

    __int8_t resived_bits[32];
    for (int i = 0; i < 32; i++) {
        pause();
        resived_bits[i] = received_bit;
        sendConfirmation();
    }

    __int32_t resived_integer = 0;
    for (int i = 0; i < 32; i++) {
        resived_integer |= resived_bits[i] << i;
    }

    printf("Received integer %d from PID %d\n", resived_integer, sender_pid);

    return 0;
}