#include "thread.h"

void Ta(int id) {
    while(1) {
        // printf("%d", id);
    }
}
void Tb(int id, int what) {
    while(1) {
        // printf("%d", what);
    }
}
int main() {
    create((void *)Ta);
    create((void *)Tb);
    join();
}