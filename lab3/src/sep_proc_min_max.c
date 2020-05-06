#include <stdio.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>

int main( int argc, char **argv) {
          int pid = fork();
          
          if ( pid == 0 ) {
               char* argv1[argc + 1];

               argv1[0] = "parallel_min_max";
               for (int i = 1; i < argc; i++ ) {
                   argv1[i] = argv[i];
               }
               argv1[argc] = NULL;
                  

                     execl("parallel_min_max", "parallel_min_max", "--seed", "5", "--array_size", "5", "--pnum", "2", "-f", NULL);
 
                     return 0;
           }
           
         if (pid != 0) {
             wait(NULL);

             printf("Exec finished\n");
       
         }
   
         return 0;
}
