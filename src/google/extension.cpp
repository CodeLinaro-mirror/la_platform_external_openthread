// Copyright 2025 Google LLC

/*
 *  Copyright (c) 2025, The OpenThread Authors.
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

#include "openthread-core-config.h"

#include <stdbool.h>
#include <stdint.h>

#include "common/code_utils.hpp"
#include "core/instance/extension.hpp"
#include "core/instance/instance.hpp"
#include "core/net/ip6_address.hpp"
#include "core/net/ip6_headers.hpp"
#include "core/net/udp6.hpp"
#include "instance/instance.hpp"
#include "net/ip6_types.hpp"
#include "openthread/ip6.h"
#include "openthread/platform/toolchain.h"

using namespace ot;

RegisterLogModule("GoogleExt");

namespace google_thread {

using ot::Extension::ExtensionBase;

bool operator==(const Ip6::Address &aA, const otIp6Address &aB) { return memcmp(&aA, &aB, sizeof(aA)) == 0; }

class Extension : public ExtensionBase
{
public:
    explicit Extension(Instance &aInstance)
        : ExtensionBase(aInstance)
    {
    }

#if !OPENTHREAD_RADIO
    static bool Filter(void              *aContext,
                       Message           &aMessage,
                       const Ip6::Header &aHeader,
                       bool              &aForwardThread,
                       bool              &aForwardHost,
                       bool              &aReceive)
    {
        return static_cast<Extension *>(aContext)->Filter(aMessage, aHeader, aForwardThread, aForwardHost, aReceive);
    }

    bool Filter(Message &aMessage, const Ip6::Header &aHeader, bool &aForwardThread, bool &aForwardHost, bool &aReceive)
    {
        return (MdnsPacketFilter(aMessage, aHeader, aForwardThread, aForwardHost, aReceive));
    }

    bool MdnsPacketFilter(Message           &aMessage,
                          const Ip6::Header &aHeader,
                          bool              &aForwardThread,
                          bool              &aForwardHost,
                          bool              &aReceive)
    {
        static constexpr uint16_t     kMdnsUdpPort   = 5353;
        static constexpr otIp6Address kMdnsMulticast = {{{0xff, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xfb}}};

        VerifyOrExit(aHeader.GetNextHeader() == Ip6::kProtoUdp && aHeader.GetDestination() == kMdnsMulticast);

        {
            Ip6::Udp::Header udpHeader;

            SuccessOrExit(aMessage.Read(sizeof(aHeader), udpHeader));
            VerifyOrExit(udpHeader.GetDestinationPort() == kMdnsUdpPort);
        }

        aForwardThread = false;
        aForwardHost   = false;
        aReceive       = false;

        return true;

    exit:
        return false;
    }
#endif
};

} // namespace google_thread

namespace ot {
namespace Extension {

using namespace google_thread;

ExtensionBase &ExtensionBase::Init(Instance &aInstance)
{
    static google_thread::Extension ext(aInstance);

    return ext;
}

void ExtensionBase::SignalInstanceInit(void)
{
#if !OPENTHREAD_RADIO
    // OpenThread instance is initialized and ready.
    Get<Ip6::Ip6>().SetPacketFilter(static_cast<google_thread::Extension *>(this), &google_thread::Extension::Filter);
#endif
}

void ExtensionBase::SignalNcpInit(Ncp::NcpBase &aNcpInstance) { OT_UNUSED_VARIABLE(aNcpInstance); }

void ExtensionBase::HandleNotifierEvents(Events aEvents) { OT_UNUSED_VARIABLE(aEvents); }

} // namespace Extension

} // namespace ot

