.data

msg_read_fail: .asciz "sum: read failure\n"     # error while reading
msg_args_fail: .asciz "sum: not enough args\n"  # input string contains only 1 argument
fmt_num:       .asciz "%d\n";

#define BUFFER_LEN 25
#define READ_ERRMSG_LEN 18
#define ARGS_ERRMSG_LEN 21

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
    mv t1, a0
    sb zero, 0(t1)  # *t1 = '\0';
    addi t1, t1, 1  # t1++;
    
    mv a0, t0
    call atoi
    mv t2, a0  # t2 = atoi(t0);
    
    mv a0, t1
    call atoi
    mv t3, a0  # t3 = atoi(t1);
    
    add t4, t2, t3  # t4 = t2 + t3
    
    la a0, fmt_num
    mv a1, t4
    call printf  # printf("%d\n", t4);
    
    li a7, 2
    li a0, 0
    ecall  # exit(0);

read_fail:
    li a7, 16
    li a0, 2
    la a1, msg_read_fail
    li a2, READ_ERRMSG_LEN
    ecall  # write(2, "sum: read failure\n", ERRMSG_LEN);
    
    li a7, 2
    li a0, 1
    ecall  # exit(1);
    
args_fail:
    li a7, 16
    li a0, 2
    la a1, msg_args_fail
    li a2, ARGS_ERRMSG_LEN
    ecall  # write(2, "sum: not enough args\n", ERRMSG_LEN);
    
    li a7, 2
    li a0, 1
    ecall  # exit(1);
