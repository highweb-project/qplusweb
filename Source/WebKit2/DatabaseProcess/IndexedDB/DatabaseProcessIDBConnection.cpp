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

#include "config.h"
#include "DatabaseProcessIDBConnection.h"

#if ENABLE(INDEXED_DATABASE) && ENABLE(DATABASE_PROCESS)

#include "DatabaseProcess.h"
#include "DatabaseToWebProcessConnection.h"
#include "IDBTransactionIdentifier.h"
#include "Logging.h"
#include "UniqueIDBDatabase.h"
#include "WebCoreArgumentCoders.h"
#include "WebIDBServerConnectionMessages.h"
#include <WebCore/IDBDatabaseMetadata.h>
#include <WebCore/IndexedDB.h>

using namespace WebCore;

namespace WebKit {

DatabaseProcessIDBConnection::DatabaseProcessIDBConnection(DatabaseToWebProcessConnection& connection, uint64_t serverConnectionIdentifier)
    : m_connection(connection)
    , m_serverConnectionIdentifier(serverConnectionIdentifier)
{
}

DatabaseProcessIDBConnection::~DatabaseProcessIDBConnection()
{
    ASSERT(!m_uniqueIDBDatabase);
}

void DatabaseProcessIDBConnection::disconnectedFromWebProcess()
{
    m_uniqueIDBDatabase->unregisterConnection(*this);
    m_uniqueIDBDatabase.clear();
}

void DatabaseProcessIDBConnection::establishConnection(const String& databaseName, const SecurityOriginData& openingOrigin, const SecurityOriginData& mainFrameOrigin)
{
    m_uniqueIDBDatabase = DatabaseProcess::shared().getOrCreateUniqueIDBDatabase(UniqueIDBDatabaseIdentifier(databaseName, openingOrigin, mainFrameOrigin));
    m_uniqueIDBDatabase->registerConnection(*this);
}

void DatabaseProcessIDBConnection::getOrEstablishIDBDatabaseMetadata(uint64_t requestID)
{
    ASSERT(m_uniqueIDBDatabase);

    LOG(IDB, "DatabaseProcess getOrEstablishIDBDatabaseMetadata request ID %llu", requestID);

    RefPtr<DatabaseProcessIDBConnection> connection(this);
    m_uniqueIDBDatabase->getOrEstablishIDBDatabaseMetadata([connection, requestID](bool success, const IDBDatabaseMetadata& metadata) {
        connection->send(Messages::WebIDBServerConnection::DidGetOrEstablishIDBDatabaseMetadata(requestID, success, metadata));
    });
}

void DatabaseProcessIDBConnection::openTransaction(uint64_t requestID, int64_t transactionID, const Vector<int64_t>& objectStoreIDs, uint64_t intMode)
{
    ASSERT(m_uniqueIDBDatabase);

    LOG(IDB, "DatabaseProcess openTransaction request ID %llu", requestID);

    if (intMode > IndexedDB::TransactionModeMaximum) {
        send(Messages::WebIDBServerConnection::DidOpenTransaction(requestID, false));
        return;
    }

    IndexedDB::TransactionMode mode = static_cast<IndexedDB::TransactionMode>(intMode);
    RefPtr<DatabaseProcessIDBConnection> connection(this);
    m_uniqueIDBDatabase->openTransaction(IDBTransactionIdentifier(*this, transactionID), objectStoreIDs, mode, [connection, requestID](bool success) {
        connection->send(Messages::WebIDBServerConnection::DidOpenTransaction(requestID, success));
    });
}

void DatabaseProcessIDBConnection::beginTransaction(uint64_t requestID, int64_t transactionID)
{
    ASSERT(m_uniqueIDBDatabase);

    LOG(IDB, "DatabaseProcess beginTransaction request ID %llu", requestID);

    RefPtr<DatabaseProcessIDBConnection> connection(this);
    m_uniqueIDBDatabase->beginTransaction(IDBTransactionIdentifier(*this, transactionID), [connection, requestID](bool success) {
        connection->send(Messages::WebIDBServerConnection::DidBeginTransaction(requestID, success));
    });
}

void DatabaseProcessIDBConnection::commitTransaction(uint64_t requestID, int64_t transactionID)
{
    ASSERT(m_uniqueIDBDatabase);

    LOG(IDB, "DatabaseProcess commitTransaction request ID %llu", requestID);

    RefPtr<DatabaseProcessIDBConnection> connection(this);
    m_uniqueIDBDatabase->commitTransaction(IDBTransactionIdentifier(*this, transactionID), [connection, requestID](bool success) {
        connection->send(Messages::WebIDBServerConnection::DidCommitTransaction(requestID, success));
    });
}

void DatabaseProcessIDBConnection::resetTransaction(uint64_t requestID, int64_t transactionID)
{
    ASSERT(m_uniqueIDBDatabase);

    LOG(IDB, "DatabaseProcess resetTransaction request ID %llu", requestID);

    RefPtr<DatabaseProcessIDBConnection> connection(this);
    m_uniqueIDBDatabase->resetTransaction(IDBTransactionIdentifier(*this, transactionID), [connection, requestID](bool success) {
        connection->send(Messages::WebIDBServerConnection::DidResetTransaction(requestID, success));
    });
}

void DatabaseProcessIDBConnection::rollbackTransaction(uint64_t requestID, int64_t transactionID)
{
    ASSERT(m_uniqueIDBDatabase);

    LOG(IDB, "DatabaseProcess rollbackTransaction request ID %llu", requestID);

    RefPtr<DatabaseProcessIDBConnection> connection(this);
    m_uniqueIDBDatabase->rollbackTransaction(IDBTransactionIdentifier(*this, transactionID), [connection, requestID](bool success) {
        connection->send(Messages::WebIDBServerConnection::DidRollbackTransaction(requestID, success));
    });
}

void DatabaseProcessIDBConnection::changeDatabaseVersion(uint64_t requestID, int64_t transactionID, uint64_t newVersion)
{
    ASSERT(m_uniqueIDBDatabase);

    LOG(IDB, "DatabaseProcess changeDatabaseVersion request ID %llu, new version %llu", requestID, newVersion);

    RefPtr<DatabaseProcessIDBConnection> connection(this);
    m_uniqueIDBDatabase->changeDatabaseVersion(IDBTransactionIdentifier(*this, transactionID), newVersion, [connection, requestID](bool success) {
        connection->send(Messages::WebIDBServerConnection::DidChangeDatabaseVersion(requestID, success));
    });
}

IPC::Connection* DatabaseProcessIDBConnection::messageSenderConnection()
{
    return m_connection->connection();
}

} // namespace WebKit

#endif // ENABLE(INDEXED_DATABASE) && ENABLE(DATABASE_PROCESS)
