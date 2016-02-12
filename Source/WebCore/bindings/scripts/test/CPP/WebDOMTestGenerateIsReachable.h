/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Samuel Weinig <sam.weinig@gmail.com>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef WebDOMTestGenerateIsReachable_h
#define WebDOMTestGenerateIsReachable_h

#include <WebDOMObject.h>
#include <WebDOMString.h>

namespace WebCore {
class TestGenerateIsReachable;
};


class WebDOMTestGenerateIsReachable : public WebDOMObject {
public:
    WebDOMTestGenerateIsReachable();
    explicit WebDOMTestGenerateIsReachable(WebCore::TestGenerateIsReachable*);
    WebDOMTestGenerateIsReachable(const WebDOMTestGenerateIsReachable&);
    WebDOMTestGenerateIsReachable& operator=(const WebDOMTestGenerateIsReachable&);
    virtual ~WebDOMTestGenerateIsReachable();


    WebCore::TestGenerateIsReachable* impl() const;

protected:
    struct WebDOMTestGenerateIsReachablePrivate;
    WebDOMTestGenerateIsReachablePrivate* m_impl;
};

WebCore::TestGenerateIsReachable* toWebCore(const WebDOMTestGenerateIsReachable&);
WebDOMTestGenerateIsReachable toWebKit(WebCore::TestGenerateIsReachable*);

#endif
