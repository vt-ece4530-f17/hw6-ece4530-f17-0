/*
http://neocontra.blogspot.com/2013/05/user-mode-performance-counters-for.html
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>

static int fddev = -1;
__attribute__((constructor)) static void init(void) {
	static struct perf_event_attr attr;
	attr.type = PERF_TYPE_HARDWARE;
	attr.config = PERF_COUNT_HW_CPU_CYCLES;
	fddev = syscall(__NR_perf_event_open, &attr, 0, -1, -1, 0);
}

__attribute__((destructor)) static void fini(void) {
	close(fddev);
}

static inline long long cpucycles(void) {
	long long result = 0;
	if (read(fddev, &result, sizeof(result)) < sizeof(result)) return 0;
	return result;
}

int main() {
	unsigned i, k;
	volatile unsigned j;
    long long time_start = 0;
    long long time_end   = 0;
	
    printf("time overhead = ");
	for (k=0; k<10; k++) {
      time_start = cpucycles();
      time_end   = cpucycles();
      printf("%llu ", time_end - time_start);
	}
    printf("\ntime delta = ");
	for (k=0; k<10; k++) {
      time_start = cpucycles();
	  j=0;
	  for (i=0; i<100000; i++)
		j += i;
      time_end   = cpucycles();	  
      printf("%llu ", time_end - time_start);
    }
    printf("\n");
	
	return 0;
}
