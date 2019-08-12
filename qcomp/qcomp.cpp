#include <cstdio>
#include "qcomp.h"
#include <cmath>
#include <fstream>

#define BASE_ADDR_X 0x0
#define BASE_ADDR_Y 0x1
#define BASE_ADDR_Z 0x20
#define EMITTER 0x40
#define STATUS 0x42
#define BASE_ADDR_RES 0x50

using namespace std;

char temp_buf[5000];

const int opParamNum[OP_NUM] = {3, 2, 1, 1, 1, 0, 0, 0, 4};
const int pow2[MAX_QUBIT_NUM + 1] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};

void qcompiler::loadfilename(const char *fn) {
    strcpy(filename, fn);
    strcpy(outputfname, fn);
    int len = strlen(fn);
    if(len<5 || (strcmp(fn+len-5, ".qasm")!=0)){
        printf("The suffix must be .qasm!\n");
        exit(1);
    }
    outputfname[len] = '.';
    outputfname[len+1] = 'c';
    outputfname[len+2] = 0;
    std::ifstream fin(filename);
    if(!fin){
        printf("input file name not exist!\n");
        exit(1);
    }
    printf("Load filename: %s\n", filename);
    printf("Output filename: %s\n", outputfname);
}

void qcompiler::loadfile() {
    fin = fopen(filename, "r");
    fileLen = fread(buffer, 1, MAX_FILE_LEN, fin);
    printf("FileLen: %d\n", fileLen);
    fclose(fin);
    buffer[fileLen] = '\0';
    printf("%s\n", buffer);
}

qcompiler::qcompiler() {
    //filename = new char[MAX_FILENAME_LEN];
    int cnt = 0;
    test = false;
    opers.insert(pair<string, int>(string("toffoli"), cnt++));
    opers.insert(pair<string, int>(string("cnot"), cnt++));
    opers.insert(pair<string, int>(string("x"), cnt++));
    opers.insert(pair<string, int>(string("y"), cnt++));
    opers.insert(pair<string, int>(string("z"), cnt++));
    opers.insert(pair<string, int>(string("rx"), cnt++));
    opers.insert(pair<string, int>(string("ry"), cnt++));
    opers.insert(pair<string, int>(string("rz"), cnt++));
    opers.insert(pair<string, int>(string("measure"), cnt++));
    printf("Qcompiler built!\n");

}

qcompiler::~qcompiler() {
    //delete [] filename;
}

void qcompiler::comp_by_line() {
    fin = fopen(filename, "r");
    if (fgets(curLine, MAX_LINE_LEN, fin) == NULL) {
        printf("FileErr!n");
        readErr = true;
        return;
    }
    //else printf("This is: %s\n", curLine);
    compile_gate_defs();
    compile_qubit_defs();
    compile_bit_defs();

    init_target();

    compile_insts();
    fclose(fin);
}

void qcompiler::def_fake_rand() {
    fprintf(fout, "float get_rand() {;\n");
    fprintf(fout, "    return rand() / 32767.0;\n");
    fprintf(fout, "};\n");
}

void qcompiler::def() {
    //fprintf(fout, "#include<cstdio>\n\n");
    /*fprintf(fout, "Def. qubit_count = %d;\n", qcnt);
    fprintf(fout, "     max = %d;\n", pow2[qcnt]-1);
    fprintf(fout, "     float fRe[0:max] = {};\n");
    fprintf(fout, "     float fIm[0:max] = {};\n");*/

    fprintf(fout, "#define qubit_count %d\n", qcnt);
    fprintf(fout, "#define bit_count %d\n", bcnt);
    fprintf(fout, "#define max %d\n", pow2[qcnt]-1);
    fprintf(fout, "#define times %d\n", rpt_times);
    fprintf(fout, "int pow2[qubit_count] = {};\n");
    ///fprintf(fout, "int val[bit_count] = {};\n");
    fprintf(fout, "int fRe[max+1] = {};\n");
    fprintf(fout, "int fIm[max+1] = {};\n");
    fprintf(fout, "volatile int* const mem = 0x0000000044a10000;\n");
    fprintf(fout, "volatile unsigned int* const LED=0x0000000040000000;\n");
    fprintf(fout, "void print();\n\n");
    ///fprintf(fout, "float scale, ran, prob;\n");

    /*fprintf(fout, "Funct. Def. void swap(i, j) {\n");
    fprintf(fout, "     float temp = fRe[i]; fRe[i] = fRe[j]; fRe[j] = temp;\n");
    fprintf(fout, "           temp = fIm[i]; fIm[i] = fIm[j]; fIm[j] = temp;\n");
    fprintf(fout, "}\n");
    fprintf(fout, "\n");*/

    fprintf(fout, "void swap(int i, int j) {\n");
    fprintf(fout, "     int temp = fRe[i]; fRe[i] = fRe[j]; fRe[j] = temp;\n");
    fprintf(fout, "         temp = fIm[i]; fIm[i] = fIm[j]; fIm[j] = temp;\n");
    fprintf(fout, "}\n");
    fprintf(fout, "\n");

    if (test) def_fake_rand();

    fprintf(fout, "int i, j, u, v, w, x, y;\n");
    fprintf(fout, "void work(){\n");

    fprintf(fout, "    for (i = 0; i <= max; i++) fRe[i] = fIm[i] = 0;\n");
    fprintf(fout, "    fRe[0] = 0x3f800000;\n");
    //fprintf(fout, "    for (int i = 0; i < bit_count; i++) val[i] = -1;\n");
    fprintf(fout, "\n");




    /*fprintf(fout, "Def. rand_seed = 1;\n");
    fprintf(fout, "Funct. Def. fix_point rand()\n");*/
}

void qcompiler::init_target() {
    def();
}

void qcompiler::compile_inst() {
    char oper[MAX_PARAM_LEN], params[MAX_PARAM_LEN];
    char param[MAX_QUBITS_IN_GATE][MAX_PARAM_LEN];             ///At most 3-qubit gate?

    int x[MAX_QUBITS_IN_GATE];
    //printf("cur: %s\n", curLine);
    printf("Compiling Inst\n");
    sscanf(curLine, "%s%s", oper, params);
    //printf("%s\n%s\n", oper, params);

    if (opers.find(oper) != opers.end()) {
        int op = (gateType) opers[oper];
        float theta = 0;
        char *p1 = &(params[0]), *p2 = &(param[0][0]);

        if (opParamNum[op] == 1) {
            while (*p1 != '\0') *(p2++) = *(p1++);
            *p2 = '\0';
            x[0] = qubits[param[0]];
        }
        else if (opParamNum[op] == 2) {
            while (*p1 != ',') *(p2++) = *(p1++);
            *p2 = '\0'; p2 = &(param[1][0]); p1++;
            while (*p1 != '\0') *(p2++) = *(p1++);
            *p2 = '\0';
            x[0] = qubits[param[0]]; x[1] = qubits[param[1]];
        } else if (opParamNum[op] == 3){
            while (*p1 != ',') *(p2++) = *(p1++);
            *p2 = '\0'; p2 = &(param[1][0]); p1++;
            while (*p1 != ',') *(p2++) = *(p1++);
            *p2 = '\0'; p2 = &(param[2][0]); p1++;
            while (*p1 != '\0') *(p2++) = *(p1++);
            *p2 = '\0';
            x[0] = qubits[param[0]]; x[1] = qubits[param[1]]; x[2] = qubits[param[2]];
        } else if (opParamNum[op] == 0) {
            while (*p1 != ',') *(p2++) = *(p1++);
            *p2 = '\0'; p2 = &(param[1][0]); p1++;
            while (*p1 != '\0') *(p2++) = *(p1++);
            *p2 = '\0';
            x[0] = qubits[param[0]];
            sscanf(param[1], "%f", &theta);
            printf("theta: %f\n", theta);
        } else if (opParamNum[op] == 4) {
            while (*p1 != ',') *(p2++) = *(p1++);
            *p2 = '\0'; p2 = &(param[1][0]); p1++;
            while (*p1 != '\0') *(p2++) = *(p1++);
            *p2 = '\0';
            x[0] = qubits[param[0]]; x[1] = bits[param[1]];
        }

        printf("inst: %d %d", (int)op, x[0]);
        if (opParamNum[op] > 1) printf(" %d", x[1]);
        if (opParamNum[op] > 2) printf(" %d", x[2]);
        putchar('\n');

        switch (op) {
            case toffoli:
                do_toffoli(x[0], x[1], x[2]);
                break;
            case cnot:
                do_cnot(x[0], x[1]);
                break;
            case px:
                do_x(x[0]);
                break;
            case py:
                do_y(x[0]);
                break;
            case pz:
                do_z(x[0]);
                break;
            case rx:
                do_rx(x[0], theta);
                break;
            case ry:
                do_ry(x[0], theta);
                break;
            case rz:
                do_rz(x[0], theta);
                break;
            case measure:
                //printf("Measure: %d %d\n", x[0], x[1]);
                do_measure(x[0], x[1]);
                break;
            default:
                break;
        }
    }
    else if (nmatrices.find(oper) != nmatrices.end()) {
        //mat.print();
        printf("SelfDef Gate\n");
        //printf("%s\n", params);
        int g1, g2;
        sscanf(params, "%d,%d", &g1, &g2);
        //printf("Doing special gate\n");
        do_gate(nmatrices[oper], g1, g2);
    }
    else {
        readErr = true;
        printf("Err!\n");
        return;
    }


    /*char *p1 = &(s2[0]), *p2 = &(s3[0]);
    while (*p1 != ',') *(p2++) = *(p1++);
    *p2 = '\0'; p2 = &(s4[0]); p1++;
    while (*p1 != ',') *(p2++) = *(p1++);
    *p2 = '\0'; p2 = &(s5[0]); p1++;
    while (*p1 != '\0') *(p2++) = *(p1++);
    *p2 = '\0';
    //printf("%s || %s || %s\n", s3, s4, s5);

    x1 = qubits[s3]; x2 = qubits[s4]; x3 = qubits[s5];
    printf("%d %d %d\n", x1, x2, x3);*/

}

void qcompiler::compile_insts() {
    printf("Compiling instructions.\n");
    compile_inst();
    while (fgets(curLine, MAX_LINE_LEN, fin) != NULL) compile_inst();
    printf("Done.\n");
}

void qcompiler::test_output_header() {
    if (test) fprintf(fout, "#include<cstdio>\n");
    if (test) fprintf(fout, "#include<cmath>\n");
    if (test) fprintf(fout, "#include<cstdlib>\n");
    if (test) fprintf(fout, "#include<ctime>\n");
}

void qcompiler::test_output() {
    fprintf(fout, "    for (i = 0; i <= max; i++) printf(\"%%d: %%f + i * %%f\\n\", i, fRe[i], fIm[i]);\n");
    fprintf(fout, "    printf(\"\\n\");\n");
    fprintf(fout, "    for (i = 0; i < bit_count; i++) printf(\"val[%%d]: %%d\\n\", i, val[i]);\n");
    fprintf(fout, "    printf(\"\\n\");\n");
}

void qcompiler::compile(const char* filename, int times) {
    if (times == 0) return;
    rpt_times = times;
    printf("Compiling %s\n", filename);
    qcnt = 0;
    bcnt = 0;
    scnt = 0;
    readErr = false;
    loadfilename(filename);
    fout = fopen(outputfname, "w");
    test_output_header();
    comp_by_line();

    if (test) test_output();

    fprintf(fout, "}\n");
    fprintf(fout, "int main(){\n");
    if (test) fprintf(fout, "    srand(time(NULL)); rand();\n");
    fprintf(fout, "    pow2[0] = 1;\n");
    fprintf(fout, "    for (i = 1; i < qubit_count; i++) pow2[i] = pow2[i-1] << 1;\n\n");
    fprintf(fout, "    for (y = 0; y < times; y++) {\n");
    fprintf(fout, "         work();\n");
    fprintf(fout, "    }\n");
    fprintf(fout, "    for (y = 0; y <= max; y++) {\n");
    fprintf(fout, "        print(fRe[y]);");
    fprintf(fout, "    }\n");
    fprintf(fout, "    return 0;\n");
    fprintf(fout, "}\n");

    FILE *finclude = fopen("include.txt", "r");
    int llen = fread(temp_buf, 1, 5000, finclude);
    printf("Read %d\n", llen);
    fwrite(temp_buf, 1, llen, fout);
    fclose(finclude);
    fclose(fout);

    printf("Done compilation.\n");
}

void qcompiler::compile_qubit_defs() {
    char s1[MAX_PARAM_LEN], s2[MAX_PARAM_LEN];
    sscanf(curLine, "%s%s", s1, s2);
    //printf("%s || %s\n", s1, s2);
    while (strcmp(s1, "qubit") == 0) {              ///Indeed a qubit def
        //printf("Add new def!\n");
        qubits.insert(pair<string, int>(string(s2), qcnt++));
        //int res = qubits[s2];
        //printf("res: %d\n", res);

        if (fgets(curLine, MAX_LINE_LEN, fin) == NULL){
             printf("FileErr1\n");
             readErr = true;
             break;
        }
        sscanf(curLine, "%s%s", s1, s2);
    }
    for (map<string, int>::iterator it = qubits.begin(); it != qubits.end(); it++) {
        printf("%s %d\n", it->first.c_str(), it->second);
    }

}

void qcompiler::compile_bit_defs() {
    char s1[MAX_PARAM_LEN], s2[MAX_PARAM_LEN];
    sscanf(curLine, "%s%s", s1, s2);
    //printf("%s || %s\n", s1, s2);
    while (strcmp(s1, "bit") == 0) {              ///Indeed a qubit def
        //printf("Add new def!\n");
        bits.insert(pair<string, int>(string(s2), bcnt++));
        //int res = qubits[s2];
        //printf("res: %d\n", res);

        if (fgets(curLine, MAX_LINE_LEN, fin) == NULL){
             printf("FileErr1\n");
             readErr = true;
             break;
        }
        sscanf(curLine, "%s%s", s1, s2);
    }
    for (map<string, int>::iterator it = bits.begin(); it != bits.end(); it++) {
        printf("%s %d\n", it->first.c_str(), it->second);
    }

}


void qcompiler::do_toffoli(int g1, int g2, int g3) {
    /*fprintf(fout, "For i: (1, max) if ((pow2[%d]&i) && (pow2[%d]&i) && (pow2[%d]&i)) {\n", g1, g2, g3);
    fprintf(fout, "     j = i ^ pow2[%d];\n", g3);
    fprintf(fout, "     swap(i, j);\n");
    fprintf(fout, "}\n");*/

    fprintf(fout, "    for (int i = 1; i <= max; i++) if ((pow2[%d]&i) && (pow2[%d]&i) && (pow2[%d]&i)) {\n", g1, g2, g3);
    fprintf(fout, "         int j = i ^ pow2[%d];\n", g3);
    fprintf(fout, "         swap(i, j);\n");
    fprintf(fout, "    }\n");
}

void qcompiler::do_cnot(int g1, int g2) {
    fprintf(fout, "    for (int i = 1; i <= max; i++) if ((pow2[%d]&i) && (pow2[%d]&i)) {\n", g1, g2);
    fprintf(fout, "         int j = i ^ pow2[%d];\n", g2);
    fprintf(fout, "         swap(i, j);\n");
    fprintf(fout, "    }\n");
}

void qcompiler::do_x(int g) {
    fprintf(fout, "    for (int i = 1; i <= max; i++) if (pow2[%d]&i) {\n", g);
    fprintf(fout, "         int j = i ^ pow2[%d];\n", g);
    fprintf(fout, "         swap(i, j);\n");
    fprintf(fout, "    }\n");
}

void qcompiler::do_z(int g) {
    fprintf(fout, "    for (int i = 1; i <= max; i++) if (pow2[%d]&i) {\n", g);
    fprintf(fout, "         fRe[i] = -fRe[i]; fIm[i] = -fIm[i];\n", g);
    fprintf(fout, "    }\n");
}

void qcompiler::do_y(int g) {
    fprintf(fout, "    for (int i = 1; i <= max; i++) if (pow2[%d]&i) {\n", g);
    fprintf(fout, "         int j = i ^ pow2[%d];\n", g);
    fprintf(fout, "         float temp = fRe[i]; fRe[i] = -fIm[j]; fIm[j] = -temp;\n", g);
    fprintf(fout, "               temp = fIm[i]; fIm[i] = fRe[j]; fRe[j] = temp;\n", g);
    fprintf(fout, "    }\n");
}

void qcompiler::do_rx(int g, float theta) {
    theta = theta / 2;
    float ct = cos(theta), st = sin(theta);
    fprintf(fout, "    for (int i = 1; i <= max; i++) if (pow2[%d]&i) {\n", g);
    fprintf(fout, "         int j = i ^ pow2[%d];\n", g);
    fprintf(fout, "         float temp = fRe[i]; fRe[i] = (%f)*fRe[i]+(%f)*fIm[j];\n              fIm[j] = (%f)*fIm[j]+(%f)*temp;\n", ct, st, ct, -st);
    fprintf(fout, "               temp = fIm[i]; fIm[i] = (%f)*fIm[i]+(%f)*fRe[j];\n              fRe[j] = (%f)*FRe[j]+(%f)*temp;\n", ct, -st, ct, st);
    fprintf(fout, "    }\n");
}

void qcompiler::do_ry(int g, float theta) {
    theta = theta / 2;
    float ct = cos(theta), st = sin(theta);
    fprintf(fout, "    for (int i = 1; i <= max; i++) if (pow2[%d]&i) {\n", g);
    fprintf(fout, "         int j = i ^ pow2[%d];\n", g);
    fprintf(fout, "         float temp = fRe[i]; fRe[i] = (%f)*fRe[i]+(%f)*fRe[j];\n              fRe[j] = (%f)*fRe[j]+(%f)*temp;\n", ct, st, ct, -st);
    fprintf(fout, "               temp = fIm[i]; fIm[i] = (%f)*fIm[i]+(%f)*fIm[j];\n              fIm[j] = (%f)*fIm[j]+(%f)*temp;\n", ct, st, ct, -st);
    fprintf(fout, "    }\n");
}

void qcompiler::do_rz(int g, float theta) {
    theta = theta / 2;
    float ct = cos(theta), st = sin(theta);
    fprintf(fout, "    for (int i = 1; i <= max; i++) if (pow2[%d]&i) {\n", g);
    fprintf(fout, "         int j = i ^ pow2[%d];\n", g);
    fprintf(fout, "         float temp = fRe[i]; fRe[i] = (%f)*fRe[i]+(%f)*fIm[i];\n              fIm[i] = (%f)*fIm[i]+(%f)*temp;\n", ct, -st, ct, st);
    fprintf(fout, "               temp = fRe[j]; fRe[j] = (%f)*fRe[j]+(%f)*fIm[j];\n              fIm[j] = (%f)*FIm[j]+(%f)*temp;\n", ct, st, ct, -st);
    fprintf(fout, "    }\n");
}

void qcompiler::do_measure(int g1, int g2) {
    printf("Measure: %d %d\n", g1, g2);
    fprintf(fout, "    prob = 0;\n");
    fprintf(fout, "    for (int i = 1; i <= max; i++) if (pow2[%d]&i) {\n", g1);
    fprintf(fout, "         prob += fRe[i]*fRe[i] + fIm[i]*fIm[i];\n");
    fprintf(fout, "    }\n");

    fprintf(fout, "    ran = get_rand();\n");
    fprintf(fout, "    if (ran < prob) {\n");
    fprintf(fout, "        val[%d] = 1;\n", g2);
    fprintf(fout, "        scale = sqrt(prob);\n");
    fprintf(fout, "        for (int i = 0; i <= max; i++) \n");
    fprintf(fout, "            { if (pow2[%d]&i) { fRe[i] /= scale; fIm[i] /= scale;} else {fRe[i] = 0; fIm[i] = 0;} }\n", g1);
    fprintf(fout, "    } else {\n");
    fprintf(fout, "        val[%d] = 0;\n", g2);
    fprintf(fout, "        scale = sqrt(1 - prob);\n");
    fprintf(fout, "        for (int i = 0; i <= max; i++) \n");
    fprintf(fout, "            { if (pow2[%d]&i) { fRe[i] = 0; fIm[i] = 0;} else {fRe[i] /= scale; fIm[i] /= scale;} }\n", g1);
    fprintf(fout, "    }\n");

}


void qcompiler::compile_gate_defs() {
    char s1[MAX_PARAM_LEN], s2[MAX_PARAM_LEN], name[MAX_PARAM_LEN];
    printf("Compiling gate defs...\n");
    sscanf(curLine, "%s%s", s1, s2);

    while (strcmp(s1, "gatedef") == 0) {
        strcpy(name, s2);
        ngates.insert(pair<string, int>(string(name), scnt++));
        matrix nmatrix;
        nmatrix.init();
        if (fgets(curLine, MAX_LINE_LEN, fin) == NULL){
             printf("FileErr1\n");
             readErr = true;
             break;
        }
        sscanf(curLine, "%s%s", s1, s2);
        while (strcmp(s1, "endgatedef") != 0) {
            if (opers.find(s1) != opers.end()) {
                int op = (gateType) opers[s1];
                float theta = 0;
                int g1, g2;
                switch (op) {
                case cnot:
                    sscanf(s2, "%d,%d", &g1, &g2);
                    nmatrix.do_cnot(g1);
                    break;
                case px:
                    sscanf(s2, "%d", &g1);
                    nmatrix.do_x(g1);
                    break;
                case py:
                    sscanf(s2, "%d", &g1);
                    nmatrix.do_y(g1);
                    break;
                case pz:
                    sscanf(s2, "%d", &g1);
                    nmatrix.do_z(g1);
                    break;
                case rx:
                    sscanf(s2, "%d,%f", &g1, &theta);
                    nmatrix.do_rx(g1, theta);
                    break;
                case ry:
                    sscanf(s2, "%d,%f", &g1, &theta);
                    nmatrix.do_ry(g1, theta);
                    break;
                case rz:
                    sscanf(s2, "%d,%f", &g1, &theta);
                    nmatrix.do_rz(g1, theta);
                    break;
                default:
                    break;
                }
            }
            if (fgets(curLine, MAX_LINE_LEN, fin) == NULL){
                 printf("FileErr1\n");
                 readErr = true;
                 break;
            }
            sscanf(curLine, "%s%s", s1, s2);
        }
        nmatrices.insert(pair<string, matrix>(string(name), nmatrix));
        if (fgets(curLine, MAX_LINE_LEN, fin) == NULL){              ///skip "endgatedef"
             printf("FileErr1\n");
             readErr = true;
             break;
        }
        sscanf(curLine, "%s%s", s1, s2);
    }
    for (map<string, int>::iterator it = ngates.begin(); it != ngates.end(); it++) {
        printf("%s %d\n", it->first.c_str(), it->second);
    }
    /*for (map<string, matrix>::iterator it = nmatrices.begin(); it != nmatrices.end(); it++) {
        printf("%s\n", it->first.c_str());
        it->second.print();
    }*/
}


void qcompiler::do_gate(matrix &mat, int g1, int g2) {
    printf("Doing gate\n");
    int g3 = -1, g4 = -1;
    if (qcnt >= 4) {
        for (int i = 0; i < qcnt; i++) if ((i != g1) && (i != g2)) {
            if (g3 == -1) g3 = i;
            else {
                g4 = i; break;
            }
        }

        fprintf(fout, "    while (mem[%d] == 1);\n ", STATUS);         ///wait for co-processor to be free
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            fprintf(fout, "    mem[%d] = 0x%x;\n", BASE_ADDR_X + i * 8 + j * 2, *(int *)&mat.matR[i][j]);   ///Load X

            fprintf(fout, "    for (i = 1; i <= max; i++)\n");
            fprintf(fout, "        if ((pow2[%d]&i) && (pow2[%d]&i) && (pow2[%d]&i) && (pow2[%d]&i)) {\n", g1, g2, g3, g4);
            fprintf(fout, "            for (u = 1; u >= 0; u--)\n");
            fprintf(fout, "            for (v = 1; v >= 0; v--)\n");
            fprintf(fout, "            for (w = 1; w >= 0; w--)\n");
            fprintf(fout, "            for (x = 1; x >= 0; x--){\n");
            fprintf(fout, "                int index = i - (x?pow2[%d]:0) - (w?pow2[%d]:0) - (v?pow2[%d]:0) - (u?pow2[%d]:0);\n", g2, g1, g3, g4);
            fprintf(fout, "                int L_index = 15 - ((u << 1) + v + (w << 3) + (x << 2));\n");
            fprintf(fout, "                mem[%d + (L_index << 1)] = fRe[index];\n", BASE_ADDR_Y);
            fprintf(fout, "            }\n\n");                                                          ///Load Y

            fprintf(fout, "            mem[%d] = 1; //start working\n", EMITTER);
            fprintf(fout, "            while (mem[%d] == 1);\n ", STATUS);
            fprintf(fout, "            for (u = 1; u >= 0; u--)\n");
            fprintf(fout, "            for (v = 1; v >= 0; v--)\n");
            fprintf(fout, "            for (w = 1; w >= 0; w--)\n");
            fprintf(fout, "            for (x = 1; x >= 0; x--){\n");
            fprintf(fout, "                int index = i - (x?pow2[%d]:0) - (w?pow2[%d]:0) - (v?pow2[%d]:0) - (u?pow2[%d]:0);\n", g2, g1, g3, g4);
            fprintf(fout, "                int L_index = 15 - ((u << 1) + v + (w << 3) + (x << 2));\n");
            fprintf(fout, "                fRe[index] = mem[%d + (L_index << 1)];\n", BASE_ADDR_Z);
            fprintf(fout, "            }\n\n");
            fprintf(fout, "        }\n");


    }
    else if (qcnt == 3){
        for (int i = 0; i < qcnt; i++) if ((i != g1) && (i != g2)) {
            g3 = i; break;
        }

        fprintf(fout, "    while (mem[%d] == 1);\n ", STATUS);         ///wait for co-processor to be free
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            fprintf(fout, "    mem[%d] = 0x%x;\n", BASE_ADDR_X + i * 8 + j * 2, *(int *)&mat.matR[i][j]);   ///Load X

            fprintf(fout, "            for (v = 1; v >= 0; v--)\n");
            fprintf(fout, "            for (w = 1; w >= 0; w--)\n");
            fprintf(fout, "            for (x = 1; x >= 0; x--){\n");
            fprintf(fout, "                int index = 7 - (x?pow2[%d]:0) - (w?pow2[%d]:0) - (v?pow2[%d]:0);\n", g2, g1, g3);
            fprintf(fout, "                int L_index = 13 - (v + (w << 3) + (x << 2));\n");
            fprintf(fout, "                mem[%d + (L_index << 1)] = fRe[index];\n", BASE_ADDR_Y);
            fprintf(fout, "            }\n\n");                                                          ///Load Y

            fprintf(fout, "            mem[%d] = 1; //start working\n", EMITTER);
            fprintf(fout, "            while (mem[%d] == 1);\n ", STATUS);

            fprintf(fout, "            for (v = 1; v >= 0; v--)\n");
            fprintf(fout, "            for (w = 1; w >= 0; w--)\n");
            fprintf(fout, "            for (x = 1; x >= 0; x--){\n");
            fprintf(fout, "                int index = 7 - (x?pow2[%d]:0) - (w?pow2[%d]:0) - (v?pow2[%d]:0);\n", g2, g1, g3);
            fprintf(fout, "                int L_index = 13 - (v + (w << 3) + (x << 2));\n");
            fprintf(fout, "                fRe[index] = mem[%d + (L_index << 1)];\n", BASE_ADDR_Z);
            fprintf(fout, "            }\n\n");

    }
    else if (qcnt == 2) {
        fprintf(fout, "    while (mem[%d] == 1);\n ", STATUS);         ///wait for co-processor to be free
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            fprintf(fout, "    mem[%d] = 0x%x;\n", BASE_ADDR_X + i * 8 + j * 2, *(int *)&mat.matR[i][j]);   ///Load X

            fprintf(fout, "            for (w = 1; w >= 0; w--)\n");
            fprintf(fout, "            for (x = 1; x >= 0; x--){\n");
            fprintf(fout, "                int index = 3 - (x?pow2[%d]:0) - (w?pow2[%d]:0);\n", g2, g1);
            fprintf(fout, "                int L_index = 12 - ((w << 3) + (x << 2));\n");
            fprintf(fout, "                mem[%d + (L_index << 1)] = fRe[index];\n", BASE_ADDR_Y);
            fprintf(fout, "            }\n\n");                                                          ///Load Y

            fprintf(fout, "            mem[%d] = 1; //start working\n", EMITTER);
            fprintf(fout, "            while (mem[%d] == 1);\n ", STATUS);

            fprintf(fout, "            for (w = 1; w >= 0; w--)\n");
            fprintf(fout, "            for (x = 1; x >= 0; x--){\n");
            fprintf(fout, "                int index = 3 - (x?pow2[%d]:0) - (w?pow2[%d]:0);\n", g2, g1, g3);
            fprintf(fout, "                int L_index = 12 - ((w << 3) + (x << 2));\n");
            fprintf(fout, "                fRe[index] = mem[%d + (L_index << 1)];\n", BASE_ADDR_Z);
            fprintf(fout, "            }\n\n");
    }
    else printf("Error!\n");

}
