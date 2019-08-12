#include<cstdio>
#define BASE_ADDR_X 0x0
#define BASE_ADDR_Y 0x1
#define BASE_ADDR_Z 0x20
#define EMITTER 0x40
#define STATUS 0x42
#define BASE_ADDR_RES 0x50
int x[4][4], y[4][4], z[4][4];
int times = 5;
const char* SERIAL_CODE=R"EOF(volatile unsigned long long* const SERIAL_PORT=0x0000000044a00000;
volatile unsigned long long* const SERIAL_STATUS=0x0000000044a00008;
char get_char(){
    char status;
    char data;
    while(1){
        status=*SERIAL_STATUS;
        if(status&2){
            data=*SERIAL_PORT;
            *LED=data;
            return data;
        }
    }
}

void write_char(char chr){
    char status;
    while(1){
        status=*SERIAL_STATUS;
        if(status&1){
            *SERIAL_PORT=chr;
            return;
        }
    }
}
char table[]="0123456789ABCDEF";
void print(const int val){
    int i;
    for(i=0; i<8; i++){
        write_char(table[((val>>((7-i)<<2))&0xf)]);
    }
    write_char('\n');
})EOF";
int main() {
    freopen("testdata.txt", "r", stdin);
    freopen("sys_tb.c", "w", stdout);

    //printf("int i = 0, j = 0;\n", times);
    printf("volatile int* const mem=0x0000000044a10000;\nvolatile unsigned int* const LED=0x0000000040000000;\n");
    printf("void _start(){\n");

    for (int w = 0; w < times; w++) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) scanf("%x", &x[i][j]);
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) scanf("%x", &y[i][j]);
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) scanf("%x", &z[i][j]);
        printf("    while (mem[%d] == 1);\n ", STATUS);
        for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            printf("    mem[%d] = 0x%x;\n", BASE_ADDR_X + i * 8 + j * 2, x[i][j]);
            printf("    mem[%d] = 0x%x;\n", BASE_ADDR_Y + i * 8 + j * 2, y[i][j]);
        }
        printf("    mem[%d] = 1; //start working\n", EMITTER);
        printf("    while (mem[%d] == 1);\n ", STATUS);
        for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        printf("    print(mem[%d]);\n", BASE_ADDR_Z + i * 8 + j * 2);
        }


    printf("    while(1);\n}\n");
    printf("%s\n", SERIAL_CODE);
    fclose(stdin);
    fclose(stdout);
    return 0;
}
