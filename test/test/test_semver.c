#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "unity.h"
#include "semver.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * static variables
 ******************************************************************************/
 //
char *g_valid_semver_strings[] =
{
    "1.0.8",
    "1.23.7",
    "2.0.0-alpha.123.abc",
    "2.0.0-alpha.123.abc+build.acebfde1284",
    "1.0.0-alpha",
    "1.0.0-alpha.1",
    "1.0.0-0.3.7",
    "1.0.0-x.7.z.92",
    "1.0.0-alpha",
    "1.0.0-alpha.1",
    "1.0.0-beta.2",
    "1.0.0-beta.11",
    "1.0.0-rc.1",
    "1.0.0-rc.1+build.1",
    "1.0.0-rc.1+build.1-b",
    "1.0.0",
    "1.0.0+0.3.7",
    "1.3.7+build",
    "1.3.7+build.2.b8f12d7",
    "1.3.7+build.11.e0f985a",
    "1.3.7+build.11.e0f9-85a",
    "1.0.0+build-acbe",
    "2.0.0+build.acebfde1284-alpha.123.abc",
};

char *g_invalid_semver_strings[] =
{
    "a.b.c",
    "1",
    "1.0.0b",
    "1.0",
    "1.0.0+b[\\]^_`uild", // [,\,],^,_,` are between A-z, but not A-Za-z
    "1.0.0+build-acbe.", // trailing period 
    "1.0.0+build.!@#$%",
};


/******************************************************************************
 * static function prototypes
 ******************************************************************************/

/******************************************************************************
 * non-static function definitions
 ******************************************************************************/

void setUp(void) {

}

void tearDown(void) {}

void test_semver_create_destroy(void)
{
    semver_t *p_semver = NULL;

    TEST_ASSERT_EQUAL(1, semver_create(NULL));
    TEST_ASSERT_EQUAL(1, semver_create(NULL));

    TEST_ASSERT_EQUAL(0,semver_create(&p_semver));

    TEST_ASSERT_NOT_EQUAL(NULL, p_semver);

    TEST_ASSERT_EQUAL(0,semver_destroy(p_semver));

    return;
}

void test_semver_get_set_major_minor_patch(void)
{
    semver_t *p_semver;
    semver_create(&p_semver);

    //Testing NullParams
    TEST_ASSERT_NOT_EQUAL(0, semver_get_major(NULL));
    TEST_ASSERT_NOT_EQUAL(0, semver_get_minor(NULL));
    TEST_ASSERT_NOT_EQUAL(0, semver_get_patch(NULL));
    TEST_ASSERT_NOT_EQUAL(0, semver_set_major(NULL, 0));
    TEST_ASSERT_NOT_EQUAL(0, semver_set_major(NULL, 0));
    TEST_ASSERT_NOT_EQUAL(0, semver_set_patch(NULL, 0));

    //Initially, zero should be returned for all versions
    TEST_ASSERT_EQUAL(0, semver_get_major(p_semver));
    TEST_ASSERT_EQUAL(0, semver_get_minor(p_semver));
    TEST_ASSERT_EQUAL(0, semver_get_patch(p_semver));

    //Now, lets set values, and then get them.
    TEST_ASSERT_EQUAL(0, semver_set_major(p_semver, 5));
    TEST_ASSERT_EQUAL(5, semver_get_major(p_semver));
    TEST_ASSERT_EQUAL(0, semver_set_minor(p_semver, 4));
    TEST_ASSERT_EQUAL(4, semver_get_minor(p_semver));
    TEST_ASSERT_EQUAL(0, semver_set_patch(p_semver, 3));
    TEST_ASSERT_EQUAL(3, semver_get_patch(p_semver));

    semver_destroy(p_semver);
}

void test_set_get_pr_str(void)
{
    semver_t *p_semver = NULL;
    char valid_pr_str[] = "1.1.2"; //A valid pre-release string
    char invalid_pr_str[] = "nope"; //An obviously invalid pre-release string
    char *out_sv_str = NULL;
    uint16_t out_sv_str_len = 0;;

    semver_create(&p_semver);

    //Test NULL params
    TEST_ASSERT_NOT_EQUAL(0, semver_set_pr_str(NULL,valid_pr_str,1));
    TEST_ASSERT_NOT_EQUAL(0, semver_set_pr_str(p_semver,NULL,1));

    //Test the getting and setting of a valid pre-release string
    TEST_ASSERT_EQUAL(0, semver_set_pr_str(p_semver,
                                           valid_pr_str,
                                           strlen(valid_pr_str)));

    TEST_ASSERT_EQUAL(0, semver_get_pr_str(p_semver,
                                           &out_sv_str,
                                           &out_sv_str_len));

    //Is the reported length correct?s
    TEST_ASSERT_EQUAL(out_sv_str_len, strlen(valid_pr_str));

    //Is the actual content correct? 
    TEST_ASSERT_EQUAL_MEMORY(valid_pr_str, out_sv_str, sizeof(valid_pr_str));

    semver_destroy(p_semver);
}

void test_set_get_bmd_str(void)
{
    semver_t *p_semver = NULL;
    char valid_bmd_str[] = "1.1.2"; //A valid pre-release string
    char invalid_bmd_str[] = "nope"; //An obviously invalid pre-release string
    char *out_sv_str = NULL;
    uint16_t out_sv_str_len = 0;;

    semver_create(&p_semver);

    //Test NULL params
    TEST_ASSERT_NOT_EQUAL(0, semver_set_bmd_str(NULL,valid_bmd_str,1));
    TEST_ASSERT_NOT_EQUAL(0, semver_set_bmd_str(p_semver,NULL,1));

    //Test the getting and setting of a valid pre-release string
    TEST_ASSERT_EQUAL(0, semver_set_pr_str(p_semver,
                                           valid_bmd_str,
                                           strlen(valid_bmd_str)));

    TEST_ASSERT_EQUAL(0, semver_get_pr_str(p_semver,
                                           &out_sv_str,
                                           &out_sv_str_len));

    //Is the reported length correct?s
    TEST_ASSERT_EQUAL(out_sv_str_len, strlen(valid_bmd_str));

    //Is the actual content correct? 
    TEST_ASSERT_EQUAL_MEMORY(valid_bmd_str, out_sv_str, sizeof(valid_bmd_str));

    semver_destroy(p_semver);
}

void test_semver_compare_basic(void)
{
    semver_t *p_sva = NULL;
    semver_t *p_svb = NULL;
    int result = 0;

    semver_create(&p_sva);
    semver_create(&p_svb);

    //This test will not be as thorough, as it's actually a little
    //bit of a pain to go around setting semvers in the not-string-fashion.
    //We will do some more testing for the string version below.

    //These just establish the most basic functionality of the
    //required components.

    //Test NULL params
    TEST_ASSERT_NOT_EQUAL(0, semver_compare(NULL, p_svb, &result));
    TEST_ASSERT_NOT_EQUAL(0, semver_compare(p_sva, NULL, &result));
    TEST_ASSERT_NOT_EQUAL(0, semver_compare(p_sva, p_svb, NULL));

    //semver A gets 5.4.3
    semver_set_major(p_sva, 5);
    semver_set_minor(p_sva, 4);
    semver_set_patch(p_sva, 3);

    //Semver B gets 4.5.6
    semver_set_major(p_svb, 4);
    semver_set_minor(p_svb, 5);
    semver_set_patch(p_svb, 6);

    //We expect that semver B precedes semver A
    TEST_ASSERT_EQUAL(0, semver_compare(p_sva,p_svb,&result));
    TEST_ASSERT_EQUAL(1, result);

    //Semver B gets 4.4.3
    semver_destroy(p_sva);
    semver_destroy(p_svb);
}

void test_semver_to_str(void)
{
    semver_t *p_sva = NULL;
    char* semver_str = NULL;
    int semver_str_len = 0;
    int result = 0;
    
    char pr_str[] = "rc.3.2.1";
    char bmd_str[] = "sha.5114f85";
    
    semver_create(&p_sva);

    //Test for NULL params failures
    TEST_ASSERT_NOT_EQUAL(0, semver_to_str(NULL, &semver_str, &semver_str_len));
    TEST_ASSERT_NOT_EQUAL(0, semver_to_str(p_sva, NULL, &semver_str_len));
    TEST_ASSERT_NOT_EQUAL(0, semver_to_str(p_sva, &semver_str, NULL));
    
    TEST_ASSERT_EQUAL(0,semver_set_major(p_sva, 5));
    TEST_ASSERT_EQUAL(0,semver_set_minor(p_sva, 4));
    TEST_ASSERT_EQUAL(0,semver_set_patch(p_sva, 3));
    TEST_ASSERT_EQUAL(0, semver_set_pr_str(p_sva, pr_str, sizeof(pr_str)));
    TEST_ASSERT_EQUAL(0, semver_set_bmd_str(p_sva, bmd_str, sizeof(bmd_str)));
    
    result = semver_to_str(p_sva, &semver_str, &semver_str_len);
    TEST_ASSERT_EQUAL(0,result);
    TEST_ASSERT_EQUAL_STRING(semver_str,"5.4.3-rc.3.2.1+sha.5114f85");
}

void test_semver_str_is_valid(void)
{
    char semver_str[] = "5.4.3-6.77.8+meow";
    int i;
    int result;

    //Test for NULL param failures
    TEST_ASSERT_NOT_EQUAL(0,semver_str_is_valid(NULL,0));

    for(i=0;i< sizeof(g_valid_semver_strings)/sizeof(char*);i++)
    {
        result = semver_str_is_valid(g_valid_semver_strings[i],
                                    strlen(g_valid_semver_strings[i]));
        TEST_ASSERT_EQUAL(0, result);
    }

    for(i=0;i< sizeof(g_invalid_semver_strings)/sizeof(char*);i++)
    {
        result = semver_str_is_valid(g_invalid_semver_strings[i],
                                    strlen(g_invalid_semver_strings[i]));
        TEST_ASSERT_NOT_EQUAL(0, result);
    }

}

void test_semver_str_to_semver(void)
{
    char semver_str[] = "5.4.3-rc.3.2.1+sha.5114f85";
    semver_t *p_semver;
    int result;
    char *p_new_semver_str;
    int new_semver_str_len;

    //Test for Null param failures
    TEST_ASSERT_NOT_EQUAL(0, semver_str_to_semver(NULL, strlen(semver_str), &p_semver) );
    TEST_ASSERT_NOT_EQUAL(0, semver_str_to_semver(semver_str, strlen(semver_str),NULL) );

    //Test to see if our semver str was converted to a semver correctly.
    result = semver_str_to_semver(semver_str, strlen(semver_str), &p_semver);
    TEST_ASSERT_EQUAL(0,result);

    result = semver_to_str(p_semver, &p_new_semver_str, &new_semver_str_len);
    TEST_ASSERT_EQUAL(0,result);
    TEST_ASSERT_EQUAL_STRING(semver_str,p_new_semver_str);
}

/******************************************************************************
 * static function definitions
 ******************************************************************************/
