#include "qcomp.h"
#include<cstdio>

void matrix::print() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) printf("%f+i(%f) ", matR[i][j], matI[i][j]);
        putchar('\n');
    }
}

void matrix::do_cnot(int g) {
    if (g == 0) {          ///cnot 0,1  10-11
        for (int i = 0; i < 4; i++) {
            float temp = matR[2][i]; matR[2][i] = matR[3][i]; matR[3][i] = temp;
            temp = matI[2][i]; matI[2][i] = matI[3][i]; matI[3][i] = temp;
        }
    }
    else {                ///cnot 1,0   01-11
        for (int i = 0; i < 4; i++) {
            float temp = matR[1][i]; matR[1][i] = matR[3][i]; matR[3][i] = temp;
            temp = matI[1][i]; matI[1][i] = matI[3][i]; matI[3][i] = temp;
        }
    }
}

void matrix::do_x(int g) {
    if (g == 0) {
        for (int i = 0; i < 4; i++) {
            float temp;
            temp = matR[2][i]; matR[2][i] = matR[0][i]; matR[0][i] = temp;
            temp = matI[2][i]; matI[2][i] = matI[0][i]; matI[0][i] = temp;
            temp = matR[1][i]; matR[1][i] = matR[3][i]; matR[3][i] = temp;
            temp = matI[1][i]; matI[1][i] = matI[3][i]; matI[3][i] = temp;
        }
    }
    else if (g == 1) {
        for (int i = 0; i < 4; i++) {
            float temp;
            temp = matR[2][i]; matR[2][i] = matR[3][i]; matR[3][i] = temp;
            temp = matI[2][i]; matI[2][i] = matI[3][i]; matI[3][i] = temp;
            temp = matR[1][i]; matR[1][i] = matR[0][i]; matR[0][i] = temp;
            temp = matI[1][i]; matI[1][i] = matI[0][i]; matI[0][i] = temp;
        }
    }
}

void matrix::do_z(int g) {
    if (g == 0) {
        for (int i = 0; i < 4; i++) {
            matR[2][i] = -matR[2][i]; matI[2][i] = -matI[2][i];
            matR[3][i] = -matR[3][i]; matI[3][i] = -matI[3][i];
        }
    }
    else if (g == 1) {
        for (int i = 0; i < 4; i++) {
            matR[1][i] = -matR[1][i]; matI[1][i] = -matI[1][i];
            matR[3][i] = -matR[3][i]; matI[3][i] = -matI[3][i];
        }
    }
}

void matrix::do_y(int g) {
    if (g == 0) {
        for (int i = 0; i < 4; i++) {
            float temp = matR[0][i]; matR[0][i] = -matI[2][i]; matI[2][i] = -temp;
            temp = matI[0][i]; matI[0][i] = matR[2][i]; matR[2][i] = temp;
            temp = matR[1][i]; matR[1][i] = -matI[3][i]; matI[3][i] = -temp;
            temp = matI[1][i]; matI[1][i] = matR[3][i]; matR[3][i] = temp;
        }
    }
    else if (g == 1) {
        for (int i = 0; i < 4; i++) {
            float temp = matR[0][i]; matR[0][i] = -matI[1][i]; matI[1][i] = -temp;
            temp = matI[0][i]; matI[0][i] = matR[1][i]; matR[1][i] = temp;
            temp = matR[2][i]; matR[2][i] = -matI[3][i]; matI[3][i] = -temp;
            temp = matI[2][i]; matI[2][i] = matR[3][i]; matR[3][i] = temp;
        }
    }
}

void matrix::do_rx(int g, float theta) {
    float ct = cos(theta/2), st = sin(theta / 2);
    if (g == 0) {
        for (int i = 0; i < 4; i++) {
            float temp = matR[0][i]; matR[0][i] = matR[0][i] * ct + matI[2][i] * st; matI[2][i] = matI[2][i] * ct - temp * st;
            temp = matI[0][i]; matI[0][i] = matI[0][i] * ct - matR[2][i] * st; matR[2][i] = matR[2][i] * ct + temp * st;
            temp = matR[1][i]; matR[1][i] = matR[1][i] * ct + matI[3][i] * st; matI[3][i] = matI[3][i] * ct - temp * st;
            temp = matI[1][i]; matI[1][i] = matI[1][i] * ct - matR[3][i] * st; matR[3][i] = matR[3][i] * ct + temp * st;
        }
    }
    else if (g == 1) {
        for (int i = 0; i < 4; i++) {
            float temp = matR[0][i]; matR[0][i] = matR[0][i] * ct + matI[1][i] * st; matI[1][i] = matI[1][i] * ct - temp * st;
            temp = matI[0][i]; matI[0][i] = matI[0][i] * ct - matR[1][i] * st; matR[1][i] = matR[1][i] * ct + temp * st;
            temp = matR[2][i]; matR[2][i] = matR[2][i] * ct + matI[3][i] * st; matI[3][i] = matI[3][i] * ct - temp * st;
            temp = matI[2][i]; matI[2][i] = matI[2][i] * ct - matR[3][i] * st; matR[3][i] = matR[3][i] * ct + temp * st;
        }
    }
}

void matrix::do_ry(int g, float theta) {
    float ct = cos(theta/2), st = sin(theta / 2);
    if (g == 0) {
        for (int i = 0; i < 4; i++) {
            float temp = matR[0][i]; matR[0][i] = matR[0][i] * ct - matR[2][i] * st; matR[2][i] = matR[2][i] * ct + temp * st;
            temp = matI[0][i]; matI[0][i] = matI[0][i] * ct - matI[2][i] * st; matI[2][i] = matI[2][i] * ct + temp * st;
            temp = matR[1][i]; matR[1][i] = matR[1][i] * ct - matR[3][i] * st; matR[3][i] = matR[3][i] * ct + temp * st;
            temp = matI[1][i]; matI[1][i] = matI[1][i] * ct - matI[3][i] * st; matI[3][i] = matI[3][i] * ct + temp * st;
        }
    }
    else if (g == 1) {
        for (int i = 0; i < 4; i++) {
            float temp = matR[0][i]; matR[0][i] = matR[0][i] * ct - matR[1][i] * st; matR[1][i] = matR[1][i] * ct + temp * st;
            temp = matI[0][i]; matI[0][i] = matI[0][i] * ct - matI[1][i] * st; matI[1][i] = matI[1][i] * ct + temp * st;
            temp = matR[2][i]; matR[2][i] = matR[2][i] * ct - matR[3][i] * st; matR[3][i] = matR[3][i] * ct + temp * st;
            temp = matI[2][i]; matI[2][i] = matI[2][i] * ct - matI[3][i] * st; matI[3][i] = matI[3][i] * ct + temp * st;
        }
    }
}

void matrix::do_rz(int g, float theta) {
    float ct = cos(theta/2), st = sin(theta / 2);
    if (g == 0) {
        for (int i = 0; i < 4; i++) {
            float temp = matR[0][i]; matR[0][i] = matR[0][i] * ct + matI[0][i] * st; matI[0][i] = matI[0][i] * ct - temp * st;
            temp = matR[1][i]; matR[1][i] = matR[1][i] * ct + matI[1][i] * st; matI[1][i] = matI[1][i] * ct - temp * st;
            temp = matR[2][i]; matR[2][i] = matR[2][i] * ct - matI[2][i] * st; matI[2][i] = matI[2][i] * ct + temp * st;
            temp = matR[3][i]; matR[3][i] = matR[3][i] * ct - matI[3][i] * st; matI[3][i] = matI[3][i] * ct + temp * st;
        }
    }
    else if (g == 1) {
        for (int i = 0; i < 4; i++) {
            float temp = matR[0][i]; matR[0][i] = matR[0][i] * ct + matI[0][i] * st; matI[0][i] = matI[0][i] * ct - temp * st;
            temp = matR[2][i]; matR[2][i] = matR[2][i] * ct + matI[2][i] * st; matI[2][i] = matI[2][i] * ct - temp * st;
            temp = matR[1][i]; matR[1][i] = matR[1][i] * ct - matI[1][i] * st; matI[1][i] = matI[1][i] * ct + temp * st;
            temp = matR[3][i]; matR[3][i] = matR[3][i] * ct - matI[3][i] * st; matI[3][i] = matI[3][i] * ct + temp * st;
        }
    }
}

void matrix::matmul(matrix & y) {
    float tempR[4][4], tempI[4][4];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) tempR[i][j] = tempI[i][j] = 0;
    for (int k = 0; k < 4; k++)
        for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++){
            tempR[i][j] += matR[i][k] * y.matR[k][j] - matI[i][k] * matI[k][j];
            tempI[i][j] += matR[i][k] * y.matI[k][j] + matR[i][k] * matI[k][j];
        }
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            matR[i][j] = tempR[i][j];
            matI[i][j] = tempI[i][j];
        }
}

void matrix::init() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            if (i != j) matR[i][j] = 0; else matR[i][j] = 1;
            matI[i][j] = 0;
        }
}
int main(int argc, char** argv) {
    if(argc<2){
        printf("usage: %s <input qasm>\n", argv[0]);
        return 1;
    }
    printf("Qcompiler working.\n");
    qcompiler qc;
    //qc.loadfilename("test.qasm");
    //qc.loadfile();
    qc.compile(argv[1]);
    printf("Job done.\n");
    return 0;
}
