#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
uint32_t ext_ram[1024*1024];
uint32_t base_ram[1024*1024];

uint64_t prog_file[1024*1024];
int main(int argc, char** argv){
    if(argc==1) {
        printf("usage: %s <ram_file>\n", argv[0]);
        return 1;
    }
    memset(prog_file, 0, sizeof(prog_file));
    memset(ext_ram, 0, sizeof(ext_ram));
    memset(base_ram, 0, sizeof(base_ram));


    FILE* ram=fopen(argv[1], "rb");
    if(!ram){
        printf("open file failed.\n");
        return 2;
    }
    int ret=fread(prog_file, 1, sizeof(prog_file), ram);
    printf("reading %d bytes\n", ret);
    for(int i=0;i<1024*1024;i++){
        base_ram[i]=prog_file[i]&0x00000000ffffffff;
        ext_ram[i]=prog_file[i]>>32;
    }
    printf("writing everything back.");
    FILE* ext_ram_file=fopen("ext_ram.bin", "wb");
    FILE* base_ram_file=fopen("base_ram.bin", "wb");
    if(!ext_ram_file || !base_ram_file){
        printf("open ext_ram / base_ram failed.\n");
        return 3;
    }

    fwrite(ext_ram, sizeof(ext_ram), 1, ext_ram_file);
    fwrite(base_ram, sizeof(base_ram), 1, base_ram_file);
    printf("everything done.\n");
    return 0;
}
