#include "cli.h"

int main(int argc, char** argv) {
    int err = cli_dispatch_command(argc, argv);
    if (err != 0) {
        return err;
    }

    return 0;
}
