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

#ifdef SDL_AUDIO_DRIVER_WIIU

// WIIU Audio driver

#include "../SDL_sysaudio.h"
#include "SDL_wiiuaudio.h"

#define WIIUAUDIO_DRIVER_NAME "wiiu"

static bool WIIUAUDIO_OpenDevice(SDL_AudioDevice *device)
{
    return true;
}

static bool WIIUAUDIO_PlayDevice(SDL_AudioDevice *device, const Uint8 *buffer, int buflen)
{
    return true;
}

static bool WIIUAUDIO_WaitDevice(SDL_AudioDevice *device)
{
    return true;
}

static Uint8 *WIIUAUDIO_GetDeviceBuf(SDL_AudioDevice *device, int *buffer_size)
{
    return device->hidden->mixbuf;
}

static void WIIUAUDIO_CloseDevice(SDL_AudioDevice *device)
{
}

static void WIIUAUDIO_ThreadInit(SDL_AudioDevice *device)
{
}

static bool WIIUAUDIO_Init(SDL_AudioDriverImpl *impl)
{
    AXInit()

    impl->OpenDevice = WIIUAUDIO_OpenDevice;
    impl->PlayDevice = WIIUAUDIO_PlayDevice;
    impl->WaitDevice = WIIUAUDIO_WaitDevice;
    impl->GetDeviceBuf = WIIUAUDIO_GetDeviceBuf;
    impl->CloseDevice = WIIUAUDIO_CloseDevice;
    impl->ThreadInit = WIIUAUDIO_ThreadInit;
    impl->OnlyHasDefaultPlaybackDevice = true;

    // Should be possible, but micInit would fail
    impl->HasRecordingSupport = false;

    return true;
}

AudioBootStrap WIIUAUDIO_bootstrap = {
    WIIUAUDIO_DRIVER_NAME,
    "SDL WIIU audio driver",
    WIIUAUDIO_Init,
    false,
    false
};

#endif // SDL_AUDIO_DRIVER_WIIU
