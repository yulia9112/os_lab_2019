#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.c"
#include "utils.c"
#include "find_min_max.h"
#include "utils.h"

volatile pid_t* child_array;
volatile int child_num;

static void sigalarm(int signal)  {
   for (int i = 0; i<child_num; i++) {
     kill(child_array[i], SIGKILL);
   } 
  }


int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;
  int timeout = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            break;
          case 1:
            array_size = atoi(optarg);
            break;
          case 2:
            pnum = atoi(optarg);
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  int active_array_step = pnum < array_size ? (array_size / pnum) : 1;

  if (timeout != -1) {
    printf("timed out\n");
    alarm(TIMEOUT);
    signal(SIGALRM, sigalarm);
  }


  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  int pipefd[2];
  if (with_files) {
    FILE *shared_file;
    shared_file = fopen("res.txt", "w+");
    fprintf(shared_file, " ");
    fclose(shared_file);
  } else {
    if (pipe(pipefd) == -1) {
      perror("\nERROR CREATE PIPE!\n");
      exit(EXIT_FAILURE);
     }
    } 

  child_array = (pid_t*)malloc(pnum * sizeof(pid_t));
  child_num = 0;

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        unsigned int start = active_array_step * (active_child_processes - 1);
        unsigned int end = start + active_array_step;
        start = start > array_size ? array_size : start;
        end = end > array_size ? array_size : end;
        if (active_child_processes == pnum)
           end = array_size;
        struct MinMax min_max = GetMinMax(array,start,end);
        if (with_files) {
          FILE *shared_file;
          shared_file = fopen("res.txt", "a+");
          fwrite(&min_max, sizeof(struct MinMax), 1, shared_file);
          fclose(shared_file);
        } else {
          close(pipefd[0]);
          write(pipefd[1], &min_max, sizeof(struct MinMax));
          close(pipefd[1]);
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) { 
    int status = -1;
    waitpid(-1, &status, WNOHANG);
    active_child_processes -= 1;
  }
  
  FILE *shared_file;
  shared_file = fopen("res.txt", "r");

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;
    struct MinMax tmp_min_max;

    if (with_files) {
      fread(&tmp_min_max, sizeof(struct MinMax), 1, shared_file);
      if (i == pnum - 1)
         fclose(shared_file);
    } else {
      close(pipefd[1]);
      read(pipefd[0], &tmp_min_max, sizeof(struct MinMax));
      close(pipefd[0]);
    }
     
    min = tmp_min_max.min;
    max = tmp_min_max.max;

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("\nMin: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
