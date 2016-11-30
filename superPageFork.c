#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MAP_HUGE_2MB (21 << MAP_HUGE_SHIFT)
//#define LEN 1024*4
#define LEN 1024*1024*2

#define HUGEPAGE 1
#define WRITE_PARENT 1
#define WRITE_CHILD 1
#define FIRST_ONLY 0


int main() {
  char *addr;
  int num;
  int res;
  volatile int counter = 0;
  for (num = 0; num < 1; num++) {
    //if (HUGEPAGE) addr = mmap(0, LEN, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB|MAP_HUGE_2MB, -1, 0);
    //else addr = mmap(0, LEN, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    posix_memalign(&addr, LEN, LEN);

    printf("%p, %s\n", addr, strerror(errno));

    volatile int i = 0, j = 0, ii = 0;
    if (WRITE_PARENT) {
      for (i = 0; i < LEN; i += 1024*4) {
        //addr[i/(FIRST_ONLY ? 1024 : 1)] = (char)((i/1024) % 256);
        addr[i] = (char)((i/1024)%256);
        j += addr[i/1024];
      }
    }

      mprotect(addr + 1024*8, 4096, PROT_READ);
      mprotect(addr + 1024*8, 4096, PROT_READ|PROT_WRITE);
    if (fork() == 0) { //child
      //for (ii = 0; ii < 100; ii++) {
      if (WRITE_CHILD) {
        for (i = 0; i < LEN; i += 1024*4) {
          addr[i/(FIRST_ONLY ? 512 : 1)] = (char)((i/1024) % 256);
          j += addr[i/1024];
        }
      } else {
        for (i = 0; i < LEN; i += 1024*4) {
          j += addr[i/1024];
        }
      }
      //}
      //res = munmap(addr, LEN);
      //printf("%d %s %d\n", res, strerror(errno), j);
      return 0;
    } else {
      for (i = 0; i < LEN; i += 1024*4) {
        j += addr[i];
      }
      int status = 0;
      wait(&status);
      //res = munmap(addr, LEN);
      //printf("%d ASDF %s\n", res, strerror(errno));
      //free(addr);
    }
  }
  return 0;
}
