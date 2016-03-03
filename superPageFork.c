#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MAP_HUGE_2MB (21 << MAP_HUGE_SHIFT)
#define LEN 1024*1024*2

#define HUGEPAGE 0
#define WRITE_PARENT 1
#define WRITE_CHILD 0
#define FIRST_ONLY 0

int main() {
    char *addr;
    if (HUGEPAGE) addr = mmap(0, LEN, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB|MAP_HUGE_2MB, -1, 0);
    else addr = mmap(0, LEN, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

    //printf("%p, %s\n", addr, strerror(errno));

    int i = 0;
    if (WRITE_PARENT) {
        for (i = 0; i < LEN; i += 1024) {
            addr[i/(FIRST_ONLY ? 1024 : 1)] = (char)(i % 256);
        }
    }

    if (fork() == 0) { //child
        if (WRITE_CHILD) {
            for (i = 0; i < LEN; i += 1024) {
                addr[i/(FIRST_ONLY ? 1024 : 1)] = (char)(i % 256);
            }
        }
    } else {
        int status = 0;
        wait(&status);
    }
    return 0;
}
