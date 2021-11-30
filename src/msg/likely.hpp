/*
 * Copyright (C) 2019. Jefferson Lab, xMsg framework (JLAB). All Rights Reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, research, and not-for-profit purposes,
 * without fee and without a signed licensing agreement.
 *
 * Contact Vardan Gyurjyan
 * Department of Experimental Nuclear Physics, Jefferson Lab.
 *
 * IN NO EVENT SHALL JLAB BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
 * INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF
 * THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF JLAB HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * JLAB SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE CLARA SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 * HEREUNDER IS PROVIDED "AS IS". JLAB HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
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
