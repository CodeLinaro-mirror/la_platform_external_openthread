/*
 *  Copyright (c) 2024, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <openthread-core-config.h>
#include <openthread/config.h>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include <openthread/platform/logging.h>

#include <log/log.h>

#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED)
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogRegion);

    va_list             args;
    android_LogPriority priority;

    switch (aLogLevel)
    {
    case OT_LOG_LEVEL_NONE:
        priority = ANDROID_LOG_SILENT;
        break;
    case OT_LOG_LEVEL_CRIT:
        priority = ANDROID_LOG_FATAL;
        break;
    case OT_LOG_LEVEL_WARN:
        priority = ANDROID_LOG_WARN;
        break;
    case OT_LOG_LEVEL_NOTE:
    case OT_LOG_LEVEL_INFO:
        priority = ANDROID_LOG_INFO;
        break;
    case OT_LOG_LEVEL_DEBG:
        priority = ANDROID_LOG_DEBUG;
        break;
    default:
        assert(false);
        priority = ANDROID_LOG_DEBUG;
        break;
    }

    va_start(args, aFormat);
    __android_log_vprint(priority, LOG_TAG, aFormat, args);
    va_end(args);
}

void platformLoggingInit(const char *aName)
{
    otPlatLog(OT_LOG_LEVEL_INFO, OT_LOG_REGION_PLATFORM, "OpenThread logs");
    otPlatLog(OT_LOG_LEVEL_INFO, OT_LOG_REGION_PLATFORM, "- Program:  %s", aName);
}
#else
void platformLoggingInit(const char *aName) { OT_UNUSED_VARIABLE(aName); }
#endif // (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED)

void platformLoggingSetFileName(const char *aName) { OT_UNUSED_VARIABLE(aName); }
void platformLoggingDeinit(void) {}
