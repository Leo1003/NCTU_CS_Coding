#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 123456

int *ptr;

int main()
{
    int id;

    // Create a shared memory section
    id = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0600);
    if (id < 0) {
        fprintf(stderr, "Failed to create share memory!\n");
        exit(1);
    }

    // Attach the memory section
    // the return value is a pointer to the shared memory section
    ptr = (int *)shmat(id, NULL, 0);
    if (ptr == (void *)-1) {
        fprintf(stderr, "Failed to attach to the share memory!\n");
        exit(1);
    }

    ptr[0] = 0;
    printf("\033[1;32m[server] The value is %d\033[0m\n", ptr[0]);

    while (1) {
        int cmd;

        printf("\n");
        printf("1: Show the value\n");
        printf("2: Modify the value\n");
        printf("3: Exit\n");
        printf("Enter commands: ");
        scanf("%d", &cmd);

        if (cmd == 1) {
            printf("\033[1;32m[server] The value is %d\033[0m\n", ptr[0]);
        } else if (cmd == 2) {
            printf("Input new value: ");
            scanf("%d", &ptr[0]);
        } else {
            break;
        }
    }

    shmdt(ptr);
    shmctl(id, IPC_RMID, NULL);
    return 0;
}
