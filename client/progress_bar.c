#include <stdio.h>

void print_progress_bar(int completed, int total) {
    const int barWidth = 50;
    int pos = (completed * barWidth) / total;

    printf("\r[");
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%% (%d/%d)", (completed * 100) / total, completed, total);
    fflush(stdout);
}
