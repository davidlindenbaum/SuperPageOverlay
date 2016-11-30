#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/wait.h>

#define SYSCALL 511
#define SETSYSCALL 510

void main (int argc, char* argv[])
{
	pid_t pid;
	int status;
	int ret = 0;
	char *process_name;

	if(argc < 2)
	{
		printf("Usage: ./badger-trap <name | pid | command | help> {arguments}\n");
		printf("Example1: ./badger-trap command ls\n");
		printf("Example2: ./badger-trap command ./a.out <arguments>\n");
		printf("Example3: ./badger-trap name blacksholes omp-csr gem5\n");
		printf("Example3: ./badger-trap pid 1124 2346 11\n");
		return;
	}

	if(strcmp(argv[1],"set")==0)
	{
		if(argc < 7)
		{
			printf("Please provide 5 settings:\n");
			printf("4k tlb set_bits (3), 4k tlb entries_per_set (16), 2m tlb set_bits (3), 2m tlb entries_per_set (16), use overlays (1)\n");
			return;
		}
		syscall(SETSYSCALL, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
	}
	else if(strcmp(argv[1],"command")==0)
	{
		if(argc < 3)
		{
			printf("Please provide a command to run\n");
			printf("Example1: ./badger-trap command ls\n");
			printf("Example2: ./badger-trap command ./a.out <arguments>\n");
			return;
		}
		process_name = basename(argv[2]);
		printf("ASDF... %p, %s\n", process_name, process_name);
		ret = syscall(SYSCALL,&process_name,1,1);
		switch ((pid = fork()))
		{
			case -1:
			perror ("Fork Failed !!");
			break;
			case 0:
			execvp(argv[2], &argv[2]);
			exit(0);
			break;
			default:
			printf("Badger Trap launched with process %s\n",argv[2]);
			wait(&status);
			break;
		}
		ret = syscall(SYSCALL,NULL,0,1);
	}
	else if (strcmp(argv[1],"name")==0)
	{
		ret = syscall(SYSCALL,&argv[2],argc-2,1);
	}
	else if (strcmp(argv[1],"pid")==0)
	{
		ret = syscall(SYSCALL,&argv[2],argc-2,-1);
	}
	else if((strcmp(argv[1],"help")==0))
	{
		printf("Usage: ./badger-trap <names | pid | command | help> {arguments}\n");
		printf("Example1: ./badger-trap command ls\n");
		printf("Example2: ./badger-trap command ./a.out <arguments>\n");
		printf("Example3: ./badger-trap names blacksholes omp-csr gem5\n");
		printf("Example3: ./badger-trap pid 1124 2346 11\n");
		return;
	}
	else
	{
		printf("Cannot run command provided. Run ./badger_trap help for more information\n");
		return;
	}
	return;
}
