/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_MSG_LIKELY_H
#define CLARA_MSG_LIKELY_H

#if defined(__GNUC__)

#define CLARA_LIKELY(x) __builtin_expect((x), 1)
#define CLARA_UNLIKELY(x) __builtin_expect((x), 0)

#elif defined(__clang__)

#if __has_builtin(__builtin_expect)
#define CLARA_LIKELY(x) __builtin_expect((x), 1)
#define CLARA_UNLIKELY(x) __builtin_expect((x), 0)
#endif

#endif

#ifndef CLARA_LIKELY
#define CLARA_LIKELY(x) (x)
#endif
#ifndef CLARA_UNLIKELY
#define CLARA_UNLIKELY(x) (x)
#endif

#endif // CLARA_MSG_LIKELY_H
