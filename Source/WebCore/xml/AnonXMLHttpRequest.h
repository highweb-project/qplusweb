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
* THIS SOFTWARE IS PROVIDED BY ELECTRONICS AND TELECOMMUNICATIONS RESEARCH INSTITUE AND INFRAWARE INC AND ITS
* CONTRIBUTORS "AS IS", AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING
* BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SAMSUNG
* ELECTRONICS CORPORATION OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS, OR BUSINESS INTERRUPTION), HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
* NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef AnonXMLHttpRequest_h
#define AnonXMLHttpRequest_h
#if MODIFY(ENGINE)
#include "XMLHttpRequest.h"

namespace WebCore {
	
class AnonXMLHttpRequest : public XMLHttpRequest {
public:
	static PassRefPtr<AnonXMLHttpRequest> create(ScriptExecutionContext& context)
	{
		RefPtr<AnonXMLHttpRequest> anonXMLHttpRequest(adoptRef(new AnonXMLHttpRequest(context)));
		anonXMLHttpRequest->suspendIfNeeded();
		return anonXMLHttpRequest.release();
	}

private:
	AnonXMLHttpRequest(ScriptExecutionContext& context)
		: XMLHttpRequest(context)
	{
		m_anonymousFlag = true;
	}

}; 

}// namespace WebCore

#endif
#endif