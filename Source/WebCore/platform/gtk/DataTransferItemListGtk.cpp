/*
* Copyright (C) 2014 Electronics and Telecommunicataions Research Institue and Infraware Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided the following conditions
* are met:
*
* 1.  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
* 2.  Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND ITS
* CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
* BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
* THE COPYRIGHT OWNER OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
* NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "config.h"


#if ENABLE(DATA_TRANSFER_ITEMS)

#include "Clipboard.h"
#include "File.h"

#include "DataTransferItemListGtk.h"
#include "ExceptionCode.h"

#include <wtf/text/WTFString.h>
#include <wtf/text/CString.h>

namespace WebCore {

PassRefPtr<DataTransferItemListGtk> DataTransferItemListGtk::create(PassRefPtr<Clipboard> owner, ScriptExecutionContext* context)
{    
    return adoptRef(new DataTransferItemListGtk(owner, context));
}

DataTransferItemListGtk::DataTransferItemListGtk(PassRefPtr<Clipboard> owner, ScriptExecutionContext* context)
    : m_owner(owner)
    , m_context(context)
{
}

size_t DataTransferItemListGtk::length() const
{   
    return m_items.size(); 
}

PassRefPtr<DataTransferItem> DataTransferItemListGtk::item(unsigned long index)
{
    if (!m_owner->canReadTypes() || index >= length())
        return 0;

    return m_items[index];
}

void DataTransferItemListGtk::deleteItem(unsigned long index, ExceptionCode& ec)
{   
    if (!m_owner->canWriteData()) {
        ec = INVALID_STATE_ERR;
        return;
    }

    if (index >= length())
        return;

    m_items.remove(index);
}

void DataTransferItemListGtk::clear()
{
    if (!m_owner->canWriteData())
        return;

    m_items.clear();

}

void DataTransferItemListGtk::add(const String& data, const String& type, ExceptionCode& ec)
{   
    if (!m_owner->canWriteData())
        return;

    // Only one 'string' item with a given type is allowed in the collection.
    
    for (size_t i = 0; i < m_items.size(); ++i) {
        if (m_items[i]->type() == type && m_items[i]->kind() == DataTransferItem::kindString) {
            ec = NOT_SUPPORTED_ERR;
            return;
        }
    }    
    m_items.append(DataTransferItemGtk::create(m_owner, m_context, data, type));

    
}

void DataTransferItemListGtk::add(PassRefPtr<File> file)
{
    if (!m_owner->canWriteData() || !file)
        return;

    m_items.append(DataTransferItemGtk::create(m_owner, m_context, file));
}

void DataTransferItemListGtk::addPasteboardItem(const String& type)
{
    m_items.append(DataTransferItemGtk::createFromPasteboard(m_owner, m_context, type));
}

} // namespace WebCore

#endif // ENABLE(DATA_TRANSFER_ITEMS)
