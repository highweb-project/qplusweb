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
#include "JSreadonly.h"

#include "readonly.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table */

static const HashTableValue JSreadonlyTableValues[] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsreadonlyConstructor), (intptr_t)0 },
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSreadonlyTable = { 2, 1, JSreadonlyTableValues, 0 };
/* Hash table for constructor */

static const HashTableValue JSreadonlyConstructorTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSreadonlyConstructorTable = { 1, 0, JSreadonlyConstructorTableValues, 0 };
const ClassInfo JSreadonlyConstructor::s_info = { "readonlyConstructor", &Base::s_info, &JSreadonlyConstructorTable, 0, CREATE_METHOD_TABLE(JSreadonlyConstructor) };

JSreadonlyConstructor::JSreadonlyConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSreadonlyConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSreadonlyPrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSreadonlyConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSreadonlyConstructor, JSDOMWrapper>(exec, JSreadonlyConstructorTable, jsCast<JSreadonlyConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSreadonlyPrototypeTableValues[] =
{
    { 0, 0, NoIntrinsic, 0, 0 }
};

static const HashTable JSreadonlyPrototypeTable = { 1, 0, JSreadonlyPrototypeTableValues, 0 };
const ClassInfo JSreadonlyPrototype::s_info = { "readonlyPrototype", &Base::s_info, &JSreadonlyPrototypeTable, 0, CREATE_METHOD_TABLE(JSreadonlyPrototype) };

JSObject* JSreadonlyPrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSreadonly>(vm, globalObject);
}

const ClassInfo JSreadonly::s_info = { "readonly", &Base::s_info, &JSreadonlyTable, 0 , CREATE_METHOD_TABLE(JSreadonly) };

JSreadonly::JSreadonly(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<readonly> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSreadonly::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSreadonly::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSreadonlyPrototype::create(vm, globalObject, JSreadonlyPrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSreadonly::destroy(JSC::JSCell* cell)
{
    JSreadonly* thisObject = static_cast<JSreadonly*>(cell);
    thisObject->JSreadonly::~JSreadonly();
}

JSreadonly::~JSreadonly()
{
    releaseImplIfNotNull();
}

bool JSreadonly::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSreadonly* thisObject = jsCast<JSreadonly*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return getStaticValueSlot<JSreadonly, Base>(exec, JSreadonlyTable, thisObject, propertyName, slot);
}

EncodedJSValue jsreadonlyConstructor(ExecState* exec, EncodedJSValue thisValue, EncodedJSValue, PropertyName)
{
    JSreadonly* domObject = jsDynamicCast<JSreadonly*>(JSValue::decode(thisValue));
    if (!domObject)
        return throwVMTypeError(exec);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSreadonly::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSreadonly::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSreadonlyConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

bool JSreadonlyOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    return false;
}

void JSreadonlyOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSreadonly* jsreadonly = jsCast<JSreadonly*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsreadonly->impl(), jsreadonly);
    jsreadonly->releaseImpl();
}

JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, readonly* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSreadonly>(exec, impl))
        return result;
#if COMPILER(CLANG)
    // If you hit this failure the interface definition has the ImplementationLacksVTable
    // attribute. You should remove that attribute. If the class has subclasses
    // that may be passed through this toJS() function you should use the SkipVTableValidation
    // attribute to readonly.
    COMPILE_ASSERT(!__is_polymorphic(readonly), readonly_is_polymorphic_but_idl_claims_not_to_be);
#endif
    ReportMemoryCost<readonly>::reportMemoryCost(exec, impl);
    return createNewWrapper<JSreadonly>(exec, globalObject, impl);
}

readonly* toreadonly(JSC::JSValue value)
{
    return value.inherits(JSreadonly::info()) ? &jsCast<JSreadonly*>(value)->impl() : 0;
}

}
