/* Copyright 2015, Brandon Kinman
 * This file is part of The semver library.
 *
 * semver library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * semver library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "semver.h"

/******************************************************************************
 * Defines
 ******************************************************************************/
/* For string safety reasons, we limit the max id string length */
#define MAX_ID_STR_LEN 20
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * static function prototypes
 ******************************************************************************/
static int pre_release_cmp(const semver_t * p_sva, const semver_t *p_svb);
//static int pre_release_str_cmp(const char* pr_stra, const char* pr_strb);
static bool is_numeric(const char* str);

static int cmp_numeric(const char* stra, const char* strb);
static int cmp_lexical(const char* stra, const char* strb);
static int get_num_identifiers(const char* str, uint16_t str_len);
static int semver_str_validator(const char*  semver_str,
                         uint16_t semver_str_len,
                         bool *po_has_primary,
                         bool *po_has_pre_release,
                         bool *po_has_bmd);

/******************************************************************************
 * static variables
 ******************************************************************************/

/******************************************************************************
 * non-static function definitions
 ******************************************************************************/
int semver_create(semver_t **p2o_semver)
{
    if(NULL == p2o_semver)
    {
        return 1;
    }

    *p2o_semver = (semver_t*)malloc(sizeof(semver_t));

    memset(*p2o_semver,0,sizeof(semver_t));

    return 0;
}

int semver_destroy(semver_t *po_semver)
{
    if(NULL == po_semver)
    {
        return 1;
    }

    free(po_semver);

    return 0;
}

int semver_get_major(const semver_t *p_semver)
{
    if(NULL == p_semver)
    {
        return 1;
    }
    return p_semver->major;
}

int semver_get_minor(const semver_t *p_semver)
{
    if(NULL == p_semver)
    {
        return 1;
    }
    return p_semver->minor;
}

int semver_get_patch(const semver_t *p_semver)
{
    if(NULL == p_semver)
    {
        return 1;
    }

    return p_semver->patch;
}

int semver_get_pr_str(const semver_t *p_semver,
                      char** p2o_pr_str,
                      uint16_t *po_str_len)
{
    int i =0;
    uint16_t res_str_len = 0;
    if(NULL == p_semver || NULL == p2o_pr_str || NULL == po_str_len)
    {
        return 1;
    }

    //Determine the size of the resulting string.
    for(i =0; i < p_semver->num_pr_identifiers; i++)
    {
        res_str_len += strlen(p_semver->pr_identifiers[i]);
    }
    if(p_semver->num_pr_identifiers >2)
    {
        res_str_len += p_semver->num_pr_identifiers -1;
    }

    *p2o_pr_str = (char*)malloc((res_str_len+1)*sizeof(char));
    if(NULL == *p2o_pr_str)
    {
        return 1;
    }
    
    memset(*p2o_pr_str,0,(res_str_len+1)*sizeof(char));
    *po_str_len = res_str_len;

    for(i=0; i< p_semver->num_pr_identifiers; i++)
    {
        strncat(*p2o_pr_str, p_semver->pr_identifiers[i],MAX_ID_STR_LEN);
        
        if( p_semver->num_pr_identifiers > 1)
        {
            if(i < (p_semver->num_pr_identifiers -1))
            {
                strncat(*p2o_pr_str,".",1);
            }
        }
    }

    return 0;
}

int semver_get_bmd_str(const semver_t *p_semver,
                      char** p2o_bmd_str,
                      uint16_t *po_str_len)
{
    if(NULL == p_semver    || 
       NULL == p2o_bmd_str ||
       NULL == po_str_len)
    {
        return 1;
    }

    if(NULL == p_semver->bmd_str)
    {
        *p2o_bmd_str = NULL;
    }
    else
    {
        *p2o_bmd_str = p_semver->bmd_str;
        *po_str_len = p_semver->bmd_str_len;
    }

    return 0;
}

int semver_set_major(semver_t *p_semver, uint32_t major)
{
    if(NULL == p_semver)
    {
        return 1;
    }

    p_semver->major = major;

    return 0;
}

int semver_set_minor(semver_t *p_semver, uint32_t minor)
{
    if(NULL == p_semver)
    {
        return 1;
    }

    p_semver->minor = minor;
    return 0;
}

int semver_set_patch(semver_t *p_semver, uint32_t patch)
{
    if(NULL == p_semver)
    {
        return 1;
    }

    p_semver->patch = patch;
    return 0;
}

int semver_set_pr_str(semver_t *p_semver,
                      const char* pr_str,
                      uint16_t pr_str_len)
{
    int i = 0;
    char* tokenizer_str = NULL;
    char* token = NULL;

    if(NULL == p_semver || NULL == pr_str)
    {
        return 1;
    }

    //TODO: CHECK VALIDITY

    tokenizer_str = strndup(pr_str,pr_str_len);
    if(NULL == tokenizer_str)
    {
        return 1;
    }

    //If there were any previous pr strings, get rid of them.
    for(i=0;i<p_semver->num_pr_identifiers;i++)
    {
        free(p_semver->pr_identifiers[i]);
    }
    free(p_semver->pr_identifiers);
    p_semver->num_pr_identifiers = 0;

    //Create new array of strings to hold pre-release identifiers.
    p_semver->num_pr_identifiers = get_num_identifiers(pr_str, pr_str_len);
    if(p_semver->num_pr_identifiers <= 0)
    {
        free(tokenizer_str);
        return 1;
    }

    p_semver->pr_identifiers = (char**)malloc(p_semver->num_pr_identifiers*sizeof(char*));
    if(NULL == p_semver->pr_identifiers)
    {
        free(tokenizer_str);
        return 1;
    }
    
    for(i=0;i<p_semver->num_pr_identifiers;i++)
    {
        if(0 == i)
        {
            token = strtok(tokenizer_str,".");
        }
        else
        {
            token = strtok(NULL,".");
        }
        
        if(NULL != token)
        {
            p_semver->pr_identifiers[i] = strndup(token, MAX_ID_STR_LEN);
        }
    }

    free(tokenizer_str);
    return 0;
}

int semver_set_bmd_str(semver_t *p_semver,
                      const char* bmd_str,
                      uint16_t bmd_str_len)
{
    if(NULL == p_semver || NULL == bmd_str)
    {
        return 1;
    }

    //TODO: CHECK VALIDITY

    p_semver->bmd_str = strndup(bmd_str, bmd_str_len);
    if(NULL == p_semver->bmd_str)
    {
        return 1;
    }

    p_semver->bmd_str_len = strlen(p_semver->bmd_str);

    return 0;
}

int semver_to_str(const semver_t *p_semver,
                  char** p2o_semver_str,
                  int* po_len)
{
    int result_str_len = 0;
    char *result_str = NULL;
    int i = 0;
    
    if(NULL == p_semver || NULL == p2o_semver_str || NULL == po_len)
    {
        return 1;
    }
    
    result_str_len += 3*10; //First 3 digits.
    result_str_len += 3;    //potentially 3 dots
    
    //Bytes for extra identifiers
    for(i=0; i<p_semver->num_pr_identifiers; i++)
    {
        result_str_len += strlen(p_semver->pr_identifiers[i]);
    }
    
    if(p_semver->num_pr_identifiers)
    {
        //1 byte for the hyphen
        result_str_len += 1;
    }
    
    if(p_semver->num_pr_identifiers > 1)
    {
        //1 byte for each dot
        result_str_len += p_semver->num_pr_identifiers -1;
    }
    
    if(NULL != p_semver->bmd_str)
    {
        result_str_len += 1; //For the '+'
        result_str_len += strlen(p_semver->bmd_str);
    }
    
    result_str_len += 1;    //1 NULL byte
    
    result_str = (char*)malloc(result_str_len*sizeof(char));
    
    //TODO: Check validity.
    
    //Add the primary components
    snprintf(result_str,
             result_str_len,
             "%d.%d.%d",
             p_semver->major,
             p_semver->minor,
             p_semver->patch);
    
    if(p_semver->num_pr_identifiers)
    {
        strncat(result_str, "-", 1);
    }
    
    //Add thre pre-release components
    for(i=0; i<p_semver->num_pr_identifiers; i++)
    {
        strncat(result_str,
                p_semver->pr_identifiers[i],
                strlen(p_semver->pr_identifiers[i]));
        if(i<p_semver->num_pr_identifiers -1)
        {
            strncat(result_str, ".", 1);
        }
    }
    
    //Add the build meta-data
    if(NULL != p_semver->bmd_str)
    {
        strncat(result_str, "+", 1);
        strncat(result_str,
                p_semver->bmd_str,
                strlen(p_semver->bmd_str));
    }
    
    *p2o_semver_str = result_str;
    *po_len = result_str_len;
    
    return 0;
}

int semver_str_to_semver(const char* semver_str,
                         uint16_t semver_str_len,
                         semver_t **p2o_semver)
{
    semver_t *p_semver = NULL;
    bool has_primary = 0;
    bool has_pr = 0;
    bool has_bmd = 0;
    
    if(NULL == semver_str || NULL == p2o_semver)
    {
        return 1;
    }
    
    if(0 != semver_str_validator(semver_str,
                                 semver_str_len,
                                 &has_primary,
                                 &has_pr, &has_bmd))
    {
        return 1;
    }
    
    *p2o_semver = (semver_t*)malloc(sizeof(semver_t));
    memset(*p2o_semver, 0, sizeof(semver_t));
    p_semver = *p2o_semver;
    
    if(0 != p_semver->num_pr_identifiers)
    {
        int i;
        for(i=0;i<p_semver->num_pr_identifiers;i++)
        {
            free(p_semver->pr_identifiers[i]);
        }
        free(p_semver->pr_identifiers);
        p_semver->pr_identifiers = NULL;
        
    }
    
    if(has_primary)
    {
        sscanf(semver_str, "%d.%d.%d", &p_semver->major,
                                       &p_semver->minor,
                                       &p_semver->patch);
    }
    
    if(has_pr)
    {
        char* end = strrchr(semver_str, '+'); //Finds either the end, or the '+"
        char* curr_tup_start = strrchr(semver_str, '-')+1;
        char* tup_end = curr_tup_start;
        int i = 0;
        
        p_semver->num_pr_identifiers = get_num_identifiers(curr_tup_start, end - curr_tup_start);
        p_semver->pr_identifiers = (char**)malloc(p_semver->num_pr_identifiers*sizeof(char*));
        
        while(tup_end <= end)
        {
            if('.' == *tup_end || tup_end == end)
            {
                p_semver->pr_identifiers[i] = strndup(curr_tup_start, tup_end - curr_tup_start);
                if(NULL == p_semver->pr_identifiers[i])
                    return 1;
                curr_tup_start = tup_end+1;
                i++;
            }
            tup_end++;
        }
        
    }
    
    if(has_bmd)
    {
        char* bmd_str = strrchr(semver_str, '+')+1;
        p_semver->bmd_str_len = strnlen(bmd_str, MAX_ID_STR_LEN);
        p_semver->bmd_str = strndup(bmd_str, p_semver->bmd_str_len);
        if(NULL == p_semver->bmd_str)
            return 1;
    }
    
    return 0;
}

int semver_compare(const semver_t *p_sva,
                   const semver_t *p_svb,
                   int *po_result)
{
    int cmp_result;
    int major_cmp;
    int minor_cmp;
    int patch_cmp;

    if( NULL == p_sva || NULL == p_svb || NULL == po_result)
    {
        return 1;
    }

    //Peform comparisons for each required component.
    major_cmp = p_sva->major - p_svb->major;
    minor_cmp = p_sva->minor - p_svb->minor;
    patch_cmp = p_sva->patch - p_svb->patch;

    //Compute the overall comparison result.
    //Recall: we are just looking for sign.
    cmp_result = (0 != major_cmp)? major_cmp:
                 (0 != minor_cmp)? minor_cmp:
                 (0 != patch_cmp)? patch_cmp: 0;


    //If the major components were not equal,
    //we can quit now
    if(cmp_result)
    {
        *po_result = cmp_result;
        return 0;
    }

    //We know the major components are the same,
    //Or else we would have dropped out by now
    //Let's compare the pre-release components
    if(p_sva->num_pr_identifiers || p_svb->num_pr_identifiers)
    {
        //If one of these two have a pre-release component.
        return pre_release_cmp(p_sva, p_svb);
    }
    else
    {
        //We are done if neither version has a pre-release component.
        *po_result = 0;
        return 0;
    }

    return 1;
}

int semver_str_compare(const char* stra,
                       const char* strb,
                       uint16_t len,
                       int *po_result)
{
    if(NULL == stra || NULL == strb || NULL == po_result)
    {
        return 1;
    }
    return 1;
}

int semver_is_valid(const semver_t *p_semver)
{
    if(NULL == p_semver)
    {
        return 1;
    }

    return 1;
}

int semver_str_is_valid(const char* semver_str, uint8_t len)
{
    if(NULL == semver_str)
    {
        return 1;
    }
    
    return semver_str_validator(semver_str, len, NULL, NULL, NULL);

    return 1;
}

/******************************************************************************
 * static function definitions
 ******************************************************************************/
 //Returns -2 on error.
 static int pre_release_cmp(const semver_t * p_sva, const semver_t *p_svb)
 {
    int a_num;
    int b_num;
    int curr_identifier;
    if(NULL == p_sva || NULL == p_svb)
    {
        return -2;
    }

    a_num = p_sva->num_pr_identifiers;
    b_num = p_svb->num_pr_identifiers;
    curr_identifier = 0;

    //Any pre-release component has precedence over having none.
    int result =
        (0 == a_num && 0 == b_num)?  0:
        (0 != a_num && 0 == b_num)? -1:
        (0 == a_num && 0 != b_num)?  1: 2;

    if(result != 2)
    {
        return result;
    }

    //From the Spec:
    //"Precedence for two pre-release is determined by comparing each dot
    //separated identifier from left to right until a difference is found
    //as follows:"
    
    while(curr_identifier != MIN(a_num,b_num))
    {
        char *id_a = p_sva->pr_identifiers[curr_identifier];
        char *id_b = p_svb->pr_identifiers[curr_identifier];

        // "Identifiers consisting of only digits are compared numerically
        //   and identifiers with letters or hyphens are compared lexically
        //   in ASCII sort order."
        //
        //  "Numeric identifiers always have lower precedence than non-numeric
        //   identifiers.""

        result = 
            ( !is_numeric(id_a) &&  is_numeric(id_b) )?  1:
            (  is_numeric(id_a) && !is_numeric(id_b) )? -1:
        ( !is_numeric(id_a) && !is_numeric(id_b) )? cmp_lexical(id_a, id_b): cmp_numeric(id_a, id_b);

        if(0 != result)
        {
            return result;
        }
    }

    //If we have not returned yet, it's everything has been
    //equal up to this point... However, before we can say
    //that these the two pre-release components are equal,
    //we have to check whether one semver has more release components
    //than another.
    //
    // The spec says: 
    //     A larger set of pre-release fields has a higher precedence
    //     than a smaller set, if all of the preceding identifiers
    //      are equal.
    if(a_num != b_num)
    {

    }

    return 0;
 }

//  static int pre_release_str_cmp(const char* pr_stra, const char* pr_strb)
//  {
//     int bytes_to_compare = 0;
//     int i = 0;
//     int result =
//     (NULL == pr_stra && NULL == pr_strb)?  0:
//     (NULL != pr_stra && NULL == pr_strb)? -1:
//     (NULL == pr_stra && NULL != pr_strb)?  1: 2;

//     if(result != 2)
//     {
//         return result;
//     }

//     //Result of 2 indicates that both stra and strb are not NULL
//     bytes_to_compare = MAX(strlen(pr_stra),strlen(pr_strb));

//     for(i =0; i<bytes_to_compare;i++)
//     {
//         result = strcmp(pr_stra[i], pr_strb[i]);
//         if(result)
//         {
//             return result;
//         }
//     }

//     return 0;
// }

static bool is_numeric(const char* str)
{
    int result = (int)strtol(str, NULL, 10);
 
    if (result == 0 && str[0] != '0')
       return false;
    else
       return true;
}

static int cmp_numeric(const char* stra, const char* strb)
{
    int num_a = (int)strtol(stra, NULL, 10);
    int num_b = (int)strtol(strb, NULL, 10);

    return num_a - num_b;
}

static int cmp_lexical(const char* stra, const char* strb)
{
    return strcmp(stra,strb);
}

static int get_num_identifiers(const char* str, uint16_t str_len)
{
    int i;
    int num_identifiers;

    if(str_len == 0)
    {
        return 0;
    }

    num_identifiers = 1;

    for(i=0;i<str_len;i++)
    {
        if(str[i] == '.')
        {
            num_identifiers++;
        }
    }
    
    return num_identifiers;
}

/******************************************************************************
 * The following function was generated by re2c using the template included
 * at the end of this file. It is not wise to modify this function. Should
 * you need to modify this function, you should instead re-generate it using
 * a modified version of the template which may be found at the end of this
 * file.
 ******************************************************************************/
static int semver_str_validator(const char *semver_str,
                                uint16_t semver_str_len,
                                bool *po_has_primary,
                                bool *po_has_pre_release,
                                bool *po_has_bmd)
{
    bool has_primary = false;
    bool has_pre_release = false;
    bool has_bmd = false;
    
    const char *p = semver_str;
    const char *q = 0;
    
    if(NULL == semver_str)
    {
        return -1;
    }
    
#define YYCTYPE     char
#define YYCURSOR    p
#define YYLIMIT     (semver_str+semver_str_len)
#define YYMARKER    q
#define YYFILL(n)
    
    for(;;)
    {
        
        {
            YYCTYPE yych;
            
            if ((YYLIMIT - YYCURSOR) < 3) YYFILL(3);
            yych = *YYCURSOR;
            switch (yych) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':	goto yy2;
                default:	goto yy4;
            }
        yy2:
            yych = *(YYMARKER = ++YYCURSOR);
            switch (yych) {
                case '.':	goto yy5;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':	goto yy7;
                default:	goto yy3;
            }
        yy3:
            {
                return 1;
                break;
            }
        yy4:
            yych = *++YYCURSOR;
            goto yy3;
        yy5:
            yych = *++YYCURSOR;
            switch (yych) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':	goto yy9;
                default:	goto yy6;
            }
        yy6:
            YYCURSOR = YYMARKER;
            goto yy3;
        yy7:
            ++YYCURSOR;
            if ((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
            yych = *YYCURSOR;
            switch (yych) {
                case '.':	goto yy5;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':	goto yy7;
                default:	goto yy6;
            }
        yy9:
            ++YYCURSOR;
            if ((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
            yych = *YYCURSOR;
            switch (yych) {
                case '.':	goto yy11;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':	goto yy9;
                default:	goto yy6;
            }
        yy11:
            yych = *++YYCURSOR;
            switch (yych) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':	goto yy12;
                default:	goto yy6;
            }
        yy12:
            ++YYCURSOR;
            if ((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
            yych = *YYCURSOR;
            switch (yych) {
                case 0x00:	goto yy16;
                case '+':	goto yy15;
                case '-':	goto yy14;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':	goto yy12;
                default:	goto yy6;
            }
        yy14:
            yych = *++YYCURSOR;
            switch (yych) {
                case 0x00:
                case '+':
                case '.':	goto yy6;
                default:	goto yy26;
            }
        yy15:
            yych = *++YYCURSOR;
            switch (yych) {
                case 0x00:
                case '.':	goto yy6;
                default:	goto yy19;
            }
        yy16:
            ++YYCURSOR;
            {
                has_primary = true;
                break;
            }
        yy18:
            ++YYCURSOR;
            if (YYLIMIT <= YYCURSOR) YYFILL(1);
            yych = *YYCURSOR;
        yy19:
            switch (yych) {
                case 0x00:	goto yy21;
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy18;
                case '.':	goto yy20;
                default:	goto yy6;
            }
        yy20:
            ++YYCURSOR;
            if (YYLIMIT <= YYCURSOR) YYFILL(1);
            yych = *YYCURSOR;
            switch (yych) {
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy23;
                default:	goto yy6;
            }
        yy21:
            ++YYCURSOR;
            {
                has_primary = true;
                has_bmd = true;
                break;
            }
        yy23:
            ++YYCURSOR;
            if (YYLIMIT <= YYCURSOR) YYFILL(1);
            yych = *YYCURSOR;
            switch (yych) {
                case 0x00:	goto yy21;
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy23;
                case '.':	goto yy20;
                default:	goto yy6;
            }
        yy25:
            ++YYCURSOR;
            if ((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
            yych = *YYCURSOR;
        yy26:
            switch (yych) {
                case 0x00:	goto yy29;
                case '+':	goto yy28;
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy25;
                case '.':	goto yy27;
                default:	goto yy6;
            }
        yy27:
            ++YYCURSOR;
            if (YYLIMIT <= YYCURSOR) YYFILL(1);
            yych = *YYCURSOR;
            switch (yych) {
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy38;
                default:	goto yy6;
            }
        yy28:
            yych = *++YYCURSOR;
            switch (yych) {
                case 0x00:
                case '.':	goto yy6;
                default:	goto yy32;
            }
        yy29:
            ++YYCURSOR;
            {
                has_primary = true;
                has_pre_release = true;
                break;
            }
        yy31:
            ++YYCURSOR;
            if (YYLIMIT <= YYCURSOR) YYFILL(1);
            yych = *YYCURSOR;
        yy32:
            switch (yych) {
                case 0x00:	goto yy34;
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy31;
                case '.':	goto yy33;
                default:	goto yy6;
            }
        yy33:
            ++YYCURSOR;
            if (YYLIMIT <= YYCURSOR) YYFILL(1);
            yych = *YYCURSOR;
            switch (yych) {
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy36;
                default:	goto yy6;
            }
        yy34:
            ++YYCURSOR;
            {
                has_primary = true;
                has_pre_release = true;
                has_bmd = true;
                break;
            }
        yy36:
            ++YYCURSOR;
            if (YYLIMIT <= YYCURSOR) YYFILL(1);
            yych = *YYCURSOR;
            switch (yych) {
                case 0x00:	goto yy34;
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy36;
                case '.':	goto yy33;
                default:	goto yy6;
            }
        yy38:
            ++YYCURSOR;
            if ((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
            yych = *YYCURSOR;
            switch (yych) {
                case 0x00:	goto yy29;
                case '+':	goto yy28;
                case '-':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':	goto yy38;
                case '.':	goto yy27;
                default:	goto yy6;
            }
        }
        
    }
    
    if(NULL != po_has_primary)
        *po_has_primary = has_primary;
    if(NULL != po_has_pre_release)
        *po_has_pre_release = has_pre_release;
    if(NULL != po_has_bmd)
        *po_has_bmd = has_bmd;
    
    return 0;
}

/******************************************************************************
 * Following is the template used to generate the semver_str_is_valid function.
 *
 * Should you decide to re-generate semver_str_is_valid using modified version
 * of this template, please be sure to update the following code.
 *
 * NOTE: Be sure to regenerate the template with re2c's -i flag, in order to
 *       avoid usage of the #line directive in generated C.
 ******************************************************************************/
#if 0
static int semver_str_validator(const char *semver_str,
                                uint16_t semver_str_len,
                                bool *po_has_primary,
                                bool *po_has_pre_release,
                                bool *po_has_bmd)
{
    bool has_primary = false;
    bool has_pre_release = false;
    bool has_bmd = false;
    
    const char *p = semver_str;
    const char *q = 0;
    
    if(NULL == semver_str)
    {
        return -1;
    }
    
#define YYCTYPE     char
#define YYCURSOR    p
#define YYLIMIT     (semver_str+semver_str_len)
#define YYMARKER    q
#define YYFILL(n)
    
    for(;;)
    {
        /*!re2c
         re2c:indent:top = 2;
         PRIMARY         = [0-9]+'.'[0-9]+'.'[0-9]+ ;
         PRE_RELEASE     = '-'([0-9A-Za-z-]+(('.'[0-9A-Za-z-]+)+)*) ;
         BUILD_META_DATA = '+'([0-9A-Za-z-]+(('.'[0-9A-Za-z-]+)+)*) ;
         
         PRIMARY"\000"
         {
         has_primary = true;
         break;
         }
         
         (PRIMARY)(PRE_RELEASE)"\000"
         {
         has_primary = true;
         has_pre_release = true;
         break;
         }
         
         (PRIMARY)(BUILD_META_DATA)"\000"
         {
         has_primary = true;
         has_bmd = true;
         break;
         }
         
         (PRIMARY)(PRE_RELEASE)(BUILD_META_DATA)"\000"
         {
         has_primary = true;
         has_pre_release = true;
         has_bmd = true;
         break;
         }
         
         [^]
         {
         return 1;
         break;
         }
         */
    }
    
    if(NULL != po_has_primary)
        *po_has_primary = has_primary;
    if(NULL != po_has_pre_release)
        *po_has_pre_release = has_pre_release;
    if(NULL != po_has_bmd)
        *po_has_bmd = has_bmd;
    
    return 0;
}

#endif
