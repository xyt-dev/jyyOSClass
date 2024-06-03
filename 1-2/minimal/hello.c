#include <unistd.h>
#include <sys/syscall.h>

int main() {
    syscall(SYS_write, 1, "hello\n", 6);
    syscall(SYS_exit, 42);
}