#include<cmath>

class matrix{
public:
    float matR[4][4], matI[4][4];       ///Re, Im part
    void matmul(matrix & y);
    void init();
    void do_cnot(int g);
    void do_x(int g);
    void do_y(int g);
    void do_z(int g);
    void do_rx(int g, float theta);
    void do_ry(int g, float theta);
    void do_rz(int g, float theta);
    void print();
};

