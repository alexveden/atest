
#include "atest.h"
//
// IMPORTANT: inlcuding .c allows testing static functions!
//            also no need to include all headers/sources except testlib2.c in Makefile
//
#include "../src/testlib.c"
// testlib.add() uses testlib2.mul() inside
// If we include testlib2.h we need to link testlib2.o object or include testlib2.c in GCC, OR include testlib2.c! 
#include "../src/testlib2.c"  // We can include dependent lib too as .c, so no need to build objects in Makefile too 

/* Initialializers called for every test */
ATEST_SETUP_F(void)
{
    return NULL;   // if no shutdown logic needed
}

ATEST_F(assert_add)
{
    atassertf(add(2, 2) == 4, "actual: %d != 4", add(2,2));

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
    ATEST_RUN(assert_add);
    // ^^^^ this block is automatically generated by 'atest' utility

    ATEST_PRINT_FOOTER();
    //fclose(f);
    
    return ATEST_EXITCODE();
}
