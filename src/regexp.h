#ifndef _regexp_h_
#define _regexp_h_

typedef struct regexp RegExp;	/* forward reference */

/*
 * create a regular expression matching engine
 */
const RegExp *RegExp_create(void);

/*
 * now define the dispatch table
 */
struct regexp {
    void *self;                  /* instance-specific state */

/*
 * compile() - compile pattern to be used by the matching engine
 *              returns 1 if successful, 0 if error
 */
    int (*compile)(const RegExp *re, char *pattern);

/*
 * status() - return the status of the last operation on the engine
 *             as a 0-byte terminated string
 */
    void (*status)(const RegExp *re, char *buf, int size);

/*
 * destroy() - return any Heap storage associated with the engine
 */
    void (*destroy)(const RegExp *re);

/*
 * match() - check whether the supplied buffer matches the pattern
 *            return 1 if successful, 0 otherwise
 */
    int (*match)(const RegExp *re, char *buf);
};

#endif /* _regexp_h_ */
