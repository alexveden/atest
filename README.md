# atest
Minimalist C Unit Testing Framework

atest.h is a micro-testing framework only ~200 lines of code. 
Unit testing in C has to be as simple as possible, this project was inspired by CuTest, mu_test, and fff.h projects.

## Installation
Copy `atest.h` into your project and you are ready to start testing!

## Features
* Minimal requirements (only single file is needed)
* Multiple assert types 
* Supports function mocking via gcc --wrap and fff.h
* Configurable context and verbosity via test command arguments
* Configurable output to any FILE* stream, by default to STDOUT
* Included `atest` utility for automatic updating new tests in the suite, just add new `ATEST_F` and `atest` will change the source to make sure to include new test function.
* Supports setup/shutdown functions


## Testing principles
1. Each unit test must be isolated and test only limited set of functions
2. Each test is a standalone executable with main
3. To run whole suite just execute all scripts in the build/ directory

### Test file structure
```c

#include "atest.h"

void my_test_shutdown_func(void){
    // instead of printf(....) consider to use alogf() it's more structural and contains line source:lnumber reference
    atlogf("atest_shutdown()\n");
}

// This function is mandatory for every test file, and must be placed at the top
ATEST_SETUP_F(void)
{
    atlogf("atest_setup()\n");
    //return NULL;   // if no shutdown logic needed
    return &my_test_shutdown_func;  // return pointer to void some_shutdown_func(void)
}

// A generic test function, must return NULL on success, or char* error_message on fail
ATEST_F(assert_pow){

    atassert(pow2(3) != 9);
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
    ATEST_RUN(assert_pow);
    // ^^^^ this block is automatically generated by 'atest' utility

    ATEST_PRINT_FOOTER();
    //fclose(f);
    
    return ATEST_EXITCODE();
}
```

### Examples
Consider to check `examples` directory for more snippets:
* General `atest` project structure and make tools
* Snippets for testing static functions
* Snippets for mocking functions with fff.h
* Snippets for wrapping system functions with `gcc -Wl,--wrap`

**Making examples**
```bash
# Go to examples directory
cd examples/tests

# Make all tests without running
make

# Make and run
make tests

# Run individual test with different level of verbosity
build/test_sample.test q
build/test_sample.test v
build/test_sample.test
build/test_testlib.test vv

```


# atest.h API
```c

// Generic assert
atassert( condition );

// Assert with formatted output, the same as printf()
atassertf( condition, const char * format_str, ...);

// Compare strings via strcmp(), (NULL friendly)
atassert_eqs(char * actual, char * expected);

// Compare equality of integers
atassert_eqi(int actual, int expected);

//Compares equality of floating point numbers, NAN friendly, i.e. atassert_eqf(NAN, NAN) -- passes
atassert_eqf(double actual, double expected); 

// Simple log message with additional formatting and information about what file and line of code was caller of the alogf()
//   Appears only when  __ATestContext.verbosity >= 3, or test script is launched with `build/test_testlib.test vvv`
atlogf(const char * format_str, ...);

```


# atest `test_sample.c`
```c

#include "atest.h"
#include <stdio.h>
#include <math.h>

void my_test_shutdown_func(void){
    // instead of printf(....) consider to use alogf() it's more structural and contains line source:lnumber reference
    atlogf("atest_shutdown()\n");
}

ATEST_SETUP_F(void)
{
    atlogf("atest_setup()\n");
    //return NULL;   // if no shutdown logic needed
    return &my_test_shutdown_func;  // return pointer to void some_shutdown_func(void)
}

ATEST_F(assert_with_formatting){
    int exp = 1;
    int act = 0;

    atassertf(exp == act, "expected: %d != actual: %d", exp, act);
    return NULL; // Every ATEST_F() must return NULL to succeed! 
}

ATEST_F(assert_eq_strings_null_expected)
{
    atassert_eqs(NULL, NULL);
    atassert_eqs("b", NULL);
    return NULL; // Every ATEST_F() must return NULL to succeed!
}
ATEST_F(assert_eq_strings_null_actual)
{
    atassert_eqs(NULL, NULL);
    atassert_eqs(NULL, "b");
    return NULL; // Every ATEST_F() must return NULL to succeed!
}
ATEST_F(assert_eq_strings)
{
    atassert_eqs("b", "b");
    atassert_eqs("b", "axb");
    return NULL; // Every ATEST_F() must return NULL to succeed!
}


ATEST_F(assert_eq_float)
{
    atassert(INFINITY == INFINITY);
    atassert(-INFINITY == -INFINITY);
    atassert(-INFINITY != INFINITY);
    atassert_eqf(0.0, 0.0);
    atassert_eqf(INFINITY, INFINITY);
    atassert_eqf(-INFINITY, -INFINITY);
    atassert(NAN != NAN);  // Originally NAN != NAN
    //atassert(NAN == NAN);  // will fail!
    atassert_eqf(NAN, NAN);  //atassert_eqf NAN == NAN, assert shoul pass 
    atassert_eqf(1.0/3.0, 1.0/3.0);
    atassert_eqf(1.0/3.0, 0.3333333);  // This one passes float epsilon test
    atassert_eqf(1.0/3.0, 0.333333);   // This one won't
    return NULL; // Every ATEST_F() must return NULL to succee!
}
ATEST_F(assert_eq_int)
{
    atassert_eqi(0, 0);
    atassert_eqi((int)(pow(3, 2)), 8);
    return NULL; // Every ATEST_F() must return NULL to succeed!
}

ATEST_F(assert_passed)
{
    atassert(1==1);
    return NULL; // Every ATEST_F() must return NULL to succeed!
}

ATEST_F(assert_fails_with_some_logging)
{
    atlogf("simple msg\n");
    atlogf("HI this test is gonna fail, because %d != %d!\n", 1, 0);

    atassert(1 == 0);
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
    ATEST_RUN(assert_with_formatting);
    ATEST_RUN(assert_eq_strings_null_expected);
    ATEST_RUN(assert_eq_strings_null_actual);
    ATEST_RUN(assert_eq_strings);
    ATEST_RUN(assert_eq_float);
    ATEST_RUN(assert_eq_int);
    ATEST_RUN(assert_passed);
    ATEST_RUN(assert_fails_with_some_logging);
    // ^^^^ this block is automatically generated by 'atest' utility

    ATEST_PRINT_FOOTER();
    //fclose(f);
    
    return ATEST_EXITCODE();
}

```
## `test_sample.c` output
```
-------------------------------------
Running Tests: test_sample.c
-------------------------------------

[ LOG] (test_sample.c:12): atest_setup()
[FAIL] (test_sample.c:21): expected: 1 != actual: 0 (assert_with_formatting)
[ LOG] (test_sample.c:7): atest_shutdown()
[ LOG] (test_sample.c:12): atest_setup()
[FAIL] (test_sample.c:28): 'b' != '(null)' (assert_eq_strings_null_expected)
[ LOG] (test_sample.c:7): atest_shutdown()
[ LOG] (test_sample.c:12): atest_setup()
[FAIL] (test_sample.c:34): '(null)' != 'b' (assert_eq_strings_null_actual)
[ LOG] (test_sample.c:7): atest_shutdown()
[ LOG] (test_sample.c:12): atest_setup()
[FAIL] (test_sample.c:40): 'b' != 'axb' (assert_eq_strings)
[ LOG] (test_sample.c:7): atest_shutdown()
[ LOG] (test_sample.c:12): atest_setup()
[FAIL] (test_sample.c:61): 0.333333 != 0.333333 (diff: 0.0000003333 epsilon: 0.0000001192) (assert_eq_float)
[ LOG] (test_sample.c:7): atest_shutdown()
[ LOG] (test_sample.c:12): atest_setup()
[FAIL] (test_sample.c:67): 9 != 8 (assert_eq_int)
[ LOG] (test_sample.c:7): atest_shutdown()
[ LOG] (test_sample.c:12): atest_setup()
[PASS] assert_passed
[ LOG] (test_sample.c:7): atest_shutdown()
[ LOG] (test_sample.c:12): atest_setup()
[ LOG] (test_sample.c:79): simple msg
[ LOG] (test_sample.c:80): HI this test is gonna fail, because 1 != 0!
[FAIL] (test_sample.c:82): 1 == 0 (assert_fails_with_some_logging)
[ LOG] (test_sample.c:7): atest_shutdown()

-------------------------------------
Total: 8 Passed: 1 Failed: 7
-------------------------------------
```

# Credits

This project was inspired by tweaks / approaches which were used in multiple other projects.

* [MinUnit](https://jera.com/techinfo/jtns/jtn002) - for 3 line concept of the unit testing framework
* [fff.h](https://github.com/meekrosoft/fff/blob/master/examples/embedded_ui/src/UI_test_ansic.c) - for setup functions and another 2 line unit testing framework
* [CuTest](https://github.com/ennorehling/cutest) - for automatic unit test generation utility idea 
