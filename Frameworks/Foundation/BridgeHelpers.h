//******************************************************************************
//
// Copyright (c) 2016 Microsoft Corporation. All rights reserved.
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
#include "Starboard.h"
#include "NSRaise.h"

// Helper macro for toll-free bridged classes - all must override the following set of functions
#define BRIDGED_CLASS_REQUIRED_DECLS                                  \
    +(void)load;                                                      \
    -(_Nonnull instancetype)retain;                                   \
    -(oneway void)release;                                            \
    -(_Nonnull instancetype)autorelease;                              \
    -(NSUInteger)retainCount;                                         \
    -(void)dealloc;                                                   \
    +(_Nonnull instancetype)allocWithZone : (NSZone * _Nullable)zone; \
    -(_Nonnull Class)classForCoder;

// Helper macro for toll-free bridged classes - all must override the following set of functions
// Provide our own implementations of retain and release so that the bridging works out.

// CFBridgedTypeRef:        Name of CF version of a bridged class,                              ie:  CFArrayRef
// CFTypeIDFunc:            Name of function that returns the CFTypeID of CFBridgedTypeRef,     ie:  CFArrayGetTypeID
// NSBridgedType:           Name of NS version of a bridged class,                              ie:  NSArray
// NSBridgedConcreteType:   Name of concrete version of NSBridgedType,                          ie:  NSArrayConcrete
// clang-format off
#define BRIDGED_CLASS_REQUIRED_IMPLS(CFBridgedTypeRef, CFTypeIDFunc, NSBridgedType, NSBridgedConcreteType) \
 \
+ (void)load { \
    /* self here is referring to the Class object since its a + method. */ \
    _CFRuntimeBridgeTypeToClass(CFTypeIDFunc(), self); \
} \
 \
- (id)retain { \
    CFRetain(static_cast<CFBridgedTypeRef>(self)); \
    return self; \
} \
 \
- (oneway void)release { \
    CFRelease(static_cast<CFBridgedTypeRef>(self)); \
} \
 \
- (id)autorelease { \
    return (id)(CFAutorelease(static_cast<CFBridgedTypeRef>(self))); \
} \
 \
- (NSUInteger)retainCount { \
    return CFGetRetainCount(static_cast<CFBridgedTypeRef>(self)); \
} \
 \
__pragma(clang diagnostic push) \
__pragma(clang diagnostic ignored "-Wobjc-missing-super-calls") \
- (void)dealloc { \
    /* No-op for bridged classes. This is because the CF system is responsible for the allocation and dealloc of the backing memory. */ \
    /* This is all handled via the CFRelease calls. */ \
    /* When its CF ref count drops to 0 the CF version of dealloc is invoked */ \
    /* so by the time the NSObject dealloc is called, there is nothing left to do. */ \
} \
 \
__pragma(clang diagnostic pop) \
+ (NSBridgedConcreteType*)allocWithZone:(NSZone*)zone { \
    FAIL_FAST(); \
    return nullptr; \
} \
 \
-(Class)classForCoder { \
  return [NSBridgedType class];\
}
// clang-format on

// Helper to determine if a concrete class should be used.
// In order to determine if a concrete class should be used, the self pointer *must* be
// one of the classes along the abstract class inheritance from derived back to base.
// Consider the following:
//     NSObject
//         |
//     NSArray ----- NSDerivedArray
//         |
//     NSMutableArray ---- NSDerivedMutableArray
//         |
//     NSArrayConcrete
//
// In the above example *only* when self is NSArray or NSMutableArray should a concrete class be substituted in.
static inline bool shouldUseConcreteClass(Class self, Class base, Class derived) {
    // Note that all inheritance walking here is probably very quick as not many levels should exist (2 usually)
    do {
        if (self == derived) {
            return true;
        }

        derived = [derived superclass];

    } while (derived != [base superclass]);

    return false;
}

// Helper macro to stamp out calling through to inner class
#define INNER_BRIDGE_CALL(InnerObject, ReturnValue, ...) \
    (ReturnValue) __VA_ARGS__ {                          \
        return [InnerObject __VA_ARGS__];                \
    }

// Helper macro for implementing allocWithZone
#define ALLOC_CONCRETE_SUBCLASS_WITH_ZONE(NSBridgedType, NSBridgedConcreteType) \
    (NSObject*) allocWithZone : (NSZone*)zone {                                 \
        if (self == [NSBridgedType class]) {                                    \
            return [NSBridgedConcreteType allocWithZone:zone];                  \
        }                                                                       \
                                                                                \
        return [super allocWithZone:zone];                                      \
    }

// Helper macro for implementing allocWithZone
#define ALLOC_PLACEHOLDER_SUBCLASS_WITH_ZONE(NSBridgedType, NSBridgedPlaceholderType)                              \
    (NSObject*) allocWithZone : (NSZone*)zone {                                                                    \
        if (self == [NSBridgedType class]) {                                                                       \
            static StrongId<NSBridgedPlaceholderType> placeholder = [NSBridgedPlaceholderType allocWithZone:zone]; \
            return placeholder;                                                                                    \
        }                                                                                                          \
                                                                                                                   \
        return [super allocWithZone:zone];                                                                         \
    }