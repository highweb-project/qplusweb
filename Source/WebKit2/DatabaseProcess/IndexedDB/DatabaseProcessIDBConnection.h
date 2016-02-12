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

#ifndef DatabaseProcessIDBConnection_h
#define DatabaseProcessIDBConnection_h

#include "MessageSender.h"

#if ENABLE(INDEXED_DATABASE) && ENABLE(DATABASE_PROCESS)

#include "SecurityOriginData.h"
#include "UniqueIDBDatabaseIdentifier.h"
#include <wtf/text/WTFString.h>

namespace WebKit {

class DatabaseToWebProcessConnection;
class UniqueIDBDatabase;

class DatabaseProcessIDBConnection : public RefCounted<DatabaseProcessIDBConnection>, public IPC::MessageSender {
public:
    static RefPtr<DatabaseProcessIDBConnection> create(DatabaseToWebProcessConnection& connection, uint64_t serverConnectionIdentifier)
    {
        return adoptRef(new DatabaseProcessIDBConnection(connection, serverConnectionIdentifier));
    }

    virtual ~DatabaseProcessIDBConnection();

    // Message handlers.
    void didReceiveDatabaseProcessIDBConnectionMessage(IPC::Connection*, IPC::MessageDecoder&);

    void disconnectedFromWebProcess();

private:
    DatabaseProcessIDBConnection(DatabaseToWebProcessConnection&, uint64_t idbConnectionIdentifier);

    // IPC::MessageSender
    virtual IPC::Connection* messageSenderConnection() OVERRIDE;
    virtual uint64_t messageSenderDestinationID() OVERRIDE { return m_serverConnectionIdentifier; }

    // Message handlers.
    void establishConnection(const String& databaseName, const SecurityOriginData& openingOrigin, const SecurityOriginData& mainFrameOrigin);
    void getOrEstablishIDBDatabaseMetadata(uint64_t requestID);
    void openTransaction(uint64_t requestID, int64_t transactionID, const Vector<int64_t>& objectStoreIDs, uint64_t transactionMode);
    void beginTransaction(uint64_t requestID, int64_t transactionID);
    void commitTransaction(uint64_t requestID, int64_t transactionID);
    void resetTransaction(uint64_t requestID, int64_t transactionID);
    void rollbackTransaction(uint64_t requestID, int64_t transactionID);
    void changeDatabaseVersion(uint64_t requestID, int64_t transactionID, uint64_t newVersion);


    Ref<DatabaseToWebProcessConnection> m_connection;
    uint64_t m_serverConnectionIdentifier;

    RefPtr<UniqueIDBDatabase> m_uniqueIDBDatabase;
};

} // namespace WebKit

#endif // ENABLE(INDEXED_DATABASE) && ENABLE(DATABASE_PROCESS)
#endif // DatabaseProcessIDBConnection_h
