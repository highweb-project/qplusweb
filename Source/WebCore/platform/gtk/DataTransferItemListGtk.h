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
#ifndef DataTransferItemListGtk_h
#define DataTransferItemListGtk_h

#if ENABLE(DATA_TRANSFER_ITEMS)

#include "DataTransferItemList.h"
#include "DataTransferItemGtk.h"
#include <wtf/RefPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

class Clipboard;
class ScriptExecutionContext;
typedef int ExceptionCode;

class DataTransferItemListGtk : public DataTransferItemList {
public:
	static PassRefPtr<DataTransferItemListGtk> create(PassRefPtr<Clipboard>, ScriptExecutionContext*);

    virtual size_t length() const;
    virtual PassRefPtr<DataTransferItem> item(unsigned long index);
    virtual void deleteItem(unsigned long index, ExceptionCode&);
    virtual void clear();
    virtual void add(const String& data, const String& type, ExceptionCode&);
    virtual void add(PassRefPtr<File>);

private:
	DataTransferItemListGtk(PassRefPtr<Clipboard>, ScriptExecutionContext*);

    virtual void addPasteboardItem(const String& type);

    RefPtr<Clipboard> m_owner;
    // Indirectly owned by our parent.
    ScriptExecutionContext* m_context;
    Vector<RefPtr<DataTransferItemGtk> > m_items;
};

} // namespace WebCore

#endif // ENABLE(DATA_TRANSFER_ITEMS)

#endif // DataTransferItemList_h