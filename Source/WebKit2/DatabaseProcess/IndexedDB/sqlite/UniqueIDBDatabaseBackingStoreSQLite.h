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

#ifndef UniqueIDBDatabaseBackingStoreSQLite_h
#define UniqueIDBDatabaseBackingStoreSQLite_h

#if ENABLE(INDEXED_DATABASE) && ENABLE(DATABASE_PROCESS)

#include "UniqueIDBDatabase.h" 
#include "UniqueIDBDatabaseBackingStore.h"
#include <wtf/OwnPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/text/WTFString.h>

namespace WebCore {
class SQLiteDatabase;
struct IDBDatabaseMetadata;
}

namespace WebKit {

class SQLiteIDBTransaction;

class UniqueIDBDatabaseBackingStoreSQLite FINAL : public UniqueIDBDatabaseBackingStore {
public:
    static PassRefPtr<UniqueIDBDatabaseBackingStore> create(const UniqueIDBDatabaseIdentifier& identifier, const String& databaseDirectory)
    {
        return adoptRef(new UniqueIDBDatabaseBackingStoreSQLite(identifier, databaseDirectory));
    }

    virtual ~UniqueIDBDatabaseBackingStoreSQLite();

    virtual std::unique_ptr<WebCore::IDBDatabaseMetadata> getOrEstablishMetadata() OVERRIDE;

    virtual bool establishTransaction(const IDBTransactionIdentifier&, const Vector<int64_t>& objectStoreIDs, WebCore::IndexedDB::TransactionMode) OVERRIDE;
    virtual bool beginTransaction(const IDBTransactionIdentifier&) OVERRIDE;
    virtual bool commitTransaction(const IDBTransactionIdentifier&) OVERRIDE;
    virtual bool resetTransaction(const IDBTransactionIdentifier&) OVERRIDE;
    virtual bool rollbackTransaction(const IDBTransactionIdentifier&) OVERRIDE;

    virtual bool changeDatabaseVersion(const IDBTransactionIdentifier&, uint64_t newVersion) OVERRIDE;

private:
    UniqueIDBDatabaseBackingStoreSQLite(const UniqueIDBDatabaseIdentifier&, const String& databaseDirectory);

    std::unique_ptr<WebCore::SQLiteDatabase> openSQLiteDatabaseAtPath(const String&);
    std::unique_ptr<WebCore::IDBDatabaseMetadata> extractExistingMetadata();
    std::unique_ptr<WebCore::IDBDatabaseMetadata> createAndPopulateInitialMetadata();

    UniqueIDBDatabaseIdentifier m_identifier;
    String m_absoluteDatabaseDirectory;

    std::unique_ptr<WebCore::SQLiteDatabase> m_sqliteDB;

    HashMap<IDBTransactionIdentifier, std::unique_ptr<SQLiteIDBTransaction>> m_transactions;
};

} // namespace WebKit

#endif // ENABLE(INDEXED_DATABASE) && ENABLE(DATABASE_PROCESS)
#endif // UniqueIDBDatabaseBackingStoreSQLite_h
