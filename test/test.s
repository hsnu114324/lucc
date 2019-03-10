	.data   
.L25:   
	.string "result: %d\n"  
	.text   
.global main   
main:   
	push %rbp  	# int_arith_2122
	mov %rsp, %rbp 
	sub $24, %rsp 	# not op_dec_787 
	mov $0, %eax 	# not op_dec_794 
	mov %eax, -8(%rbp)  	# mot op_dec_800
	mov $1, %eax 	# not op_dec_794 
	mov %eax, -16(%rbp)  	# mot op_dec_800
	mov 1(%rip), %eax 
	test %rax, %rax 
	je .L5  
.L5:   
	mov -8(%rbp), %rax 	# int_arith_563
	push %rax  	# int_arith_564 
	mov $4, %eax 
	mov %rax, %rcx 
	pop %rax  
	add %rcx, %rax 
	mov %eax, -8(%rbp)  	# op_none_1471
.L6:   
.L11:   
	mov -16(%rbp), %eax 
	push %rax 	# op_if_1095  
	mov $10, %eax 
	pop %rcx  
	cmp %rax, %rcx 
	setl %al  
	movzb %al, %eax 
	test %rax, %rax 
	je .L13  
	mov -8(%rbp), %rax 	# int_arith_563
	push %rax  	# int_arith_564 
	mov -16(%rbp), %rax 	# int_arith_563
	mov %rax, %rcx 
	pop %rax  
	add %rcx, %rax 
	mov %eax, -8(%rbp)  	# op_none_1471
	mov -16(%rbp), %rax 	# int_arith_563
	push %rax  	# int_arith_564 
	mov $1, %eax 
	mov %rax, %rcx 
	pop %rax  
	add %rcx, %rax 
	mov %eax, -16(%rbp)  	# op_none_1471
	jmp .L11  
.L13:   
	lea .L25(%rip), %rax 
	push %rax  	# op_call_1261 
	mov -8(%rbp), %eax 
	push %rax 	# op_call_1273 
	pop %rsi 	# op_call_1281 
	pop %rdi 	# op_call_1281 
	mov $0, %eax 
	call printf  
	pop %rsi 	# op_call_1290 
	# mov (%rax), %rcx 	# op_none_1454
	# mov %ecx, (%rax) 	# op_none_1455
	leave   
	ret   
