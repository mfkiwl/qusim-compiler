#include<cstdlib>
#include<ctime>
#include<cstdio>
float x[4][4], y[4][4], z[4][4];
float getrand() {
    return (rand() / (float)RAND_MAX) * 2 - 1;
}
int times = 5;
//using namespace std;
int main() {
    srand(time(NULL));
    rand(); rand();
    freopen("testdata.txt", "w", stdout);

    for (int w = 0; w < times; w++) {

        for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            x[i][j] = getrand();
            y[i][j] = getrand();
            z[i][j] = 0;
        }
        for (int k = 0; k < 4; k++)
        for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) z[i][j] += x[i][k] * y[k][j];

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) printf("%08x ", *((int *)&x[i][j]), x[i][j]);
            putchar('\n');
        }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) printf("%08x ", *((int *)&y[i][j]), x[i][j]);
            putchar('\n');
        }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) printf("%08x ", *((int *)&z[i][j]), x[i][j]);
            putchar('\n');
        }
        putchar('\n');


    }
    fclose(stdout);
    return 0;
}
