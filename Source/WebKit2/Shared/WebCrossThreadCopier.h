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

#ifndef WebCrossThreadCopier_h
#define WebCrossThreadCopier_h

#include <WebCore/CrossThreadCopier.h>
#include <wtf/Vector.h>

#if ENABLE(INDEXED_DATABASE)

namespace WebKit {
class IDBTransactionIdentifier;
class UniqueIDBDatabaseIdentifier;
}

namespace WebCore {

template<> struct CrossThreadCopierBase<false, false, WebKit::UniqueIDBDatabaseIdentifier> {
    static WebKit::UniqueIDBDatabaseIdentifier copy(const WebKit::UniqueIDBDatabaseIdentifier&);
};

template<> struct CrossThreadCopierBase<false, false, WebKit::IDBTransactionIdentifier> {
    static WebKit::IDBTransactionIdentifier copy(const WebKit::IDBTransactionIdentifier&);
};

template<> struct CrossThreadCopierBase<false, false, Vector<int64_t>> {
    static Vector<int64_t> copy(const Vector<int64_t>&);
};

} // namespace WebCore

#endif // ENABLE(INDEXED_DATABASE)
#endif // WebCrossThreadCopier_h
