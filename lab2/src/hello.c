#include <unistd.h>
#include <stdio.h>

int main() {
    char *args[] = {"./a.out", "-l", NULL};  // argv style: program name + args + NULL terminator

    printf("About to exec ls -l\n");

    // execvp searches PATH for "ls" and replaces current process
    execvp(args[0], args);

    // only reached if execvp fails
    perror("execvp failed");
    return 1;
}
