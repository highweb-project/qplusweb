/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "PageThrottler.h"

#include "Chrome.h"
#include "ChromeClient.h"
#include "MainFrame.h"
#include "Page.h"
#include "PageActivityAssertionToken.h"
#include <wtf/StdLibExtras.h>

namespace WebCore {

static const double kThrottleHysteresisSeconds = 2.0;

PageThrottler::PageThrottler(Page& page)
    : m_page(page)
    , m_throttleState(PageNotThrottledState)
    , m_throttleHysteresisTimer(this, &PageThrottler::throttleHysteresisTimerFired)
    , m_visuallyNonIdle("Page is not visually idle.")
{
    m_page.chrome().client().incrementActivePageCount();
}

PageThrottler::~PageThrottler()
{
    setIsVisuallyIdle(false);

    for (auto it = m_activityTokens.begin(), end = m_activityTokens.end(); it != end; ++it)
        (*it)->invalidate();

    if (m_throttleState != PageThrottledState)
        m_page.chrome().client().decrementActivePageCount();
}

std::unique_ptr<PageActivityAssertionToken> PageThrottler::createActivityToken()
{
    return std::make_unique<PageActivityAssertionToken>(*this);
}

void PageThrottler::throttlePage()
{
    m_throttleState = PageThrottledState;

    m_page.chrome().client().decrementActivePageCount();

    for (Frame* frame = &m_page.mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (frame->document())
            frame->document()->scriptedAnimationControllerSetThrottled(true);
    }

    m_page.throttleTimers();
}

void PageThrottler::unthrottlePage()
{
    PageThrottleState oldState = m_throttleState;
    m_throttleState = PageNotThrottledState;

    if (oldState == PageNotThrottledState)
        return;

    if (oldState == PageThrottledState)
        m_page.chrome().client().incrementActivePageCount();
    
    for (Frame* frame = &m_page.mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (frame->document())
            frame->document()->scriptedAnimationControllerSetThrottled(false);
    }

    m_page.unthrottleTimers();
}

void PageThrottler::setIsVisuallyIdle(bool isVisuallyIdle)
{
    if (isVisuallyIdle) {
        m_throttleState = PageWaitingToThrottleState;
        startThrottleHysteresisTimer();
        if (m_visuallyNonIdle.isActive())
            m_visuallyNonIdle.endActivity();
    } else {
        unthrottlePage();
        stopThrottleHysteresisTimer();
        if (!m_visuallyNonIdle.isActive())
            m_visuallyNonIdle.beginActivity();
    }
}

void PageThrottler::stopThrottleHysteresisTimer()
{
    m_throttleHysteresisTimer.stop();
}

void PageThrottler::reportInterestingEvent()
{
    if (m_throttleState == PageNotThrottledState)
        return;
    if (m_throttleState == PageThrottledState)
        unthrottlePage();
    m_throttleState = PageWaitingToThrottleState;
    startThrottleHysteresisTimer();
}

void PageThrottler::startThrottleHysteresisTimer()
{
    if (m_throttleHysteresisTimer.isActive())
        m_throttleHysteresisTimer.stop();
    if (!m_activityTokens.size())
        m_throttleHysteresisTimer.startOneShot(kThrottleHysteresisSeconds);
}

void PageThrottler::throttleHysteresisTimerFired(Timer<PageThrottler>&)
{
    ASSERT(!m_activityTokens.size());
    throttlePage();
}

void PageThrottler::addActivityToken(PageActivityAssertionToken& token)
{
    ASSERT(!m_activityTokens.contains(&token));

    m_activityTokens.add(&token);

    // If we've already got events that block throttling we can return early
    if (m_activityTokens.size() > 1)
        return;

    if (m_throttleState == PageNotThrottledState)
        return;

    if (m_throttleState == PageThrottledState)
        unthrottlePage();

    m_throttleState = PageWaitingToThrottleState;
    stopThrottleHysteresisTimer();
}

void PageThrottler::removeActivityToken(PageActivityAssertionToken& token)
{
    ASSERT(m_activityTokens.contains(&token));

    m_activityTokens.remove(&token);

    if (m_activityTokens.size())
        return;

    if (m_throttleState == PageNotThrottledState)
        return;

    ASSERT(m_throttleState == PageWaitingToThrottleState);
    startThrottleHysteresisTimer();
}

}
