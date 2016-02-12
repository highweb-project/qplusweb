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
#ifndef DataTransferItemGtk_h
#define DataTransferItemGtk_h

#if ENABLE(DATA_TRANSFER_ITEMS)

#include "DataTransferItem.h"
#include <wtf/text/WTFString.h>
#include <wtf/RefPtr.h>

namespace WebCore {

class Clipboard;
class File;
class ScriptExecutionContext;

class DataTransferItemGtk : public DataTransferItem {
public:
    static PassRefPtr<DataTransferItemGtk> create(PassRefPtr<Clipboard> owner, ScriptExecutionContext*, const String& data, const String& type);
    static PassRefPtr<DataTransferItemGtk> create(PassRefPtr<Clipboard> owner, ScriptExecutionContext*, PassRefPtr<File>);
    static PassRefPtr<DataTransferItemGtk> createFromPasteboard(PassRefPtr<Clipboard> owner,
                                                               ScriptExecutionContext*,
                                                               const String&);

    virtual String kind() const { return m_kind; }
    virtual String type() const { return m_type; }
    RefPtr<Clipboard> owner() const { return m_owner; }
    virtual void getAsString(PassRefPtr<StringCallback>) const;
    virtual PassRefPtr<Blob> getAsFile() const;

private:
    friend class DataTransferItemListGtk;

    enum DataSource {
        PasteboardSource,
        InternalSource
    };

    DataTransferItemGtk(PassRefPtr<Clipboard> owner,
                       ScriptExecutionContext*,
                       DataSource,
                       const String&, const String&, const String&);
    DataTransferItemGtk(PassRefPtr<Clipboard> owner,
                       ScriptExecutionContext*,
                       DataSource,
                       PassRefPtr<File>);

    const RefPtr<Clipboard> m_owner;
    ScriptExecutionContext* m_context;
    const String m_kind;
    const String m_type;
    const DataSource m_dataSource;
    const String m_data;
    RefPtr<File> m_file;
};

}

#endif // ENABLE(DATA_TRANSFER_ITEMS)

#endif