#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
 int p2c[2];
 int c2p[2];
 pipe(p2c);
 pipe(c2p);

 int cpid = fork();
 if (cpid == 0){ //child
   close(p2c[1]);
   close(c2p[0]);
   
   int ppid;
   int n = read(p2c[0], &ppid, sizeof(ppid));
   if (n != sizeof(ppid)) { 
     exit(1);  // read ping failed!
   }
   printf("%d: received ping from pid %d\n", getpid(), ppid);

   char pong = 'y';
   write(c2p[1], &pong, sizeof(pong));
   close(c2p[1]);

   exit(0);

 } else{ //parent
   close(p2c[0]);
   close(c2p[1]);
   
   int ppid = getpid();
   write(p2c[1], &ppid, sizeof(ppid));
   close(p2c[1]);

   char pong;
   int n = read(c2p[0], &pong, sizeof(pong));
   if (n != sizeof(pong)) {
     exit(1); //read pong failed!
   }
   printf("%d: received pong from pid %d\n", getpid(), cpid);

   wait(0);
   exit(0);
 }

}
