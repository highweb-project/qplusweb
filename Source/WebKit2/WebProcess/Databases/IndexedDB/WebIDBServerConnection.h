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
 * THIS SOFTWARE IS PROVIDED BY APPLE, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef WebIDBServerConnection_h
#define WebIDBServerConnection_h

#if ENABLE(INDEXED_DATABASE) && ENABLE(DATABASE_PROCESS)

#include "MessageSender.h"
#include <WebCore/IDBDatabaseMetadata.h>
#include <WebCore/IDBServerConnection.h>

namespace WebKit {

class AsyncRequest;

class WebIDBServerConnection FINAL : public WebCore::IDBServerConnection, public IPC::MessageSender {
public:
    static PassRefPtr<WebIDBServerConnection> create(const String& databaseName, const WebCore::SecurityOrigin& openingOrigin, const WebCore::SecurityOrigin& mainFrameOrigin);

    virtual ~WebIDBServerConnection();

    virtual bool isClosed() OVERRIDE;

    typedef std::function<void (bool success)> BoolCallbackFunction;

    // Factory-level operations
    virtual void deleteDatabase(const String& name, BoolCallbackFunction successCallback) OVERRIDE;

    // Database-level operations
    virtual void getOrEstablishIDBDatabaseMetadata(GetIDBDatabaseMetadataFunction) OVERRIDE;
    virtual void close() OVERRIDE;

    // Transaction-level operations
    virtual void openTransaction(int64_t transactionID, const HashSet<int64_t>& objectStoreIds, WebCore::IndexedDB::TransactionMode, BoolCallbackFunction successCallback) OVERRIDE;
    virtual void beginTransaction(int64_t transactionID, std::function<void()> completionCallback) OVERRIDE;
    virtual void commitTransaction(int64_t transactionID, BoolCallbackFunction successCallback) OVERRIDE;
    virtual void resetTransaction(int64_t transactionID, std::function<void()> completionCallback) OVERRIDE;
    virtual void rollbackTransaction(int64_t transactionID, std::function<void()> completionCallback) OVERRIDE;

    virtual void setIndexKeys(int64_t transactionID, int64_t databaseID, int64_t objectStoreID, const WebCore::IDBObjectStoreMetadata&, WebCore::IDBKey& primaryKey, const Vector<int64_t>& indexIDs, const Vector<Vector<RefPtr<WebCore::IDBKey>>>& indexKeys, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;

    virtual void createObjectStore(WebCore::IDBTransactionBackend&, const WebCore::CreateObjectStoreOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void createIndex(WebCore::IDBTransactionBackend&, const WebCore::CreateIndexOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void deleteIndex(WebCore::IDBTransactionBackend&, const WebCore::DeleteIndexOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void get(WebCore::IDBTransactionBackend&, const WebCore::GetOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void put(WebCore::IDBTransactionBackend&, const WebCore::PutOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void openCursor(WebCore::IDBTransactionBackend&, const WebCore::OpenCursorOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void count(WebCore::IDBTransactionBackend&, const WebCore::CountOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void deleteRange(WebCore::IDBTransactionBackend&, const WebCore::DeleteRangeOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void clearObjectStore(WebCore::IDBTransactionBackend&, const WebCore::ClearObjectStoreOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void deleteObjectStore(WebCore::IDBTransactionBackend&, const WebCore::DeleteObjectStoreOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;
    virtual void changeDatabaseVersion(WebCore::IDBTransactionBackend&, const WebCore::IDBDatabaseBackend::VersionChangeOperation&, std::function<void(PassRefPtr<WebCore::IDBDatabaseError>)> completionCallback) OVERRIDE;

    // Cursor-level operations
    virtual void cursorAdvance(WebCore::IDBCursorBackend&, const WebCore::CursorAdvanceOperation&, std::function<void()> completionCallback) OVERRIDE;
    virtual void cursorIterate(WebCore::IDBCursorBackend&, const WebCore::CursorIterationOperation&, std::function<void()> completionCallback) OVERRIDE;
    virtual void cursorPrefetchIteration(WebCore::IDBCursorBackend&, const WebCore::CursorPrefetchIterationOperation&, std::function<void()> completionCallback) OVERRIDE;
    virtual void cursorPrefetchReset(WebCore::IDBCursorBackend&, int usedPrefetches) OVERRIDE;

    // Message handlers.
    void didReceiveWebIDBServerConnectionMessage(IPC::Connection*, IPC::MessageDecoder&);

    // IPC::MessageSender
    virtual uint64_t messageSenderDestinationID() OVERRIDE { return m_serverConnectionIdentifier; }

private:
    WebIDBServerConnection(const String& databaseName, const WebCore::SecurityOrigin& openingOrigin, const WebCore::SecurityOrigin& mainFrameOrigin);

    // IPC::MessageSender
    virtual IPC::Connection* messageSenderConnection() OVERRIDE;

    void didGetOrEstablishIDBDatabaseMetadata(uint64_t requestID, bool success, const WebCore::IDBDatabaseMetadata&);
    void didOpenTransaction(uint64_t requestID, bool success);
    void didBeginTransaction(uint64_t requestID, bool success);
    void didCommitTransaction(uint64_t requestID, bool success);
    void didResetTransaction(uint64_t requestID, bool success);
    void didRollbackTransaction(uint64_t requestID, bool success);
    void didChangeDatabaseVersion(uint64_t requestID, bool success);

    uint64_t m_serverConnectionIdentifier;

    String m_databaseName;
    Ref<WebCore::SecurityOrigin> m_openingOrigin;
    Ref<WebCore::SecurityOrigin> m_mainFrameOrigin;

    HashMap<uint64_t, RefPtr<AsyncRequest>> m_serverRequests;
};

} // namespace WebKit

#endif // ENABLE(INDEXED_DATABASE) && ENABLE(DATABASE_PROCESS)
#endif // WebIDBServerConnection_h
