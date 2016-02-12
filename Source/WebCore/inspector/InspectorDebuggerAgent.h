/*
 * Copyright (C) 2010, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2010-2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InspectorDebuggerAgent_h
#define InspectorDebuggerAgent_h

#if ENABLE(JAVASCRIPT_DEBUGGER) && ENABLE(INSPECTOR)

#include "ConsoleAPITypes.h"
#include "ConsoleTypes.h"
#include "InspectorWebAgentBase.h"
#include "ScriptBreakpoint.h"
#include "ScriptDebugListener.h"
#include <bindings/ScriptValue.h>
#include <debugger/Debugger.h>
#include <inspector/InspectorJSBackendDispatchers.h>
#include <inspector/InspectorJSFrontendDispatchers.h>
#include <wtf/Forward.h>
#include <wtf/HashMap.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>
#include <wtf/text/StringHash.h>

namespace Inspector {
class InjectedScript;
class InjectedScriptManager;
class InspectorArray;
class InspectorObject;
class InspectorValue;
}

namespace WebCore {

class InstrumentingAgents;
class ScriptDebugServer;

typedef String ErrorString;

class InspectorDebuggerAgent : public InspectorAgentBase, public ScriptDebugListener, public Inspector::InspectorDebuggerBackendDispatcherHandler {
    WTF_MAKE_NONCOPYABLE(InspectorDebuggerAgent); WTF_MAKE_FAST_ALLOCATED;
public:
    static const char* backtraceObjectGroup;

    virtual ~InspectorDebuggerAgent();

    virtual void didCreateFrontendAndBackend(Inspector::InspectorFrontendChannel*, Inspector::InspectorBackendDispatcher*) OVERRIDE;
    virtual void willDestroyFrontendAndBackend() OVERRIDE;

    bool isPaused();
    void addMessageToConsole(MessageSource, MessageType);

    // Part of the protocol.
    virtual void enable(ErrorString*);
    virtual void disable(ErrorString*);
    virtual void setBreakpointsActive(ErrorString*, bool active);

    virtual void setBreakpointByUrl(ErrorString*, int lineNumber, const String* optionalURL, const String* optionalURLRegex, const int* optionalColumnNumber, const RefPtr<Inspector::InspectorObject>* options, Inspector::TypeBuilder::Debugger::BreakpointId*, RefPtr<Inspector::TypeBuilder::Array<Inspector::TypeBuilder::Debugger::Location>>& locations);
    virtual void setBreakpoint(ErrorString*, const RefPtr<Inspector::InspectorObject>& location, const RefPtr<Inspector::InspectorObject>* options, Inspector::TypeBuilder::Debugger::BreakpointId*, RefPtr<Inspector::TypeBuilder::Debugger::Location>& actualLocation);
    virtual void removeBreakpoint(ErrorString*, const String& breakpointIdentifier);
    virtual void continueToLocation(ErrorString*, const RefPtr<Inspector::InspectorObject>& location);

    virtual void searchInContent(ErrorString*, const String& scriptID, const String& query, const bool* optionalCaseSensitive, const bool* optionalIsRegex, RefPtr<Inspector::TypeBuilder::Array<Inspector::TypeBuilder::GenericTypes::SearchMatch>>&);
    virtual void getScriptSource(ErrorString*, const String& scriptID, String* scriptSource);
    virtual void getFunctionDetails(ErrorString*, const String& functionId, RefPtr<Inspector::TypeBuilder::Debugger::FunctionDetails>&);
    virtual void pause(ErrorString*);
    virtual void resume(ErrorString*);
    virtual void stepOver(ErrorString*);
    virtual void stepInto(ErrorString*);
    virtual void stepOut(ErrorString*);
    virtual void setPauseOnExceptions(ErrorString*, const String& pauseState);
    virtual void evaluateOnCallFrame(ErrorString*,
                             const String& callFrameId,
                             const String& expression,
                             const String* objectGroup,
                             const bool* includeCommandLineAPI,
                             const bool* doNotPauseOnExceptionsAndMuteConsole,
                             const bool* returnByValue,
                             const bool* generatePreview,
                             RefPtr<Inspector::TypeBuilder::Runtime::RemoteObject>& result,
                             Inspector::TypeBuilder::OptOutput<bool>* wasThrown);
    virtual void setOverlayMessage(ErrorString*, const String*);

    void schedulePauseOnNextStatement(Inspector::InspectorDebuggerFrontendDispatcher::Reason::Enum breakReason, PassRefPtr<Inspector::InspectorObject> data);
    void cancelPauseOnNextStatement();
    void breakProgram(Inspector::InspectorDebuggerFrontendDispatcher::Reason::Enum breakReason, PassRefPtr<Inspector::InspectorObject> data);
    void scriptExecutionBlockedByCSP(const String& directiveText);

    class Listener {
    public:
        virtual ~Listener() { }
        virtual void debuggerWasEnabled() = 0;
        virtual void debuggerWasDisabled() = 0;
        virtual void stepInto() = 0;
        virtual void didPause() = 0;
    };
    void setListener(Listener* listener) { m_listener = listener; }

    virtual ScriptDebugServer& scriptDebugServer() = 0;

protected:
    InspectorDebuggerAgent(InstrumentingAgents*, Inspector::InjectedScriptManager*);

    virtual void startListeningScriptDebugServer() = 0;
    virtual void stopListeningScriptDebugServer() = 0;
    virtual void muteConsole() = 0;
    virtual void unmuteConsole() = 0;
    Inspector::InjectedScriptManager* injectedScriptManager() const { return m_injectedScriptManager; }
    virtual Inspector::InjectedScript injectedScriptForEval(ErrorString*, const int* executionContextId) = 0;

    virtual void enable();
    virtual void disable();
    virtual void didPause(JSC::ExecState*, const Deprecated::ScriptValue& callFrames, const Deprecated::ScriptValue& exception);
    virtual void didContinue();
    void reset();

private:
    PassRefPtr<Inspector::TypeBuilder::Array<Inspector::TypeBuilder::Debugger::CallFrame>> currentCallFrames();

    virtual void didParseSource(JSC::SourceID, const Script&) OVERRIDE FINAL;
    virtual void failedToParseSource(const String& url, const String& data, int firstLine, int errorLine, const String& errorMessage) OVERRIDE FINAL;

    PassRefPtr<Inspector::TypeBuilder::Debugger::Location> resolveBreakpoint(const String& breakpointIdentifier, JSC::SourceID, const ScriptBreakpoint&);
    void clear();
    bool assertPaused(ErrorString*);
    void clearBreakDetails();

    String sourceMapURLForScript(const Script&);

    typedef HashMap<JSC::SourceID, Script> ScriptsMap;
    typedef HashMap<String, Vector<JSC::BreakpointID>> BreakpointIdentifierToDebugServerBreakpointIDsMap;
    typedef HashMap<String, RefPtr<Inspector::InspectorObject>> BreakpointIdentifierToBreakpointMap;

    Inspector::InjectedScriptManager* m_injectedScriptManager;
    std::unique_ptr<Inspector::InspectorDebuggerFrontendDispatcher> m_frontendDispatcher;
    RefPtr<Inspector::InspectorDebuggerBackendDispatcher> m_backendDispatcher;
    JSC::ExecState* m_pausedScriptState;
    Deprecated::ScriptValue m_currentCallStack;
    ScriptsMap m_scripts;
    BreakpointIdentifierToDebugServerBreakpointIDsMap m_breakpointIdentifierToDebugServerBreakpointIDs;
    BreakpointIdentifierToBreakpointMap m_javaScriptBreakpoints;
    JSC::BreakpointID m_continueToLocationBreakpointID;
    Inspector::InspectorDebuggerFrontendDispatcher::Reason::Enum m_breakReason;
    RefPtr<Inspector::InspectorObject> m_breakAuxData;
    bool m_enabled;
    bool m_javaScriptPauseScheduled;
    Listener* m_listener;
};

} // namespace WebCore

#endif // ENABLE(JAVASCRIPT_DEBUGGER) && ENABLE(INSPECTOR)

#endif // !defined(InspectorDebuggerAgent_h)
