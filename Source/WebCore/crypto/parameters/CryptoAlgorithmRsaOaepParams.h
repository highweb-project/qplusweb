/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CryptoAlgorithmRsaOaepParams_h
#define CryptoAlgorithmRsaOaepParams_h

#include "CryptoAlgorithmIdentifier.h"
#include "CryptoAlgorithmParameters.h"

#if ENABLE(SUBTLE_CRYPTO)

namespace WebCore {

class CryptoAlgorithmRsaOaepParams FINAL : public CryptoAlgorithmParameters {
public:
    CryptoAlgorithmRsaOaepParams()
        : hasLabel(false)
    {
    }

    // The hash function to apply to the message.
    CryptoAlgorithmIdentifier hash;

    // The optional label/application data to associate with the message.
    // FIXME: Is there a difference between a missing label and an empty one? Perhaps we don't need the hasLabel member.
    bool hasLabel;
    Vector<uint8_t> label;

    virtual Class parametersClass() const OVERRIDE { return Class::RsaOaepParams; }
};

CRYPTO_ALGORITHM_PARAMETERS_CASTS(RsaOaepParams)

}

#endif // ENABLE(SUBTLE_CRYPTO)
#endif // CryptoAlgorithmRsaOaepParams_h
