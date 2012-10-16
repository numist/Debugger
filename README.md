Debugger
========

Originally written for [TagLib](https://github.com/numist/TagLib), the Debugger module provides debugging macros and functions useful to any project using the Obj-C runtime.

Wherever possible, arguments to macros are evaluated only once. If you find this is not the case, please file a ticket.

Installation
------------

`debugger.h` is written to be included in your project's precompiled header, or any other place it is wanted. If using in a precompiled header, it must be bracketed inside an `#ifdef __OBJC__` conditional, like so:

    #ifdef __OBJC__
        // …
        #include "debugger.h"
    #endif

<em>Unfortunately, the same `#ifdef __OBJC__` macro already present in `debugger.h` doesn't have the same effect in Xcode (tested in 4.5+) (why?).</em>

`debugger.c` will need to be built and linked as appropriate.

Usage
-----

Debugger provides the following utilities:

* [`AmIBeingDebugged`](#__github__readme_md__AmIBeingDebugged)
* [`DebugBreak`](#__github__readme_md__DebugBreak)
* [`Check`](#__github__readme_md__Check)
* [`NotTested`](#__github__readme_md__NotTested)
* [`Log`](#__github__readme_md__Log)
* [`Assert`](#__github__readme_md__Assert)
* [`NotReached`](#__github__readme_md__NotReached)
* [`BailUnless`](#__github__readme_md__BailUnless)
* [`BailWithBlockUnless`](#__github__readme_md__BailWithBlockUnless)
* [`BailWithGotoUnless`](#__github__readme_md__BailWithGotoUnless)

<a name="__github__readme_md__AmIBeingDebugged" />
---
### `bool AmIBeingDebugged()`
Returns whether a debugger is attached.

Used by [`DebugBreak`](#__github__readme_md__DebugBreak) to determine if it's safe to interrupt execution. If `DEBUG` is not defined, this is never called internally, but is still available.

<a name="__github__readme_md__DebugBreak" />
---
### `DebugBreak()`
Breaks into the debugger, if attached.

If there is no debugger attached, does nothing. If `DEBUG` is not defined, does nothing.

<a name="__github__readme_md__Check" />
---
### `Check(exp)`
[`Log`](#__github__readme_md__Log)s an error message and invokes `DebugBreak` if condition `exp` evaluates to `false`.

If `DEBUG` is not defined, does nothing.

<a name="__github__readme_md__NotTested" />
---
### `NotTested()`
[`Log`](#__github__readme_md__Log)s a "NOT TESTED" message and invokes `DebugBreak`.

If `DEBUG` is not defined, does nothing.

<a name="__github__readme_md__Log" />
---
### `Log(…)`
Emits a message via [`NSLog`](https://developer.apple.com/library/mac/documentation/cocoa/reference/foundation/miscellaneous/foundation_functions/reference/reference.html#//apple_ref/c/func/NSLog).

Prepends the file path, line number, and current function to the format string. Parameters are passed directly to [`NSString +stringWithFormat:`](https://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/Classes/NSString_Class/Reference/NSString.html#//apple_ref/occ/clm/NSString/stringWithFormat:). If `DEBUG` is not defined, does nothing.

##### Example

    Log(@"View hierarchy: %@", [[UIWindow keyWindow] recursiveDescription]);

<a name="__github__readme_md__Assert" />
---
### `Assert(exp)`
[`Log`](#__github__readme_md__Log)s an error message, invokes `DebugBreak`, and then `abort`s.

<em>If `DEBUG` is not defined, does nothing</em>.

##### Example

    Assert(NO); // ifdef DEBUG, kaboom.

<a name="__github__readme_md__NotReached" />
---
### `NotReached()`
[`Log`](#__github__readme_md__Log)s a "NOT REACHED" message, invokes `DebugBreak`, and then `abort`s.

<em>If `DEBUG` is not defined, does nothing</em>.

<a name="__github__readme_md__BailUnless" />
---
### `BailUnless(exp,return_value)`
[`Check`](#__github__readme_md__Check)s the expression, and then returns `return_value` if the check failed.

If `DEBUG` is not defined, this macro still bails, but does not break into the debugger or log any messages.

##### Example

    - (NSArray *)foo {
        BailUnless(NO, (NSArray *)nil);
        NotReached();
    }

When using `BailUnless` with a function or method returning `void`:

    - (void)foo {
        BailUnless(NO,);
        NotReached();
    }

<a name="__github__readme_md__BailWithBlockUnless" />
---
### `BailWithBlockUnless(exp,block)`
[`Check`](#__github__readme_md__Check)s the expression, and then returns the result returned by `block()` if the check failed.

If `DEBUG` is not defined, this macro still bails, but does not break into the debugger or log any messages.

##### Example

    - (NSArray *)foo {
        BailWithBlockUnless(NO, ^{
            // Clean up…
            return (NSArray *)nil;
        });
        NotReached();
    }

When using `BailWithBlockUnless` with a function or method returning `void`:

    - (void)foo {
        BailWithBlockUnless(NO, ^{
            // Clean up…
        });
        NotReached();
    }

<a name="__github__readme_md__BailWithGotoUnless" />
---
### `BailWithGotoUnless(exp,label)`
[`Check`](#__github__readme_md__Check)s the expression, and then jumps to `label` if the check failed.

If `DEBUG` is not defined, this macro still bails, but does not break into the debugger or log any messages.

##### Example

    - (NSArray *)foo {
        BailWithGotoUnless(NO, error);
        NotReached();
    error:
      // Clean up…
      return nil;
    }

---

License/Credits
===============

The most interesting code in this project is not originally mine, but was assembled, tested, and in some cases rewritten by me from public sources (Stack Overflow, public documentation, freely released snippets). Thus all code in this project is released to the public domain. While more complete attribution is provided in the source, credit is especially due for:

* `bool AmIBeingDebugged()` provided by an [Apple TechNote](http://developer.apple.com/library/mac/#qa/qa1361/_index.html).
* `DebugBreak()` for iOS provided by m20.nl (now defunct)
* `DebugBreak()` for OS X provided by [Matt Gallagher](http://cocoawithlove.com/2008/03/break-into-debugger.html)
