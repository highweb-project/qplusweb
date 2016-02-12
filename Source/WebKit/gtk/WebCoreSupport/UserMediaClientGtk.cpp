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

#include "config.h"
#include "UserMediaClientGtk.h"

#if ENABLE(MEDIA_STREAM)
#include "NotImplemented.h"
#include "UserMediaRequest.h"

using namespace WebCore;

namespace WebKit {

UserMediaClientGtk::UserMediaClientGtk()
{
}

#if MODIFY(ENGINE)
UserMediaClientGtk::UserMediaClientGtk(WebKitWebView* webView)
    :m_webView(webView)
{    
}
#endif
UserMediaClientGtk::~UserMediaClientGtk()
{
}

void UserMediaClientGtk::pageDestroyed()
{
    notImplemented();
}

void UserMediaClientGtk::requestPermission(PassRefPtr<UserMediaRequest> request)
{
#if MODIFY(ENGINE)
    gboolean isHandled = FALSE;
    g_signal_emit_by_name(m_webView, "user-media-policy-decision-derequested", &isHandled);

    if(!isHandled)
        request->userMediaAccessDenied();
    else
        request->userMediaAccessGranted();
#else
	 notImplemented();
#endif
}

void UserMediaClientGtk::cancelRequest(UserMediaRequest*)
{
    notImplemented();
}

} // namespace WebKit;

#endif // MEDIA_STREAM
