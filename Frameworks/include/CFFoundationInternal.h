//******************************************************************************
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//******************************************************************************

#pragma once

#if !defined(CF_PRIVATE)
#define CF_PRIVATE __attribute__((__visibility__("hidden")))
#endif

#include <CoreFoundation/CoreFoundation.h>

CF_PRIVATE CF_EXPORT void _CFRuntimeBridgeTypeToClass(CFTypeID type, const void* isa);
CF_PRIVATE CF_EXPORT void _CFAppendPathComponent2(CFMutableStringRef path, CFStringRef component);
CF_PRIVATE CF_EXPORT Boolean _CFAppendPathExtension2(CFMutableStringRef path, CFStringRef extension);
CF_PRIVATE CF_EXPORT CFIndex _CFStartOfPathExtension2(CFStringRef path);
CF_PRIVATE CF_EXPORT CFIndex _CFStartOfLastPathComponent2(CFStringRef path);
CF_EXPORT UniChar _CFGetSlash();
CF_PRIVATE CF_EXPORT CFStringRef _CFGetSlashStr();

// Copied from CFFileUtilities.c
#if DEPLOYMENT_TARGET_WINDOWS
#define WINDOWS_PATH_SEMANTICS
#else
#define UNIX_PATH_SEMANTICS
#endif

#if defined(WINDOWS_PATH_SEMANTICS)
#define IS_SLASH(C) ((C) == '\\' || (C) == '/')
#elif defined(UNIX_PATH_SEMANTICS)
#define IS_SLASH(C) ((C) == '/')
#endif