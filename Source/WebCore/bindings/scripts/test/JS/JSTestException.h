/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef JSTestException_h
#define JSTestException_h

#include "JSDOMBinding.h"
#include "TestException.h"
#include <runtime/ErrorPrototype.h>
#include <runtime/JSGlobalObject.h>
#include <runtime/JSObject.h>

namespace WebCore {

class JSTestException : public JSDOMWrapper {
public:
    typedef JSDOMWrapper Base;
    static JSTestException* create(JSC::Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<TestException> impl)
    {
        JSTestException* ptr = new (NotNull, JSC::allocateCell<JSTestException>(globalObject->vm().heap)) JSTestException(structure, globalObject, impl);
        ptr->finishCreation(globalObject->vm());
        return ptr;
    }

    static JSC::JSObject* createPrototype(JSC::VM&, JSC::JSGlobalObject*);
    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    static void destroy(JSC::JSCell*);
    ~JSTestException();
    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

    static JSC::JSValue getConstructor(JSC::VM&, JSC::JSGlobalObject*);
    TestException& impl() const { return *m_impl; }
    void releaseImpl() { m_impl->deref(); m_impl = 0; }

    void releaseImplIfNotNull()
    {
        if (m_impl) {
            m_impl->deref();
            m_impl = 0;
        }
    }

private:
    TestException* m_impl;
protected:
    JSTestException(JSC::Structure*, JSDOMGlobalObject*, PassRefPtr<TestException>);
    void finishCreation(JSC::VM&);
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::InterceptsGetOwnPropertySlotByIndexEvenWhenLengthIsNotZero | Base::StructureFlags;
};

class JSTestExceptionOwner : public JSC::WeakHandleOwner {
public:
    virtual bool isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown>, void* context, JSC::SlotVisitor&);
    virtual void finalize(JSC::Handle<JSC::Unknown>, void* context);
};

inline JSC::WeakHandleOwner* wrapperOwner(DOMWrapperWorld&, TestException*)
{
    DEFINE_STATIC_LOCAL(JSTestExceptionOwner, jsTestExceptionOwner, ());
    return &jsTestExceptionOwner;
}

inline void* wrapperContext(DOMWrapperWorld& world, TestException*)
{
    return &world;
}

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, TestException*);
TestException* toTestException(JSC::JSValue);

class JSTestExceptionPrototype : public JSC::JSNonFinalObject {
public:
    typedef JSC::JSNonFinalObject Base;
    static JSC::JSObject* self(JSC::VM&, JSC::JSGlobalObject*);
    static JSTestExceptionPrototype* create(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestExceptionPrototype* ptr = new (NotNull, JSC::allocateCell<JSTestExceptionPrototype>(vm.heap)) JSTestExceptionPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestExceptionPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure) : JSC::JSNonFinalObject(vm, structure) { }
protected:
    static const unsigned StructureFlags = Base::StructureFlags;
};

class JSTestExceptionConstructor : public DOMConstructorObject {
private:
    JSTestExceptionConstructor(JSC::Structure*, JSDOMGlobalObject*);
    void finishCreation(JSC::VM&, JSDOMGlobalObject*);

public:
    typedef DOMConstructorObject Base;
    static JSTestExceptionConstructor* create(JSC::VM& vm, JSC::Structure* structure, JSDOMGlobalObject* globalObject)
    {
        JSTestExceptionConstructor* ptr = new (NotNull, JSC::allocateCell<JSTestExceptionConstructor>(vm.heap)) JSTestExceptionConstructor(structure, globalObject);
        ptr->finishCreation(vm, globalObject);
        return ptr;
    }

    static bool getOwnPropertySlot(JSC::JSObject*, JSC::ExecState*, JSC::PropertyName, JSC::PropertySlot&);
    DECLARE_INFO;
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::ImplementsHasInstance | DOMConstructorObject::StructureFlags;
};

// Attributes

JSC::EncodedJSValue jsTestExceptionName(JSC::ExecState*, JSC::EncodedJSValue, JSC::EncodedJSValue, JSC::PropertyName);
JSC::EncodedJSValue jsTestExceptionConstructor(JSC::ExecState*, JSC::EncodedJSValue, JSC::EncodedJSValue, JSC::PropertyName);

} // namespace WebCore

#endif
