echo "Compiling test2.qasm to ram file..."
./qcomp test2.qasm
riscv64-unknown-elf-gcc init.S test2.qasm.c  -o test2.qsim -static -mcmodel=medany -fvisibility=hidden -nostartfiles -march=rv64i -mabi=lp64  -T link.ld
riscv64-unknown-elf-objcopy -O binary test2.qsim test2.bin
./split_ram test2.bin
