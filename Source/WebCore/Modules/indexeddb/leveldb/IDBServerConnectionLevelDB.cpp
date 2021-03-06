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
#include "IDBServerConnectionLevelDB.h"

#if ENABLE(INDEXED_DATABASE)
#if USE(LEVELDB)

#include "IDBBackingStoreCursorLevelDB.h"
#include "IDBBackingStoreLevelDB.h"
#include "IDBBackingStoreTransactionLevelDB.h"
#include "IDBCursorBackend.h"
#include "IDBFactoryBackendLevelDB.h"
#include "IDBIndexWriterLevelDB.h"
#include <wtf/MainThread.h>

#define ASYNC_COMPLETION_CALLBACK_WITH_ARG(callback, arg) \
    callOnMainThread([callback, arg]() { \
        callback(arg); \
    });

#define ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(callback) \
    callOnMainThread([callback]() { \
        callback(0); \
    });

#define EMPTY_ASYNC_COMPLETION_CALLBACK(callback) \
    callOnMainThread([callback]() { \
        callback(); \
    });


namespace WebCore {

IDBServerConnectionLevelDB::IDBServerConnectionLevelDB(const String& databaseName, IDBBackingStoreLevelDB* backingStore)
    : m_backingStore(backingStore)
    , m_nextCursorID(1)
    , m_closed(false)
    , m_databaseName(databaseName)
{
}

IDBServerConnectionLevelDB::~IDBServerConnectionLevelDB()
{
}

bool IDBServerConnectionLevelDB::isClosed()
{
    return m_closed;
}

void IDBServerConnectionLevelDB::getOrEstablishIDBDatabaseMetadata(GetIDBDatabaseMetadataFunction callback)
{
    RefPtr<IDBServerConnection> self(this);
    m_backingStore->getOrEstablishIDBDatabaseMetadata(m_databaseName, [self, this, callback](const IDBDatabaseMetadata& metadata, bool success) {
        callback(metadata, success);
    });
}

void IDBServerConnectionLevelDB::deleteDatabase(const String& name, BoolCallbackFunction successCallback)
{
    RefPtr<IDBServerConnection> self(this);
    m_backingStore->deleteDatabase(name, [self, this, successCallback](bool success) {
        successCallback(success);
    });
}

void IDBServerConnectionLevelDB::close()
{
    m_backingStore.clear();
    m_closed = true;
}

void IDBServerConnectionLevelDB::openTransaction(int64_t transactionID, const HashSet<int64_t>&, IndexedDB::TransactionMode, BoolCallbackFunction successCallback)
{
    if (!m_backingStore) {
        callOnMainThread([successCallback]() {
            successCallback(false);
        });
        return;
    }

    m_backingStore->establishBackingStoreTransaction(transactionID);
    callOnMainThread([successCallback]() {
        successCallback(true);
    });
}

void IDBServerConnectionLevelDB::beginTransaction(int64_t transactionID, std::function<void()> completionCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> transaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(transaction);

    transaction->begin();
    callOnMainThread(completionCallback);
}

void IDBServerConnectionLevelDB::commitTransaction(int64_t transactionID, BoolCallbackFunction successCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> transaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(transaction);

    bool result = transaction->commit();
    callOnMainThread([successCallback, result]() {
        successCallback(result);
    });
}

void IDBServerConnectionLevelDB::resetTransaction(int64_t transactionID, std::function<void()> completionCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> transaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(transaction);

    transaction->resetTransaction();
    callOnMainThread(completionCallback);
}

void IDBServerConnectionLevelDB::rollbackTransaction(int64_t transactionID, std::function<void()> completionCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> transaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(transaction);

    transaction->rollback();
    callOnMainThread(completionCallback);
}

void IDBServerConnectionLevelDB::setIndexKeys(int64_t transactionID, int64_t databaseID, int64_t objectStoreID, const IDBObjectStoreMetadata& objectStoreMetadata, IDBKey& primaryKey, const Vector<int64_t>& indexIDs, const Vector<Vector<RefPtr<IDBKey>>>& indexKeys, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    RefPtr<IDBBackingStoreTransactionLevelDB> backingStoreTransaction = m_backingStoreTransactions.get(transactionID);
    ASSERT(backingStoreTransaction);

    RefPtr<IDBRecordIdentifier> recordIdentifier;
    bool ok = m_backingStore->keyExistsInObjectStore(*backingStoreTransaction, databaseID, objectStoreID, primaryKey, recordIdentifier);
    if (!ok) {
        callOnMainThread([completionCallback]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error: setting index keys."));
        });
        return;
    }
    if (!recordIdentifier) {
        callOnMainThread([completionCallback]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error: setting index keys for object store."));
        });
        return;
    }

    Vector<RefPtr<IDBIndexWriterLevelDB>> indexWriters;
    String errorMessage;
    bool obeysConstraints = false;

    bool backingStoreSuccess = m_backingStore->makeIndexWriters(transactionID, databaseID, objectStoreMetadata, primaryKey, false, indexIDs, indexKeys, indexWriters, &errorMessage, obeysConstraints);
    if (!backingStoreSuccess) {
        callOnMainThread([completionCallback]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error: backing store error updating index keys."));
        });
        return;
    }
    if (!obeysConstraints) {
        callOnMainThread([completionCallback, errorMessage]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::ConstraintError, errorMessage));
        });
        return;
    }

    for (size_t i = 0; i < indexWriters.size(); ++i) {
        IDBIndexWriterLevelDB* indexWriter = indexWriters[i].get();
        indexWriter->writeIndexKeys(recordIdentifier.get(), *m_backingStore, *backingStoreTransaction, databaseID, objectStoreID);
    }

    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::createObjectStore(IDBTransactionBackend& transaction, const CreateObjectStoreOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    String objectStoreName = operation.objectStoreMetadata().name;

    if (!m_backingStore->createObjectStore(*backingStoreTransaction, transaction.database().id(), operation.objectStoreMetadata().id, objectStoreName, operation.objectStoreMetadata().keyPath, operation.objectStoreMetadata().autoIncrement)) {
        callOnMainThread([completionCallback, objectStoreName]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, String::format("Internal error creating object store '%s'.", objectStoreName.utf8().data())));
        });
        return;
    }
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::createIndex(IDBTransactionBackend& transaction, const CreateIndexOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    const IDBIndexMetadata& indexMetadata = operation.idbIndexMetadata();
    if (!m_backingStore->createIndex(*backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), indexMetadata.id, indexMetadata.name, indexMetadata.keyPath, indexMetadata.unique, indexMetadata.multiEntry)) {
        callOnMainThread([completionCallback, indexMetadata]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, String::format("Internal error when trying to create index '%s'.", indexMetadata.name.utf8().data())));
        });
        return;
    }
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::deleteIndex(IDBTransactionBackend& transaction, const DeleteIndexOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    const IDBIndexMetadata& indexMetadata = operation.idbIndexMetadata();
    if (!m_backingStore->deleteIndex(*backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), indexMetadata.id)) {
        callOnMainThread([completionCallback, indexMetadata]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, String::format("Internal error deleting index '%s'.", indexMetadata.name.utf8().data())));
        });
        return;
    }
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::get(IDBTransactionBackend& transaction, const GetOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    RefPtr<IDBKey> key;

    if (operation.keyRange()->isOnlyKey())
        key = operation.keyRange()->lower();
    else {
        RefPtr<IDBBackingStoreCursorLevelDB> backingStoreCursor;
        int64_t cursorID = m_nextCursorID++;

        if (operation.indexID() == IDBIndexMetadata::InvalidId) {
            ASSERT(operation.cursorType() != IndexedDB::CursorType::KeyOnly);
            // ObjectStore Retrieval Operation
            backingStoreCursor = m_backingStore->openObjectStoreCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.keyRange(), IndexedDB::CursorDirection::Next);
        } else {
            if (operation.cursorType() == IndexedDB::CursorType::KeyOnly) {
                // Index Value Retrieval Operation
                backingStoreCursor = m_backingStore->openIndexKeyCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.indexID(), operation.keyRange(), IndexedDB::CursorDirection::Next);
            } else {
                // Index Referenced Value Retrieval Operation
                backingStoreCursor = m_backingStore->openIndexCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.indexID(), operation.keyRange(), IndexedDB::CursorDirection::Next);
            }
        }

        if (!backingStoreCursor) {
            operation.callbacks()->onSuccess();
            ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
            return;
        }

        key = backingStoreCursor->key();
    }

    RefPtr<IDBKey> primaryKey;
    bool ok;
    if (operation.indexID() == IDBIndexMetadata::InvalidId) {
        // Object Store Retrieval Operation
        Vector<char> value;
        ok = m_backingStore->getRecord(*backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), *key, value);
        if (!ok) {
            operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error in getRecord."));
            ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
            return;
        }

        if (value.isEmpty()) {
            operation.callbacks()->onSuccess();
            ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
            return;
        }

        if (operation.autoIncrement() && !operation.keyPath().isNull()) {
            operation.callbacks()->onSuccess(SharedBuffer::adoptVector(value), key, operation.keyPath());
            ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
            return;
        }

        operation.callbacks()->onSuccess(SharedBuffer::adoptVector(value));
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;

    }

    // From here we are dealing only with indexes.
    ok = m_backingStore->getPrimaryKeyViaIndex(*backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.indexID(), *key, primaryKey);
    if (!ok) {
        operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error in getPrimaryKeyViaIndex."));
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }
    if (!primaryKey) {
        operation.callbacks()->onSuccess();
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }
    if (operation.cursorType() == IndexedDB::CursorType::KeyOnly) {
        // Index Value Retrieval Operation
        operation.callbacks()->onSuccess(primaryKey.get());
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }

    // Index Referenced Value Retrieval Operation
    Vector<char> value;
    ok = m_backingStore->getRecord(*backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), *primaryKey, value);
    if (!ok) {
        operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error in getRecord."));
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }

    if (value.isEmpty()) {
        operation.callbacks()->onSuccess();
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }
    if (operation.autoIncrement() && !operation.keyPath().isNull()) {
        operation.callbacks()->onSuccess(SharedBuffer::adoptVector(value), primaryKey, operation.keyPath());
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }
    operation.callbacks()->onSuccess(SharedBuffer::adoptVector(value));
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::put(IDBTransactionBackend& transaction, const PutOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    bool keyWasGenerated = false;

    RefPtr<IDBKey> key;
    if (operation.putMode() != IDBDatabaseBackend::CursorUpdate && operation.objectStore().autoIncrement && !operation.key()) {
        RefPtr<IDBKey> autoIncKey = m_backingStore->generateKey(transaction, transaction.database().id(), operation.objectStore().id);
        keyWasGenerated = true;
        if (!autoIncKey->isValid()) {
            operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::ConstraintError, "Maximum key generator value reached."));
            ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
            return;
        }
        key = autoIncKey;
    } else
        key = operation.key();

    ASSERT(key);
    ASSERT(key->isValid());

    RefPtr<IDBRecordIdentifier> recordIdentifier;
    if (operation.putMode() == IDBDatabaseBackend::AddOnly) {
        bool ok = m_backingStore->keyExistsInObjectStore(*backingStoreTransaction, transaction.database().id(), operation.objectStore().id, *key, recordIdentifier);
        if (!ok) {
            operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error checking key existence."));
            ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
            return;
        }
        if (recordIdentifier) {
            operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::ConstraintError, "Key already exists in the object store."));
            ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
            return;
        }
    }

    Vector<RefPtr<IDBIndexWriterLevelDB>> indexWriters;
    String errorMessage;
    bool obeysConstraints = false;
    bool backingStoreSuccess = m_backingStore->makeIndexWriters(transaction.id(), transaction.database().id(), operation.objectStore(), *key, keyWasGenerated, operation.indexIDs(), operation.indexKeys(), indexWriters, &errorMessage, obeysConstraints);
    if (!backingStoreSuccess) {
        operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error: backing store error updating index keys."));
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }
    if (!obeysConstraints) {
        operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::ConstraintError, errorMessage));
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }

    // Before this point, don't do any mutation. After this point, rollback the transaction in case of error.
    backingStoreSuccess = m_backingStore->putRecord(*backingStoreTransaction, transaction.database().id(), operation.objectStore().id, *key, operation.value(), recordIdentifier.get());
    if (!backingStoreSuccess) {
        operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error: backing store error performing put/add."));
        ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
        return;
    }

    for (size_t i = 0; i < indexWriters.size(); ++i) {
        IDBIndexWriterLevelDB* indexWriter = indexWriters[i].get();
        indexWriter->writeIndexKeys(recordIdentifier.get(), *m_backingStore, *backingStoreTransaction, transaction.database().id(), operation.objectStore().id);
    }

    if (operation.objectStore().autoIncrement && operation.putMode() != IDBDatabaseBackend::CursorUpdate && key->type() == IDBKey::NumberType) {
        bool ok = m_backingStore->updateKeyGenerator(transaction, transaction.database().id(), operation.objectStore().id, *key, !keyWasGenerated);
        if (!ok) {
            operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Internal error updating key generator."));
            ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
            return;
        }
    }

    operation.callbacks()->onSuccess(key.release());
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::openCursor(IDBTransactionBackend& transaction, const OpenCursorOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    // The frontend has begun indexing, so this pauses the transaction
    // until the indexing is complete. This can't happen any earlier
    // because we don't want to switch to early mode in case multiple
    // indexes are being created in a row, with put()'s in between.
    if (operation.taskType() == IDBDatabaseBackend::PreemptiveTask)
        transaction.addPreemptiveEvent();

    int64_t cursorID = m_nextCursorID++;

    RefPtr<IDBBackingStoreCursorLevelDB> backingStoreCursor;
    if (operation.indexID() == IDBIndexMetadata::InvalidId) {
        ASSERT(operation.cursorType() != IndexedDB::CursorType::KeyOnly);
        backingStoreCursor = m_backingStore->openObjectStoreCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.keyRange(), operation.direction());
    } else {
        ASSERT(operation.taskType() == IDBDatabaseBackend::NormalTask);
        if (operation.cursorType() == IndexedDB::CursorType::KeyOnly)
            backingStoreCursor = m_backingStore->openIndexKeyCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.indexID(), operation.keyRange(), operation.direction());
        else
            backingStoreCursor = m_backingStore->openIndexCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.indexID(), operation.keyRange(), operation.direction());
    }

    if (!backingStoreCursor) {
        operation.callbacks()->onSuccess(static_cast<SharedBuffer*>(0));
        callOnMainThread([completionCallback]() {
            completionCallback(0);
        });
        return;
    }

    IDBDatabaseBackend::TaskType taskType(static_cast<IDBDatabaseBackend::TaskType>(operation.taskType()));

    RefPtr<IDBCursorBackend> cursor = IDBCursorBackend::create(cursorID, operation.cursorType(), taskType, transaction, operation.objectStoreID());

    operation.callbacks()->onSuccess(cursor, cursor->key(), cursor->primaryKey(), cursor->value());

    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::count(IDBTransactionBackend& transaction, const CountOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    uint32_t count = 0;
    RefPtr<IDBBackingStoreCursorLevelDB> backingStoreCursor;

    int64_t cursorID = m_nextCursorID++;

    if (operation.indexID() == IDBIndexMetadata::InvalidId)
        backingStoreCursor = m_backingStore->openObjectStoreKeyCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.keyRange(), IndexedDB::CursorDirection::Next);
    else
        backingStoreCursor = m_backingStore->openIndexKeyCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.indexID(), operation.keyRange(), IndexedDB::CursorDirection::Next);
    if (!backingStoreCursor) {
        operation.callbacks()->onSuccess(count);
        callOnMainThread([completionCallback]() {
            completionCallback(0);
        });
        return;
    }

    do {
        ++count;
    } while (backingStoreCursor->continueFunction(0));

    operation.callbacks()->onSuccess(count);
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::deleteRange(IDBTransactionBackend& transaction, const DeleteRangeOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    int64_t cursorID = m_nextCursorID++;

    RefPtr<IDBBackingStoreCursorLevelDB> backingStoreCursor = m_backingStore->openObjectStoreCursor(cursorID, *backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), operation.keyRange(), IndexedDB::CursorDirection::Next);
    if (backingStoreCursor) {
        do {
            if (!m_backingStore->deleteRecord(*backingStoreTransaction, transaction.database().id(), operation.objectStoreID(), backingStoreCursor->recordIdentifier())) {
                operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Error deleting data in range"));
                callOnMainThread([completionCallback]() {
                    completionCallback(0);
                });
                return;
            }
        } while (backingStoreCursor->continueFunction(0));
    }

    operation.callbacks()->onSuccess();
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::clearObjectStore(IDBTransactionBackend& transaction, const ClearObjectStoreOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    if (!m_backingStore->clearObjectStore(*backingStoreTransaction, transaction.database().id(), operation.objectStoreID())) {
        operation.callbacks()->onError(IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Error clearing object store"));
        callOnMainThread([completionCallback]() {
            completionCallback(0);
        });
        return;
    }
    operation.callbacks()->onSuccess();
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::deleteObjectStore(IDBTransactionBackend& transaction, const DeleteObjectStoreOperation& operation, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    const IDBObjectStoreMetadata& objectStoreMetadata = operation.objectStoreMetadata();

    if (!m_backingStore->deleteObjectStore(*backingStoreTransaction, transaction.database().id(), objectStoreMetadata.id)) {
        callOnMainThread([completionCallback, objectStoreMetadata]() {
            completionCallback(IDBDatabaseError::create(IDBDatabaseException::UnknownError, String::format("Internal error deleting object store '%s'.", objectStoreMetadata.name.utf8().data())));
        });
        return;
    }
    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::changeDatabaseVersion(IDBTransactionBackend& transaction, const IDBDatabaseBackend::VersionChangeOperation&, std::function<void(PassRefPtr<IDBDatabaseError>)> completionCallback)
{
    IDBBackingStoreTransactionLevelDB* backingStoreTransaction = m_backingStoreTransactions.get(transaction.id());
    ASSERT(backingStoreTransaction);

    IDBDatabaseBackend& database = transaction.database();
    int64_t databaseId = database.id();

    if (!m_backingStore->updateIDBDatabaseVersion(*backingStoreTransaction, databaseId, database.metadata().version)) {
        RefPtr<IDBDatabaseError> error = IDBDatabaseError::create(IDBDatabaseException::UnknownError, "Error writing data to stable storage when updating version.");
        ASYNC_COMPLETION_CALLBACK_WITH_ARG(completionCallback, error);
        return;
    }

    ASYNC_COMPLETION_CALLBACK_WITH_NULL_ARG(completionCallback);
}

void IDBServerConnectionLevelDB::cursorAdvance(IDBCursorBackend& cursor, const CursorAdvanceOperation& operation, std::function<void()> completionCallback)
{
    IDBBackingStoreCursorLevelDB* backingStoreCursor = cursor.id() ? m_backingStoreCursors.get(cursor.id()) : 0;
#ifndef NDEBUG
    if (cursor.id())
        ASSERT(backingStoreCursor);
#endif

    if (!backingStoreCursor || !backingStoreCursor->advance(operation.count())) {
        m_backingStoreCursors.remove(cursor.id());
        cursor.clear();

        operation.callbacks()->onSuccess(static_cast<SharedBuffer*>(0));
        EMPTY_ASYNC_COMPLETION_CALLBACK(completionCallback);
        return;
    }

    cursor.updateCursorData(backingStoreCursor->key().get(), backingStoreCursor->primaryKey().get(), backingStoreCursor->value().get());
    operation.callbacks()->onSuccess(backingStoreCursor->key(), backingStoreCursor->primaryKey(), backingStoreCursor->value());
    EMPTY_ASYNC_COMPLETION_CALLBACK(completionCallback);
}

void IDBServerConnectionLevelDB::cursorIterate(IDBCursorBackend& cursor, const CursorIterationOperation& operation, std::function<void()> completionCallback)
{
    IDBBackingStoreCursorLevelDB* backingStoreCursor = cursor.id() ? m_backingStoreCursors.get(cursor.id()) : 0;
#ifndef NDEBUG
    if (cursor.id())
        ASSERT(backingStoreCursor);
#endif

    EMPTY_ASYNC_COMPLETION_CALLBACK(completionCallback);

    if (!backingStoreCursor || !backingStoreCursor->continueFunction(operation.key())) {
        m_backingStoreCursors.remove(cursor.id());
        cursor.clear();
        operation.callbacks()->onSuccess(static_cast<SharedBuffer*>(0));
        return;
    }

    cursor.updateCursorData(backingStoreCursor->key().get(), backingStoreCursor->primaryKey().get(), backingStoreCursor->value().get());
    operation.callbacks()->onSuccess(backingStoreCursor->key(), backingStoreCursor->primaryKey(), backingStoreCursor->value());
    
}

void IDBServerConnectionLevelDB::cursorPrefetchIteration(IDBCursorBackend& cursor, const CursorPrefetchIterationOperation& operation, std::function<void()> completionCallback)
{
    IDBBackingStoreCursorLevelDB* backingStoreCursor = cursor.id() ? m_backingStoreCursors.get(cursor.id()) : 0;
#ifndef NDEBUG
    if (cursor.id())
        ASSERT(backingStoreCursor);
#endif

    Vector<RefPtr<IDBKey>> foundKeys;
    Vector<RefPtr<IDBKey>> foundPrimaryKeys;
    Vector<RefPtr<SharedBuffer>> foundValues;

    if (backingStoreCursor) {
        int64_t cursorID = m_nextCursorID++;
        RefPtr<IDBBackingStoreCursorLevelDB> savedCursor = backingStoreCursor->clone();
        m_backingStoreCursors.set(cursorID, savedCursor.release());
        cursor.setSavedCursorID(cursorID);
    }

    const size_t maxSizeEstimate = 10 * 1024 * 1024;
    size_t sizeEstimate = 0;

    for (int i = 0; i < operation.numberToFetch(); ++i) {
        if (!backingStoreCursor || !backingStoreCursor->continueFunction(0)) {
            m_backingStoreCursors.remove(cursor.id());
            cursor.clear();
            break;
        }

        foundKeys.append(backingStoreCursor->key());
        foundPrimaryKeys.append(backingStoreCursor->primaryKey());

        switch (cursor.cursorType()) {
        case IndexedDB::CursorType::KeyOnly:
            foundValues.append(SharedBuffer::create());
            break;
        case IndexedDB::CursorType::KeyAndValue:
            sizeEstimate += backingStoreCursor->value()->size();
            foundValues.append(backingStoreCursor->value());
            break;
        default:
            ASSERT_NOT_REACHED();
        }
        sizeEstimate += backingStoreCursor->key()->sizeEstimate();
        sizeEstimate += backingStoreCursor->primaryKey()->sizeEstimate();

        if (sizeEstimate > maxSizeEstimate)
            break;
    }

    if (!foundKeys.size()) {
        operation.callbacks()->onSuccess(static_cast<SharedBuffer*>(0));
        return;
    }

    operation.callbacks()->onSuccessWithPrefetch(foundKeys, foundPrimaryKeys, foundValues);
    
    EMPTY_ASYNC_COMPLETION_CALLBACK(completionCallback);
}

void IDBServerConnectionLevelDB::cursorPrefetchReset(IDBCursorBackend& cursor, int usedPrefetches)
{
    IDBBackingStoreCursorLevelDB* backingStoreCursor = cursor.id() ? m_backingStoreCursors.get(cursor.id()) : 0;
    ASSERT(backingStoreCursor);

    for (int i = 0; i < usedPrefetches; ++i) {
        bool ok = backingStoreCursor->continueFunction();
        ASSERT_UNUSED(ok, ok);
    }
}

} // namespace WebCore

#endif // USE(LEVELDB)
#endif // ENABLE(INDEXED_DATABASE)
