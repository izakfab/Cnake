#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Initial content on screen\n");
    printf("About to clear...\n");

    system("clear");  /* Clear screen on Linux/macOS */

    printf("Console cleared successfully!\n");
    printf("This is the new content.\n");
    return 0;
}
