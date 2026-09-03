/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

#ifdef SDL_THREAD_WIIU

// Thread management routines for SDL

#include "../SDL_systhread.h"

// 1MB stack size default.
#define WIIU_THREAD_STACK_SIZE_DEFAULT (1024 * 1024)

enum WiiUThreadPriority {
    WIIU_THREAD_PRIORITY_LOW = 31,           /**< Minimum priority */
    WIIU_THREAD_PRIORITY_MAIN_THREAD = 30,   /**< Main thread */
    WIIU_THREAD_PRIORITY_MEDIUM = 29,        /**< Slightly higher than main thread (30) */
    WIIU_THREAD_PRIORITY_HIGH = 5,           /**< High priority for non-video work */
    WIIU_THREAD_PRIORITY_TIME_CRITICAL = 0,  /**< Highest priority */
};

static size_t GetStackSize(size_t requested_size);

static int ThreadEntry(int argc, const char **argv)
{
    SDL_RunThread((SDL_Thread *)argv);
    return 0;
}

bool SDL_SYS_CreateThread(SDL_Thread *thread,
                          SDL_FunctionPointer pfnBeginThread,
                          SDL_FunctionPointer pfnEndThread)
{
    int32_t priority = WIIU_THREAD_PRIORITY_MAIN_THREAD;
    uint32_t stack_size = GetStackSize(thread->stacksize);

    void* stack_space = SDL_malloc(stack_size);

    BOOL result = OSCreateThread(&thread->handle,
                                  ThreadEntry,
                                  0,
                                  (char*)thread,
                                  stack_space,
                                  stack_size,
                                  priority,
                                  OS_THREAD_ATTRIB_AFFINITY_ANY
                                );

    if (!result) {
        return SDL_SetError("Couldn't create thread");
    }

    thread->threadid = (SDL_ThreadID) thread->handle.id;

    return true;
}

static size_t GetStackSize(size_t requested_size)
{
    if (requested_size == 0) {
        return WIIU_THREAD_STACK_SIZE_DEFAULT;
    }

    return requested_size;
}

void SDL_SYS_SetupThread(const char *name)
{
    return;
}

SDL_ThreadID SDL_GetCurrentThreadID(void)
{
    OSThread *thread = OSGetCurrentThread();
    return (SDL_ThreadID)thread->id;
}

bool SDL_SYS_SetThreadPriority(SDL_ThreadPriority sdl_priority)
{
    int32_t svc_priority;
    switch (sdl_priority) {
    case SDL_THREAD_PRIORITY_LOW:
        svc_priority = WIIU_THREAD_PRIORITY_LOW;
        break;
    case SDL_THREAD_PRIORITY_NORMAL:
        svc_priority = WIIU_THREAD_PRIORITY_MEDIUM;
        break;
    case SDL_THREAD_PRIORITY_HIGH:
        svc_priority = WIIU_THREAD_PRIORITY_HIGH;
        break;
    case SDL_THREAD_PRIORITY_TIME_CRITICAL:
        svc_priority = WIIU_THREAD_PRIORITY_TIME_CRITICAL;
        break;
    default:
        svc_priority = WIIU_THREAD_PRIORITY_MEDIUM;
    }
    if (OSSetThreadPriority(OSGetCurrentThread(), svc_priority) < 0) {
        return SDL_SetError("OSSetThreadPriority failed");
    }
    return true;
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
    int returnCode = 0;
    BOOL res = OSJoinThread(&thread->handle, &returnCode);

    /*
      Detached threads can be waited on, but should NOT be cleaned manually
      as it would result in a fatal error.
    */
    //if (res && SDL_GetThreadState(thread) != SDL_THREAD_DETACHED) {
    //    threadFree(thread->handle);
    //}
}

void SDL_SYS_DetachThread(SDL_Thread *thread)
{
    OSDetachThread(&thread->handle);
}

#endif // SDL_THREAD_WIIU
