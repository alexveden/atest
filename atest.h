//
//
// C mimimalist unit testing framework
//    Alex Vedeneev (c) 2023 MIT License
//
//
#ifndef __ATEST_H
#define __ATEST_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#ifndef NAN
#error "NAN is undefined on this platform"
#endif

#ifndef FLT_EPSILON
#error "FLT_EPSILON is undefined on this platform"
#endif 

#define ATEST_AMSG_MAX_LEN 512   // max length for aassertf() string formatting

//
//  atest.h settings, you can alter them in main() function, for example replace out_stream by fopen() file
//
struct __ATestContext_s {
    FILE* out_stream;    // by default uses stdout, however you can replace it by any FILE* stream
    int tests_run;       // number of tests run
    int tests_failed;    // number of tests failed
    int verbosity;       // verbosity level: 0 - only stats, 1 - short test results (. or F) + stats, 2 - all tests results, 3 - include logs    
    char _str_buf[ATEST_AMSG_MAX_LEN];  // Internal buffer for aassertf() string formatting
};
extern struct __ATestContext_s __ATestContext;

/*
 *
 *  ASSERTS
 *
 */
//
// atassert( condition ) - simple assert, failes when condition false
// 
#define atassert(A) do {\
    if(!(A)) { \
        return __ATEST_LOG_ERR(#A);} \
    } while(0);

//
// atassertf( condition, format_string, ...) - assert with printf() style formatting error message  
//
#define atassertf(A, M, ...) do { \
    if(!(A)) { \
        snprintf(__ATestContext._str_buf, ATEST_AMSG_MAX_LEN-1, __ATEST_LOG_ERR(M), ##__VA_ARGS__); \
        return __ATestContext._str_buf; } \
    } while(0);

//
// atassert_eqs(char * actual, char * expected) - compares strings via strcmp(), (NULL friendly) 
//
#define atassert_eqs(ac, ex) do {\
    int __at_assert_passed = 1; \
    if ((ac) == NULL && (ex) != NULL) {__at_assert_passed = 0; } \
    else if ((ac) != NULL && (ex) == NULL) {__at_assert_passed = 0;} \
    else if ((ac) == NULL && (ex) == NULL) {__at_assert_passed = 1; } \
    else if (strcmp((ac), (ex)) != 0) {__at_assert_passed = 0; } \
    if (__at_assert_passed == 0){\
        snprintf(__ATestContext._str_buf, ATEST_AMSG_MAX_LEN-1, __ATEST_LOG_ERR("'%s' != '%s'"), (char*)(ac), (char*)(ex)); \
        return __ATestContext._str_buf; } \
} while(0);

//
// atassert_eqi(int actual, int expected) - compares equality of integers
//
#define atassert_eqi(ac, ex) do {\
    if ((ac) != (ex)) {\
        snprintf(__ATestContext._str_buf, ATEST_AMSG_MAX_LEN-1, __ATEST_LOG_ERR("%d != %d"), (ac), (ex)); \
        return __ATestContext._str_buf; } \
} while(0);

//
// atassert_eqf(double actual, double expected) - compares equiality of floating point numbers, NAN friendly, i.e. atassert_eqf(NAN, NAN) -- passes
//
#define atassert_eqf(ac, ex) do {\
    int __at_assert_passed = 1; \
    if (isnan(ac) && !isnan(ex)) __at_assert_passed = 0; \
    else if (!isnan(ac) && isnan(ex)) __at_assert_passed = 0; \
    else if (isnan(ac), isnan(ex)) __at_assert_passed = 1;   \
    else if ( fabs( (ac) - (ex) ) > (double)FLT_EPSILON) __at_assert_passed = 0; \
    if (__at_assert_passed == 0) {\
        snprintf(__ATestContext._str_buf, ATEST_AMSG_MAX_LEN-1, __ATEST_LOG_ERR("%f != %f (diff: %0.10f epsilon: %0.10f)"), (ac), (ex), ((ac)-(ex)), (double)FLT_EPSILON); \
        return __ATestContext._str_buf; } \
} while(0);

//
// atlogf() - simple log message with additional formatting and information about what file and line of code was caller of the alogf() 
//
#define atlogf(M, ...) if(__ATestContext.verbosity >= 3) {fprintf(__atest_stream, __ATEST_LOG(M), ##__VA_ARGS__);} 


/*
 *
 *  TEST FUNCTIONS
 *
 */

//
//  ATEST_SETUP_F() is mandatory for every test file, 
//  only one instance of this function is allowed.
//
//  This function is launched before every test is run.
//
//  ATEST_SETUP_F() may return pointer to atest_shutdown function, or NULL if no such function.
//
// // Bacic function, no setup and no shutdown
// ATEST_SETUP_F(void)
// {     
//     return NULL;
// }
//
// // Shutting down after every tests
// void shutdown(void){
//     printf("atest_shutdown()\n");
// }
//
// ATEST_SETUP_F(void)
// {
//     printf("atest_setup()\n");
//     return &shutdown;
// }
typedef void (*atest_shutdown_ptr)(void); 
atest_shutdown_ptr __atest_setup_func();

#define ATEST_SETUP_F  \
     struct __ATestContext_s __ATestContext = { \
        .out_stream = NULL, \
        .tests_run = 0,   \
        .tests_failed = 0, \
        .verbosity = 3, \
     }; \
     atest_shutdown_ptr __atest_setup_func
//
// Typical test function template
//  MUST return NULL on test success, or char* with error message when failed!
//
#define ATEST_F(TESTNAME) \
    char * TESTNAME()

//
// To be used in main() test launching function
//  
//  ATEST_F(test_ok){  .... tests code here ..}
//  
//  int main(...) { ... 
//        ATEST_RUN(test_ok);
//  }
#define ATEST_RUN(TESTNAME) do { \
    atest_shutdown_ptr shutdown_fun_p = __atest_setup_func();  \
    char * result = (TESTNAME()); \
    __ATestContext.tests_run++; \
    if (result == NULL) { \
        if (__ATestContext.verbosity > 0) { \
            if (__ATestContext.verbosity == 1) {fprintf(__atest_stream, ".");} \
            else {fprintf(__atest_stream, "[PASS] %s\n", #TESTNAME);}   \
        }\
    } \
    else { \
        if (__ATestContext.verbosity > 0) { \
            if (__ATestContext.verbosity == 1) {fprintf(__atest_stream, "F");} \
            else {fprintf(__atest_stream, "[FAIL] %s (%s)\n", result, #TESTNAME);} \
        }\
        __ATestContext.tests_failed++; \
    }   \
    if (shutdown_fun_p != NULL) {(*shutdown_fun_p)();}\
} while(0)

//
//  Prints current test header 
//
#define ATEST_PRINT_HEAD() do {\
	setbuf(__atest_stream, NULL); \
	if (__ATestContext.verbosity > 0) { \
	    fprintf(__atest_stream, "-------------------------------------\n"); \
	    fprintf(__atest_stream, "Running Tests: %s\n", __FILE__);  \
	    fprintf(__atest_stream, "-------------------------------------\n\n"); \
    } \
    fflush(0); \
} while(0)

//
// Prints current tests stats total / passed / failed
//
#define ATEST_PRINT_FOOTER() do {\
	if (__ATestContext.verbosity > 0) { \
	    fprintf(__atest_stream, "\n-------------------------------------\n"); \
        fprintf(__atest_stream, "Total: %d Passed: %d Failed: %d\n", __ATestContext.tests_run, __ATestContext.tests_run - __ATestContext.tests_failed, __ATestContext.tests_failed); \
	    fprintf(__atest_stream, "-------------------------------------\n"); \
	} else {\
        fprintf(__atest_stream, "[%s] %-40s [%2d/%2d]\n", \
                __ATestContext.tests_failed == 0 ? "PASS" : "FAIL", \
                __FILE__, \
                __ATestContext.tests_run - __ATestContext.tests_failed,\
                __ATestContext.tests_run); \
    } \
} while (0)

//
//  Utility macro for returning main() exit code based on test failed/run, if no tests run it's an error too
//
#define ATEST_PARSE_MAINARGS(argc, argv) do { \
        if (argc == 1) break; \
        if (argc > 2){ \
            fprintf(__atest_stream, "Too many arguments: use test_name_exec vvv\n"); \
            return 1; } \
        char a; int i = 0; \
        int _has_quiet = 0; int _verb = 0;\
        while ((a = argv[1][i]) != '\0') { \
            if (a == 'q') _has_quiet = 1; \
            if (a == 'v') _verb++; \
            i++; \
        } \
        __ATestContext.verbosity = _has_quiet ? 0 : _verb; \
    } while(0); 

#define ATEST_EXITCODE() (-1 ? __ATestContext.tests_run == 0 : __ATestContext.tests_failed)

/*
 *
 * Utility non public macros
 *
 */
#define __atest_stream (__ATestContext.out_stream == NULL ? stdout : __ATestContext.out_stream) 
#define __STRINGIZE_DETAIL(x) #x
#define __STRINGIZE(x) __STRINGIZE_DETAIL(x)
#define __ATEST_LOG_ERR(msg) ("(" __FILE__ ":" __STRINGIZE(__LINE__) "): " msg)
#define __ATEST_LOG(msg) ("[ LOG] (" __FILE__ ":" __STRINGIZE(__LINE__) "): " msg)


#endif // !__ATEST_H
