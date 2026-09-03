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

#include <coreinit/semaphore.h>

struct SDL_Semaphore
{
    OSSemaphore semaphore;
};

SDL_Semaphore *SDL_CreateSemaphore(Uint32 initial_value)
{
    SDL_Semaphore *sem;

    if (initial_value > SDL_MAX_SINT32) {
        SDL_SetError("Initial semaphore value too high for this platform");
        return NULL;
    }

    sem = (SDL_Semaphore *)SDL_malloc(sizeof(*sem));
    if (!sem) {
        return NULL;
    }

    OSInitSemaphore(&sem->semaphore, initial_value);

    return sem;
}

/* WARNING:
   You cannot call this function when another thread is using the semaphore.
*/
void SDL_DestroySemaphore(SDL_Semaphore *sem)
{
    SDL_free(sem);
}

static bool WaitOnSemaphoreFor(SDL_Semaphore *sem, Sint64 timeoutNS)
{
    Uint64 stop_time = SDL_GetTicksNS() + timeoutNS;
    while (SDL_GetTicksNS() < stop_time) {
        if (OSTryWaitSemaphore(&sem->semaphore, 1) == 0) {
            return true;
        }
        // FIXME: this is what 3DS is using, I've not tested for a better value.
        // 100 microseconds seems to be the sweet spot
        SDL_DelayNS(SDL_US_TO_NS(100));
    }

    // If we failed, yield to avoid starvation on busy waits
    SDL_DelayNS(1);
    return false;
}

bool SDL_WaitSemaphoreTimeoutNS(SDL_Semaphore *sem, Sint64 timeoutNS)
{
    if (!sem) {
        return true;
    }

    if (timeoutNS < 0) { // -1 == wait indefinitely.
        OSWaitSemaphore(&sem->semaphore);
        return true;
    }

    if (OSTryWaitSemaphore(&sem->semaphore) == 0) {
        return true;
    }

    return WaitOnSemaphoreFor(sem, timeoutNS);
}

Uint32 SDL_GetSemaphoreValue(SDL_Semaphore *sem)
{
    if (!sem) {
        return 0;
    }
    return OSGetSemaphoreCount(&sem->semaphore);
}

void SDL_SignalSemaphore(SDL_Semaphore *sem)
{
    if (!sem) {
        return;
    }

    OSSignalSemaphore(&sem->semaphore);
}

#endif // SDL_THREAD_WIIU
