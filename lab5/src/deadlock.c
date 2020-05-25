#include <pthread.h>
#include <stdio.h>

pthread_mutex_t first_variable_mutex;
pthread_mutex_t second_variable_mutex;
static int first_variable = 1;
static int second_variable = 1;

void *deadlock_first(void *args) {
  pthread_mutex_lock(&first_variable_mutex);
  first_variable++;
  second_variable++;
}


int main() {

  pthread_t threads[2];
  pthread_mutex_init(&first_variable_mutex, NULL);
  pthread_mutex_init(&second_variable_mutex, NULL);
  pthread_create(&threads[0], NULL, deadlock_first, NULL);
  pthread_create(&threads[1], NULL, deadlock_first, NULL);
  for (int i = 1; i< 2; i++) {
     pthread_join(threads[i], NULL);
  }
  printf("First - %i\nSecond - %i\n", first_variable, second_variable);
  return 0;
 }
