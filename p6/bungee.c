/**
 * @file bungee.c
 * @author Jayani Sivakumar
 * Competitive programming solution for Bungee Builder using segment tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include "segTree.h"

int minComparator(const void *a, const void *b) {
    return *(const int *)b - *(const int *)a;
}

int main() {
    int n;
    if (scanf("%d", &n) != 1) {
        return 1;
    }

    int *a = malloc(n * sizeof(int));
    if (!a) {
        return 1;
    }
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    SegTree *seg = makeST(sizeof(int), minComparator);
    for (int i = 0; i < n; i++) {
        addST(seg, &a[i]);
    }

    int *s = malloc(n * sizeof(int));
    if (!s) {
        return 1;
    }
    int m = 0;
    int ans = 0;

    for (int i = 0; i < n; i++) {
        int isPeak = 0;
        if (n == 1) {
            isPeak = 1;
        } else if (i == 0) {
            isPeak = (a[0] > a[1]);
        } else if (i == n - 1) {
            isPeak = (a[n - 1] > a[n - 2]);
        } else {
            isPeak = (a[i] > a[i - 1] && a[i] > a[i + 1]);
        }
        if (!isPeak) {
            continue;
        }

        while (m > 0 && a[s[m - 1]] <= a[i]) {
            int j = s[m - 1];
            if (j + 1 <= i - 1) {
                int idx = queryST(seg, j + 1, i - 1, NULL);
                int valley = a[idx];
                int diff = a[j] - valley;
                if (diff > ans) {
                    ans = diff;
                }
            }
            m--;
        }
        if (m > 0) {
            int j = s[m - 1];
            if (j + 1 <= i - 1) {
                int idx = queryST(seg, j + 1, i - 1, NULL);
                int valley = a[idx];
                int diff = a[i] - valley;
                if (diff > ans) {
                    ans = diff;
                }
            }
        }
        s[m++] = i;
    }

    printf("%d\n", ans);

    free(s);
    free(a);
    freeST(seg);
    return 0;
}
