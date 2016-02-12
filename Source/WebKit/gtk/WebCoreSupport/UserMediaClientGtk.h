/*
 * Copyright (C) 2012 Intel Inc. All rights reserved.
 * Copyright (C) 2014 Electronics and Telecommunicataions Research Institue and Infraware Inc. All rights reserved.
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
 *
 */
#ifndef UserMediaClientGtk_h
#define UserMediaClientGtk_h

#if ENABLE(MEDIA_STREAM)

#include "UserMediaClient.h"
#if MODIFY(ENGINE)
#include "webkitwebview.h"
#endif

namespace WebKit {

class UserMediaClientGtk : public WebCore::UserMediaClient {
public:
#if MODIFY(ENGINE) 
    UserMediaClientGtk(WebKitWebView* webView);
#endif
    UserMediaClientGtk();
    virtual ~UserMediaClientGtk();

    virtual void pageDestroyed();
    virtual void requestPermission(WTF::PassRefPtr<WebCore::UserMediaRequest>);
    virtual void cancelRequest(WebCore::UserMediaRequest*);
#if MODIFY(ENGINE)
    WebKitWebView* m_webView;
#endif
};

} // namespace WebKit

#endif // ENABLE(MEDIA_STREAM)

#endif // UserMediaClientGtk_h
