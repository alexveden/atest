#include <stdlib.h>
#include <stdio.h>
#include "testlib2.h"

int add(int a, int b)
{
    return a + b + mul(a, b);
}

int add2(int a){
    return add(a, a);
}

void public_puts_func_mock(void){
    puts("public_abort_func_mock");
}
static void static_puts_func_mock(){
    puts("static_puts_func_mock");
}

void putchar_func(int a){
    static_puts_func_mock();
    putchar('T');
}

