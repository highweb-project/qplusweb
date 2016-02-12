/*
 * Copyright (C) 2008 Holger Hans Peter Freyther <zecke@selfish.org>
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
#include "GeolocationClientGtk.h"

#if ENABLE(GEOLOCATION)

#include "Chrome.h"
#include "ChromeClient.h"
#include "Geolocation.h"
#include "GeolocationController.h"
#include "GeolocationError.h"
#include "GeolocationPosition.h"
#include "webkitgeolocationpolicydecisionprivate.h"
#include "webkitwebframeprivate.h"
#include "webkitwebviewprivate.h"
#include <glib/gi18n-lib.h>
#include <wtf/gobject/GRefPtr.h>

namespace WebKit {

GeolocationClient::GeolocationClient(WebKitWebView* webView)
    : m_webView(webView)
    , m_provider(this)
{
}

void GeolocationClient::geolocationDestroyed()
{
    delete this;
}

void GeolocationClient::startUpdating()
{
    m_provider.startUpdating();
}

void GeolocationClient::stopUpdating()
{
    m_provider.stopUpdating();
}

void GeolocationClient::setEnableHighAccuracy(bool enable)
{
    m_provider.setEnableHighAccuracy(enable);
}

WebCore::GeolocationPosition* GeolocationClient::lastPosition()
{
    return m_lastPosition.get();
}

void GeolocationClient::requestPermission(WebCore::Geolocation* geolocation)
{
    WebKitWebFrame* webFrame = kit(geolocation->frame());
    GRefPtr<WebKitGeolocationPolicyDecision> policyDecision(adoptGRef(webkit_geolocation_policy_decision_new(webFrame, geolocation)));

    gboolean isHandled = FALSE;
    g_signal_emit_by_name(m_webView, "geolocation-policy-decision-requested", webFrame, policyDecision.get(), &isHandled);
    
    if (!isHandled)
        webkit_geolocation_policy_deny(policyDecision.get());
#if MODIFY(ENGINE)
    else        
        webkit_geolocation_policy_allow(policyDecision.get());
#endif
}

void GeolocationClient::cancelPermissionRequest(WebCore::Geolocation* geolocation)
{
    g_signal_emit_by_name(m_webView, "geolocation-policy-decision-cancelled", kit(geolocation->frame()));
}

void GeolocationClient::notifyPositionChanged(int timestamp, double latitude, double longitude, double altitude, double accuracy, double altitudeAccuracy)
{
#if MODIFY(ENGINE)
    //current : 37.57000, 126.980000
    //infraware : 37.5006959, 127.0097287 
    double correctLatitude = 37.5006959 - 37.5700000;
    double correctLongitude = 127.0097287 - 126.980000;

    double calculatedLatitude = static_cast<double>(latitude + correctLatitude);
    double calculatedLongitude = static_cast<double>(longitude + correctLongitude);

    m_lastPosition = WebCore::GeolocationPosition::create(static_cast<double>(timestamp), calculatedLatitude, calculatedLongitude, accuracy, 
                                                                            true, altitude, true, altitudeAccuracy, false, 0, false, 0);
#else
    m_lastPosition = WebCore::GeolocationPosition::create(static_cast<double>(timestamp), latitude, longitude, accuracy, 
                                                                            true, altitude, true, altitudeAccuracy, false, 0, false, 0);
#endif
    WebCore::GeolocationController::from(core(m_webView))->positionChanged(m_lastPosition.get());
}

void GeolocationClient::notifyErrorOccurred(const char* message)
{
    RefPtr<WebCore::GeolocationError> error = WebCore::GeolocationError::create(WebCore::GeolocationError::PositionUnavailable, message);
    WebCore::GeolocationController::from(core(m_webView))->errorOccurred(error.get());
}

}

#endif // ENABLE(GEOLOCATION)
