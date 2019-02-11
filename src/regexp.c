#include "regexp.h"
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

#define ALLOCATED 1
#define COMPILED 2
#define NPATTERNS 50

typedef struct {
    int re_sindex;
    int re_eindex;
} RegExpMatch;

typedef struct re_data {
    regex_t preg;
    int status;
    int compSts;
    int execSts;
    int nMatch;
    RegExpMatch pMatch[NPATTERNS];
} REData;

/*
 * re_compile - compile pattern to be used by the matching engine
 *              returns 1 if successful, 0 if error
 */
static int re_compile(const RegExp *re, char *pattern) {
    REData *red = (REData *)re->self;

    red->execSts = 0;
    if (red->status == COMPILED) {
        regfree(&(red->preg));
        red->status = ALLOCATED;
    }
    red->compSts = regcomp(&(red->preg), pattern, REG_EXTENDED|REG_NEWLINE);
    if (red->compSts == 0) {
        red->status = COMPILED;
    }
    return (red->compSts == 0);
}

/*
 * re_status - return the status of the last operation on the engine
 *             as a 0-byte terminated string
 */
static void re_status(const RegExp *re, char *buf, int size) {
    REData *red = (REData *)re->self;

    if (red->compSts)
        (void) regerror(red->compSts, &(red->preg), buf, size);
    else if (red->execSts && red->execSts != REG_NOMATCH)
        (void) regerror(red->execSts, &(red->preg), buf, size);
    else
        buf[0] = '\0';
}

/*
 * re_destroy - return any Heap storage associated with the engine
 */
static void re_destroy(const RegExp *re) {
    REData *red = (REData *)re->self;

    if (red->status == COMPILED)
        regfree(&(red->preg));
    free(red);
    free((void *)re);
}

/*
 * re_match - check whether the supplied buffer matches the pattern
 *            return 1 if successful, 0 otherwise
 */
static int re_match(const RegExp *re, char *buf) {
    REData *red = (REData *)re->self;
    size_t nmatch = NPATTERNS;
    regmatch_t pmatch[NPATTERNS];

    if (red->status == COMPILED) {
        int n, i;
        red->execSts = regexec(&(red->preg), buf, nmatch, pmatch, 0);
        if (red->execSts != 0)
            return 0;
        for (n = 0; n < NPATTERNS; n++)
            if (pmatch[n].rm_so == -1)
                break;
        red->nMatch = n;
        for (i = 0; i < n; i++) {
            red->pMatch[i].re_sindex = pmatch[i].rm_so;
            red->pMatch[i].re_eindex = pmatch[i].rm_eo;
        }
        return 1;
    }
    return 0;
}

static RegExp template = {
    NULL, re_compile, re_status, re_destroy, re_match
};

/*
 * RegExp_create - create regular expression matching engine
 *             returns NULL if there is an error
 */
const RegExp *RegExp_create(void) {
    RegExp *re = (RegExp *)malloc(sizeof(RegExp));

    if (re != NULL) {
        REData *red = (REData *)malloc(sizeof(REData));

        if (red != NULL) {
            red->status = ALLOCATED;
            red->compSts = 0;
            red->nMatch = 0;
            *re = template;
            re->self = red;
        } else {
            free(re);
            re = NULL;
        }
    }
    return re;
}
