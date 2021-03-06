/***********************************************************************
 *                                                                      *
 *               This software is part of the ast package               *
 *          Copyright (c) 1985-2012 AT&T Intellectual Property          *
 *                      and is licensed under the                       *
 *                 Eclipse Public License, Version 1.0                  *
 *                    by AT&T Intellectual Property                     *
 *                                                                      *
 *                A copy of the License is available at                 *
 *          http://www.eclipse.org/org/documents/epl-v10.html           *
 *         (with md5 checksum b35adb5213ca9657e911e9befb180842)         *
 *                                                                      *
 *              Information and Software Systems Research               *
 *                            AT&T Research                             *
 *                           Florham Park NJ                            *
 *                                                                      *
 *               Glenn Fowler <glenn.s.fowler@gmail.com>                *
 *                    David Korn <dgkorn@gmail.com>                     *
 *                     Phong Vo <phongvo@gmail.com>                     *
 *                                                                      *
 ***********************************************************************/
/*
 * posix regex executor
 * single unsized-string interface
 */
#include "config_ast.h"  // IWYU pragma: keep

#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "ast_api.h"
#include "reglib.h"

/*
 * standard wrapper for the sized-record interface
 */

int regexec_20120528(const regex_t *p, const char *s, size_t nmatch, regmatch_t *match,
                     regflags_t flags) {
    if (flags & REG_STARTEND) {
        int r;
        int m = match->rm_so;
        regmatch_t *e;

        if (!(r = regnexec(p, s + m, match->rm_eo - m, nmatch, match, flags)) && m > 0)
            for (e = match + nmatch; match < e; match++)
                if (match->rm_so >= 0) {
                    match->rm_so += m;
                    match->rm_eo += m;
                }
        return r;
    }
    return regnexec(p, s, s ? strlen(s) : 0, nmatch, match, flags);
}

/*
 * 20120528: regoff_t changed from int to ssize_t
 */

#undef regexec
#if _map_libc
#define regexec _ast_regexec
#endif

int regexec(const regex_t *p, const char *s, size_t nmatch, oldregmatch_t *oldmatch,
            regflags_t flags) {
    if (oldmatch) {
        regmatch_t *match;
        size_t i;
        int r;

        match = malloc(nmatch * sizeof(regmatch_t));
        if (!match) return -1;
        r = regexec_20120528(p, s, nmatch, match, flags);
        if (!r) {
            for (i = 0; i < nmatch; i++) {
                oldmatch[i].rm_so = match[i].rm_so;
                oldmatch[i].rm_eo = match[i].rm_eo;
            }
        }
        free(match);
        return r;
    }
    return regexec_20120528(p, s, 0, NULL, flags);
}
