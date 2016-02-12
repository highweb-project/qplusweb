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
#include "JSDataTransferItemList.h"

#include "Clipboard.h"
#include "Element.h"
#include "HTMLImageElement.h"
#include "HTMLNames.h"
#include "IntPoint.h"
#include "JSNode.h"
#include "Node.h"

#include <runtime/ArrayPrototype.h>
#include <runtime/Error.h>
#include <wtf/HashSet.h>
#include <wtf/text/StringHash.h>

using namespace JSC;

namespace WebCore {

using namespace HTMLNames;

bool JSDataTransferItemList::deleteProperty(JSC::JSCell* cell, JSC::ExecState* exec, JSC::PropertyName propertyName)
{	
    JSDataTransferItemList* thisObject = jsCast<JSDataTransferItemList*>(cell);

	ExceptionCode ec = 0;
  
    unsigned i = propertyName.asIndex();
	if (i != PropertyName::NotAnIndex) {
        thisObject->impl().deleteItem(i, ec);
		if (ec) {
			setDOMException(exec, ec);
			return false;
		}
	}
		
    return JSObject::deleteProperty(thisObject, exec, propertyName);
}

bool JSDataTransferItemList::deletePropertyByIndex(JSC::JSCell* cell, JSC::ExecState* exec, unsigned i)
{
	
	JSDataTransferItemList* thisObject = jsCast<JSDataTransferItemList*>(cell);

	ExceptionCode ec = 0;
 	thisObject->impl().deleteItem(i, ec);
	if (ec) {
		setDOMException(exec, ec);
		return false;
		}
		
    return true;
}

} // namespace WebCore
