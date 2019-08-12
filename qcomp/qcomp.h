#ifndef QCOMP_H_
#define QCOMP_H_

///Src code params
#define MAX_FILE_LEN 10000
#define MAX_FILENAME_LEN 100
#define MAX_LINE_LEN 100
#define MAX_PARAM_LEN 30

///Resrc params
#define MAX_QUBIT_NUM 15
#define MAX_QUBITS_IN_GATE 3
#define OP_NUM 9

///Architecture parameter definition
/*#define IS_RUNNING_PORT 0x1000000
#define START_RUNNING_PORT 0x1000001
#define W_1_ARRAY_BASE_ADDR 0x0
#define W_2_ARRAY_BASE_ADDR 0x100
#define RES_ARRAY_BASE_ADDR 0x200*/

#include <cstring>
#include<string>
#include<map>
#include "matrix.h"

enum gateType{
    toffoli, cnot, px, py, pz, rx, ry, rz, measure
};

class qcompiler{
    char filename[MAX_FILENAME_LEN];
    char outputfname[MAX_FILENAME_LEN];
    char buffer[MAX_FILE_LEN];
    char curLine[MAX_LINE_LEN];
    int fileLen;
    FILE *fin, *fout;
    std::map<std::string, int> qubits;
    std::map<std::string, int> bits;
    std::map<std::string, int> opers;
    std::map<std::string, int> ngates;
    std::map<std::string, matrix> nmatrices;
    int qcnt, bcnt, scnt;
    bool readErr;

    int rpt_times;

    bool test;

    void loadfilename(const char *fn);
    void loadfile();


    void comp_by_line();
    void compile_qubit_defs();
    void compile_bit_defs();
    void compile_gate_defs();

    void compile_inst();
    void compile_insts();

    void do_toffoli(int g1, int g2, int g3);
    void do_cnot(int g1, int g2);
    void do_x(int g);
    void do_y(int g);
    void do_z(int g);

    void do_rx(int g, float theta);
    void do_ry(int g, float theta);
    void do_rz(int g, float theta);

    void do_measure(int g1, int g2);

    void init_target();
    void def();
    void def_fake_rand();

    void test_output();
    void test_output_header();

    void do_gate(matrix &mat, int g1, int g2);


public:

    qcompiler();
    ~qcompiler();



    void compile(const char *filename, int times = 1);


};
#endif
