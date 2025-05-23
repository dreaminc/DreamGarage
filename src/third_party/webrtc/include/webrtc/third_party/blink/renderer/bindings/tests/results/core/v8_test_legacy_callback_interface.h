// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file has been auto-generated from the Jinja2 template
// third_party/blink/renderer/bindings/templates/callback_interface.h.tmpl
// by the script code_generator_v8.py.
// DO NOT MODIFY!

// clang-format off

#ifndef V8TestLegacyCallbackInterface_h
#define V8TestLegacyCallbackInterface_h

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/bindings/callback_interface_base.h"
#include "third_party/blink/renderer/platform/bindings/dom_wrapper_world.h"
#include "third_party/blink/renderer/platform/bindings/wrapper_type_info.h"

namespace blink {

class Node;

class CORE_EXPORT V8TestLegacyCallbackInterface final : public CallbackInterfaceBase {
 public:
  // Support of "legacy callback interface"
  static v8::Local<v8::FunctionTemplate> DomTemplate(v8::Isolate*, const DOMWrapperWorld&);
  static const WrapperTypeInfo wrapperTypeInfo;
  // Constants
  static constexpr uint16_t CONST_VALUE_USHORT_42 = 42;

  static V8TestLegacyCallbackInterface* Create(v8::Local<v8::Object> callback_object) {
    return new V8TestLegacyCallbackInterface(callback_object);
  }

  ~V8TestLegacyCallbackInterface() override = default;

  // TraceWrapperBase overrides:
  const char* NameInHeapSnapshot() const override;

  // Performs "call a user object's operation".
  // https://heycam.github.io/webidl/#call-a-user-objects-operation
  v8::Maybe<uint16_t> acceptNode(ScriptWrappable* callback_this_value, Node* node) WARN_UNUSED_RESULT;

 private:
  explicit V8TestLegacyCallbackInterface(v8::Local<v8::Object> callback_object)
      : CallbackInterfaceBase(callback_object, kSingleOperation) {}
};

template <>
class CORE_TEMPLATE_CLASS_EXPORT V8PersistentCallbackInterface<V8TestLegacyCallbackInterface> final : public V8PersistentCallbackInterfaceBase {
  using V8CallbackInterface = V8TestLegacyCallbackInterface;

 public:
  ~V8PersistentCallbackInterface() override = default;

  CORE_EXTERN_TEMPLATE_EXPORT
  v8::Maybe<uint16_t> acceptNode(ScriptWrappable* callback_this_value, Node* node) WARN_UNUSED_RESULT;

 private:
  explicit V8PersistentCallbackInterface(V8CallbackInterface* callback_interface)
      : V8PersistentCallbackInterfaceBase(callback_interface) {}

  V8CallbackInterface* Proxy() {
    return As<V8CallbackInterface>();
  }

  template <typename V8CallbackInterface>
  friend V8PersistentCallbackInterface<V8CallbackInterface>*
  ToV8PersistentCallbackInterface(V8CallbackInterface*);
};

// V8TestLegacyCallbackInterface is designed to be used with wrapper-tracing.
// As blink::Persistent does not perform wrapper-tracing, use of
// |WrapPersistent| for callback interfaces is likely (if not always) misuse.
// Thus, this code prohibits such a use case. The call sites should explicitly
// use WrapPersistent(V8PersistentCallbackInterface<T>*).
Persistent<V8TestLegacyCallbackInterface> WrapPersistent(V8TestLegacyCallbackInterface*) = delete;

}  // namespace blink

#endif  // V8TestLegacyCallbackInterface_h
