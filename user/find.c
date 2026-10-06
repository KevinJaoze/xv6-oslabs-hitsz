#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"

char* basename(char* path) { 
  char* p;
  //p scan backward and point at the last '/'
  for (p = path+strlen(path); p>=path && *p!='/'; p--);
  
  return p+1;
}

void find(char* path, char* name) {
  int fd;
  struct stat st; //store file status

  if ((fd = open(path, 0)) < 0) { // no such path!
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (strcmp(basename(path), name) == 0) {
    printf("%s\n", path);
  }

  switch (st.type) {
    case T_FILE: //path is a file, leaf node, no recursion
      break;
    
    case T_DIR: //directory is the internal node of recursion
      struct dirent de; // directory entry store
      char buf[512]; // store new subpath 
      char* p;

      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("find: path too long\n");
        break;
      }

      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/'; //write '/' and move forward

      while (read(fd, &de, sizeof(de)) == sizeof(de)) { // iterate entry of current dir to de
       // fd maintain current offset, read can iterate whole dir
        if (de.inum == 0) // invalid entry!
          continue;

        if (strcmp(de.name, ".") == 0 || // strcmp expects String, not char
            strcmp(de.name, "..") == 0)
          continue;

        
        memmove(p, de.name, DIRSIZ); // concatenate path and name
        p[DIRSIZ] = '\0'; //add '\0' at end of subpath

        find(buf, name); //recursion
      }

      break;
  }

  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(2, "usage: find <path> <name>\n");
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}
