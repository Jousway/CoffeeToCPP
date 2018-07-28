/*
 * Copyright © 2013 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/** @file config.h
 *
 * Provides configuration options for Epoxy.
 */

#ifndef EFOXY_CONFIG_H
#define EFOXY_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* #undef ENABLE_EGL */

#ifdef _WIN32
#  include <windows.h>
//#  define DLL_EXPORT true
#  define inline __inline
#else
#  define ENABLE_GLX 1
#endif

// We are linking it as a static.
#ifndef EFOXY_PUBLIC
//# if defined(_MSC_VER)
//#  define EFOXY_PUBLIC __declspec(dllexport) extern 
# else
//#  define EFOXY_PUBLIC extern
//# endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* EFOXY_CONFIG_H */
