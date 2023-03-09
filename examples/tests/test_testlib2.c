
#include "atest.h"
//
// IMPORTANT: inlcuding .c allows testing static functions!
//            also no need to include all headers/sources except testlib2.c in Makefile
//
#include "../src/testlib2.c"

/* Initialializers called for every test */
ATEST_SETUP_F(void)
{
    return NULL;   // if no shutdown logic needed
}

ATEST_F(assert_pow){

    atassert(pow2(3) != 9);
    return NULL;
}

ATEST_F(assert_passed)
{
    atassert(mul(2, 2) == 4);
    atassert(pow2(3) == 9);

    return NULL; // Every ATEST_F() must return NULL to succeed!
}

int main(int argc, char *argv[])
{
    //FILE * f = fopen("test.log", "w");
    //__ATestContext.out_stream = f;
    __ATestContext.verbosity = 3;

    ATEST_PARSE_MAINARGS(argc, argv);
    ATEST_PRINT_HEAD();

    // <ATEST_RUNS>   -- do not replace to make 'atest' utility automatic test updates
    ATEST_RUN(assert_passed);
    ATEST_RUN(assert_pow);
    // ^^^^ this block is automatically generated by 'atest' utility

    ATEST_PRINT_FOOTER();
    //fclose(f);
    
    return ATEST_EXITCODE();
}
