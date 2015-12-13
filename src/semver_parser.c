
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static int semver_str_validator(const char *semver_str,
                                uint16_t semver_str_len,
                                bool *po_has_primary,
                                bool *po_has_pre_release,
                                bool *po_has_bmd)
{
    int result = 0;
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

int main(int argc, char ** argv)
{
    int result;
    bool has_primary;
    bool has_pre_release;
    bool has_bmd;
    
    if (argc > 1)
    {
        result = semver_str_validator(argv[1],
                                     strlen(argv[1]),
                                     &has_primary,
                                     &has_pre_release,
                                     &has_bmd);
        
        printf("Result: %d, has_primary: %d, has_pre_release: %d, has_bmd: %d\n",
               result,has_primary,has_pre_release,has_bmd);
    }
    else
    {
        fprintf(stderr, "%s <expr>\n", argv[0]);
        return 1;
    }
    
}