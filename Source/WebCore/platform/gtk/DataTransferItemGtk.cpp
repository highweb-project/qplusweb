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
#include "DataTransferItemGtk.h"

#if ENABLE(DATA_TRANSFER_ITEMS)

#include "Blob.h"
#include "Clipboard.h"
#include "File.h"
#include "NotImplemented.h"
#include "StringCallback.h"

#include <wtf/text/WTFString.h>
#include <wtf/text/CString.h>


namespace WebCore {

PassRefPtr<DataTransferItemGtk> DataTransferItemGtk::create(PassRefPtr<Clipboard> owner,
                                                          ScriptExecutionContext* context,
                                                          const String& data,
                                                          const String& type)
{
    RefPtr<DataTransferItemGtk> item = adoptRef(new DataTransferItemGtk(owner, context, DataTransferItemGtk::InternalSource, DataTransferItem::kindString, type, data));
    return item.release();
}    

PassRefPtr<DataTransferItemGtk> DataTransferItemGtk::create(PassRefPtr<Clipboard> owner,
                                                          ScriptExecutionContext* context,
                                                          PassRefPtr<File> file)
{
    RefPtr<DataTransferItemGtk> item = adoptRef(new DataTransferItemGtk(owner, context, DataTransferItemGtk::InternalSource, file));
    return item.release();
}

PassRefPtr<DataTransferItemGtk> DataTransferItemGtk::createFromPasteboard(PassRefPtr<Clipboard> owner,
                                                                        ScriptExecutionContext* context,
                                                                        const String& type)
{
    if (type == "text/plain" || type == "text/html")
        RefPtr<DataTransferItemGtk> item = adoptRef(new DataTransferItemGtk(owner, context, PasteboardSource, DataTransferItem::kindString, type, ""));
   
    RefPtr<DataTransferItemGtk> item = adoptRef(new DataTransferItemGtk(owner, context, PasteboardSource, DataTransferItem::kindFile, type, ""));
    return item.release();
}

DataTransferItemGtk::DataTransferItemGtk(PassRefPtr<Clipboard> owner,
                                       ScriptExecutionContext* context,
                                       DataSource source,
                                       const String& kind, const String& type,
                                       const String& data)
    : m_owner(owner)
    , m_context(context)
    , m_kind(kind)
    , m_type(type)
    , m_dataSource(source)
    , m_data(data)
{
}

DataTransferItemGtk::DataTransferItemGtk(PassRefPtr<Clipboard> owner,
                                       ScriptExecutionContext* context,
                                       DataSource source,
                                       PassRefPtr<File> file)
    : m_owner(owner)
    , m_context(context)
    , m_kind(kindFile)
    , m_type(file.get() ? file->type() : "")
    , m_dataSource(source)
    , m_file(file)
{
}

void DataTransferItemGtk::getAsString(PassRefPtr<StringCallback> callback) const
{
    if (!owner()->canReadData() || kind() != DataTransferItem::kindString || !callback)
        return;

    if (m_dataSource == InternalSource) {
        callback->scheduleCallback(m_context, m_data);
        return;
    }
    
}

PassRefPtr<Blob> DataTransferItemGtk::getAsFile() const
{
    if (kind() == kindFile && m_dataSource == InternalSource)
        return m_file;

    return 0;
}

}

#endif // ENABLE(DATA_TRANSFER_ITEMS)
