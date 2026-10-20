#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

void print_error_and_usage(char* prog, char* errmsg)
{
    fprintf(stderr, "Error: %s\n", errmsg);
    printf("Usage: %s WRITEFILE WRITESTR\n", prog);
    printf("Create a file named WRITEFILE with content WRITESTR\n");
}

int write_to_fn(const char* fn, const char* data)
{
    int rv = 0;
    FILE* f = fopen(fn, "w");
    if (f == NULL) {
        const char* errmsg = strerror(errno);
        syslog(LOG_ERR, "Failed to open '%s' for writing: %s", fn, errmsg);
        return 1;
    }
    syslog(LOG_DEBUG, "Writing '%s' to '%s'", data, fn);
    if (fprintf(f, "%s", data) < 0) {
        const char* errmsg = strerror(errno);
        syslog(LOG_ERR, "Failed to write to '%s': %s", fn, strerror(errno));
        rv = 1;
    }
    if (fclose(f)) {
        const char* errmsg = strerror(errno);
        syslog(LOG_ERR, "Failed to close '%s': %s", fn, strerror(errno));
        rv = 1;
    }
    return rv;
}

int main(int argc, char* argv[])
{
    int rc = 0;
    if (argc < 3) {
        print_error_and_usage(argv[0], "Not enough arguments");
        return 1;
    }
    else if (argc > 3) {
        print_error_and_usage(argv[0], "Too many arguments");
        return 1;
    }
    // XXX: LOG_PERROR is not POSIX, but available on most UNIX versions
    // according to the man
    openlog(argv[0], LOG_PERROR, LOG_USER);
    return write_to_fn(argv[1], argv[2]);
}
