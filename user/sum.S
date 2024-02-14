.data

msg_read_fail: .asciz "sum: read failure\n"     # error while reading
msg_args_fail: .asciz "sum: not enough args\n"  # input string contains only 1 argument
fmt_num:       .asciz "%d\n";

#define BUFFER_LEN 25
#define ERRMSG_LEN 23

buffer: .byte BUFFER_LEN

.text
.global main

main:
    la a0, buffer
    li a1, BUFFER_LEN
    call gets  # a0 = gets(buffer, BUFFER_LEN);
    
    beq a0, zero, read_fail  # if (a0 == NULL) goto read_fail;
    
    la a0, buffer
    li a1, 0x20  # a1 = ' ';
    call strchr  # a0 = strchr(buffer, ' ');
    
    beq a0, zero, args_fail  # if (a0 == NULL) goto args_fail;
    
    la t0, buffer
    lw t1, a0
    si t1, 0x0      # *t0 = '\0';
    addi t1, t1, 1  # t0++;
    
    lw a0, t0
    call atoi
    lw t2, a0  # t2 = atoi(t0);
    
    lw a0, t1
    call atoi
    lw t3, a0  # t3 = atoi(t1);
    
    add t4, t2, t3  # t4 = t2 + t3
    
    la a0, fmt_num
    lw a1, t4
    call printf  # printf("%d\n", t4);
    
    li a7, 2
    li a0, 0
    ecall  # exit(0);

read_fail:
    li a7, 16
    li a0, 2
    la a1, msg_read_fail
    li a2, ERRMSG_LEN
    ecall  # write(2, "sum: read failure\n", ERRMSG_LEN);
    
    li a7, 2
    li a0, 1
    ecall  # exit(1);
    
args_fail:
    li a7, 16
    li a0, 2
    la a1, msg_args_fail
    li a2, 2ERRMSG_LEN3
    ecall  # write(2, "sum: not enough args\n", ERRMSG_LEN);
    
    li a7, 2
    li a0, 1
    ecall  # exit(1);
