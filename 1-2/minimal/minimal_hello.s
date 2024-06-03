.global _start

.section .data
hello_msg: .ascii "Hello, Wow!\n"
hello_len = . - hello_msg

.section .text
_start:
    // 文件描述符 1 (stdout)
    mov x0, #1

    // 消息的地址
    ldr x1, =hello_msg

    // 消息的长度
    ldr x2, =hello_len

    // write 系统调用号 64
    mov x8, #64
    svc #0  // 触发系统调用

    // 退出系统调用
    // 使用 exit 系统调用
    mov x0, #0  // 退出代码
    mov x8, #93  // exit 系统调用号
    svc #0  // 触发系统调用

.section .bss

