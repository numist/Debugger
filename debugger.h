//
//  debugger.h
//  Debugger
//
//  Created by Scott Perry on 8/11/11.
//  Public domain.
//

#ifdef __OBJC__

#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <stdbool.h>
bool AmIBeingDebugged(void);

/*
 * The TODO macro allows TODO items to appear as compiler warnings.
 * Always enabled—if you've got something you still need to do, do it before you ship!
 */
#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " x))

#pragma mark - DebugBreak implementations for all known platforms
// TODO: Should this be in a C function instead of a macro in order to force the debugger to break in the right place?

#ifdef DEBUG
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        /*
         * iOS DebugBreak and friends provided by http://iphone.m20.nl/wp/?p=1 (defunct)
         * As modified by http://secretspaceman.com/2011/11/assert-yourself/
         */
        #if defined(__arm__)
            #pragma mark iOS(arm)

            #define DebugBreak() \
                do { \
                    if(AmIBeingDebugged()) { \
                        __asm__ __volatile__ ( \
                            "mov r0, %0\n" \
                            "mov r1, %1\n" \
                            "mov r12, #37\n" \
                            "swi 128\n" \
                            : : "r" (getpid ()), "r" (SIGINT) : "r12", "r0", "r1", "cc" \
                        ); \
                    } \
                } while (false)

        #elif defined(__i386__) || defined(__x86_64__)
            #pragma mark iOS(x86)

            #define DebugBreak() \
                do { \
                    if(AmIBeingDebugged()) { \
                        __asm__ __volatile__ ( \
                            "pushl %0\n" \
                            "pushl %1\n" \
                            "push $0\n" \
                            "movl %2, %%eax\n" \
                            "int $0x80\n" \
                            "add $12, %%esp\n" \
                            : : "g" (SIGINT), "g" (getpid ()), "n" (37) : "eax", "cc"); \
                    } \
                } while (false)

        #else
            #pragma mark iOS(unknown)
            #warning Debugger: Current iOS architecture not supported, please report (Debugger integration disabled)
            #define DebugBreak()
        #endif
    #elif TARGET_OS_MAC
        // Breaking into the debugger on the Mac (if possible) provided by: http://cocoawithlove.com/2008/03/break-into-debugger.html
        #if defined(__ppc64__) || defined(__ppc__)
            #pragma mark desktop(ppc)

            #define DebugBreak() \
                if(AmIBeingDebugged()) \
                { \
                    __asm__("li r0, 20\nsc\nnop\nli r0, 37\nli r4, 2\nsc\nnop\n" \
                    : : : "memory","r0","r3","r4" ); \
                }

        #elif defined(__x86_64__) || defined(__i386__)
            #pragma mark desktop(x86)
            #define DebugBreak() if(AmIBeingDebugged()) {__asm__("int $3\n" : : );}
        #else
            #pragma mark desktop(unknown)
            #warning Debugger: Current desktop architecture not supported, please report (Debugger integration disabled)
            #define DebugBreak()
        #endif
    #else
        #pragma mark unknown()
        #warning Debugger: Current platform not supported, please report (Debugger integration disabled)
        #define DebugBreak()
    #endif

#pragma mark High(er) level debugging macros
    // The Check and NotTested functions emit a log message and will break a watching debugger if possible.
    #define Check(exp) do { \
                if (!(exp)) { \
                    Log(@"Failed check `%s`", #exp); \
                    DebugBreak(); \
                } \
            } while(0)
    #define NotTested() do { \
                Log(@"NOT TESTED"); \
                DebugBreak(); \
            } while(0)

    // The Log, Assert, and NotReached macros are much more mundane, serving to prevent the incidence of NSLog calls in Release builds, improve logging in Debug builds, and kill the program.
    #define Log(fmt, ...) NSLog(@"%s:%d <%s> %@", __FILE__, __LINE__, __PRETTY_FUNCTION__, [NSString stringWithFormat:(fmt), ##__VA_ARGS__]);

    // Assert is ALWAYS FATAL on DEBUG! If the error was recoverable, you should be using Check() or Bail…Unless()!
    #define Assert(exp) do { \
                if (!(exp)) { \
                    Log(@"Failed assertion `%s`", #exp); \
                    DebugBreak(); \
                    abort(); \
                } \
            } while(0)

    // NotReached is ALWAYS FATAL on DEBUG! If the code path is intentionally reachable, you should be using NotTested()!
    #define NotReached() do { \
                Log(@"Entered THE TWILIGHT ZONE"); \
                DebugBreak(); \
                abort(); \
            } while(0)

    // Macros that affect control flow on condition
    #define BailUnless(exp,return_value) do { \
                if (!(exp)) { \
                    Log(@"Failed check `%s`, bailing.", #exp); \
                    DebugBreak(); \
                    return return_value; \
                } \
            } while(0)
    #define BailWithBlockUnless(exp,block) do { \
                if (!(exp)) { \
                    Log(@"Failed check `%s`, bailing.", #exp); \
                    DebugBreak(); \
                    return block(); \
                } \
            } while(0)
    #define BailWithGotoUnless(exp,label) do { \
                if (!(exp)) { \
                    Log(@"Failed check `%s`, bailing.", #exp); \
                    DebugBreak(); \
                    goto label; \
                } \
            } while(0)
#else // DEBUG
#pragma mark - Debugging stubs
    #define DebugBreak()
    #define Log(...)
    #define Check(exp)
    #define NotTested()

    // TODO: Should Assert() and NotReached() be fatal on non-debug? Side effects are scary, but failing fast is more instructive to the developer when they get the crash report instead of having to unwind the mess that was created when the code keeps executing past the assertion.
    #define Assert(exp)
    #define NotReached()

    // Macros that affect control flow on condition
    #define BailUnless(exp,return_value) do { \
            if (!(exp)) { \
                return return_value; \
            } \
        } while(0)
    #define BailWithBlockUnless(exp,block) do { \
            if (!(exp)) { \
                return block(); \
            } \
        } while(0)
    #define BailWithGotoUnless(exp,label) do { \
            if (!(exp)) { \
                goto label; \
            } \
        } while(0)
#endif // DEBUG

#endif // _DEBUGGER_H_

#endif // __OBJC__
