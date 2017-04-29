#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define LEN (1024*1024*2l)

int rand_int(int max) {
  return (((double) rand()) / RAND_MAX) * max;
}

int rand_distance_rolln(unsigned long addr, int total, int n) {
  int min;
  int i;
  int tmp;
  for (i = 0; i < n; i++) {
    tmp = rand_int(total);
    if (i == 0 || tmp < min) min = tmp;
  }
  return addr + min * (rand() > RAND_MAX/2 ? 1 : -1);
}

long rand_distance_pow(unsigned long addr, long total, int n) {
  double d = ((double)rand()) / RAND_MAX;
  return addr + pow(d, n) * total * (rand() > RAND_MAX/2 ? 1 : -1);
}

long rand_distance_walk(unsigned long addr, long total, float n) {
  int dir = rand() > RAND_MAX/2 ? 1 : -1;
  while(rand() > RAND_MAX * n) {
    addr += LEN * dir;
  }
  while(rand() > RAND_MAX * n) {
    addr += 4096 * dir;
  }
  while(rand() > RAND_MAX * n) {
    addr += dir;
  }
  return addr;
}

void main (int argc, char* argv[]) {
  time_t t;
  int i, j;

  long pages = 10;
  float write_prob = .1;
  float locality = .5;
  char **addrs;

  if (argc > 1) {
    pages = atoi(argv[1]);
    write_prob = atof(argv[2]);
    locality = atof(argv[3]);
  }
  addrs = calloc(pages, sizeof(char*));

  //syscall(502,0,0,0);

  for (i = 0; i < pages; i++) {
    posix_memalign((void**)&addrs[i], LEN, LEN);
    addrs[i][0] = 1;
    printf("%p, %s\n", addrs[i], strerror(errno));
  }
  srand((unsigned) time(&t));
  syscall(502,0,0,0);

  int sum = 0;
  unsigned long addr = 0;
  for (i = 0; i < 50000000; i++) {
    addr = rand_distance_walk(addr, pages * LEN, locality);
    addr = addr % (pages * LEN);
    if (addr < 0) addr += pages * LEN;
    if (rand() > (write_prob * RAND_MAX)) {
      sum += addrs[addr/LEN][addr%LEN];
    } else {
      addrs[addr/LEN][addr%LEN] = (char) rand();
    }
    if (i % 1000000 == 0) printf("%d\n", i);
  }
}
