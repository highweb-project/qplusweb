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

#include "config.h"
#include "JSTestException.h"

#include "TestException.h"
#include "URL.h"
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSTestExceptionTableValues[] =
{
    { "name", DontDelete | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestExceptionName), (intptr_t)0 },
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestExceptionConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestExceptionTable = { 5, 3, JSTestExceptionTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSTestExceptionConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestExceptionConstructorTable = { 1, 0, JSTestExceptionConstructorTableValues, 0 };
const ClassInfo JSTestExceptionConstructor::s_info = { "TestExceptionConstructor", &Base::s_info, &JSTestExceptionConstructorTable, 0, CREATE_METHOD_TABLE(JSTestExceptionConstructor) };

JSTestExceptionConstructor::JSTestExceptionConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSTestExceptionConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSTestExceptionPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSTestExceptionConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSTestExceptionConstructor, JSDOMWrapper>(exec, JSTestExceptionConstructorTable, jsCast<JSTestExceptionConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSTestExceptionPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSTestExceptionPrototypeTable = { 1, 0, JSTestExceptionPrototypeTableValues, 0 };
const ClassInfo JSTestExceptionPrototype::s_info = { "TestExceptionPrototype", &Base::s_info, &JSTestExceptionPrototypeTable, 0, CREATE_METHOD_TABLE(JSTestExceptionPrototype) };

JSObject* JSTestExceptionPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTestException>(vm, globalObject);
}

const ClassInfo JSTestException::s_info = { "TestException", &Base::s_info, &JSTestExceptionTable, 0 , CREATE_METHOD_TABLE(JSTestException) };

JSTestException::JSTestException(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<TestException> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSTestException::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSTestException::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTestExceptionPrototype::create(vm, globalObject, JSTestExceptionPrototype::createStructure(vm, globalObject, globalObject->errorPrototype()));
}

void JSTestException::destroy(JSC::JSCell* cell)
{
    JSTestException* thisObject = static_cast<JSTestException*>(cell);
    thisObject->JSTestException::~JSTestException();
}

JSTestException::~JSTestException()
{
    releaseImplIfNotNull();
}

bool JSTestException::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTestException* thisObject = jsCast<JSTestException*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSTestException, Base>(exec, JSTestExceptionTable, thisObject, propertyName, slot);
}

EncodedJSValue jsTestExceptionName(ExecState* exec, EncodedJSValue slotBase, EncodedJSValue thisValue, PropertyName)
{
    JSTestException* castedThis = jsDynamicCast<JSTestException*>(JSValue::decode(thisValue));
    UNUSED_PARAM(slotBase);
    if (!castedThis)
        return throwVMTypeError(exec);
    UNUSED_PARAM(exec);
    TestException& impl = castedThis->impl();
    JSValue result = jsStringWithCache(exec, impl.name());
    return JSValue::encode(result);
}


EncodedJSValue jsTestExceptionConstructor(ExecState* exec, EncodedJSValue thisValue, EncodedJSValue, PropertyName)
{
    JSTestException* domObject = jsDynamicCast<JSTestException*>(JSValue::decode(thisValue));
    if (!domObject)
        return throwVMTypeError(exec);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSTestException::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSTestException::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestExceptionConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

bool JSTestExceptionOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSTestExceptionOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSTestException* jsTestException = jsCast<JSTestException*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestException->impl(), jsTestException);
    jsTestException->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestException@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore13TestExceptionE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, TestException* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSTestException>(exec, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7TestException@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore13TestExceptionE[2];
#if COMPILER(CLANG)
    // If this fails TestException does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(TestException), TestException_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // TestException has subclasses. If TestException has subclasses that get passed
    // to toJS() we currently require TestException you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    ReportMemoryCost<TestException>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSTestException>(exec, globalObject, impl);
}

TestException* toTestException(JSC::JSValue value)
{
    return value.inherits(JSTestException::info()) ? &jsCast<JSTestException*>(value)->impl() : 0;
}

}
