#pragma once
#include "EventHandler.h"

struct EventHandle
{
  EventHandle(uintptr_t _uid, const std::string& _eventName)
    : uid(_uid)
    , eventName(_eventName)
  {
  }

  const uintptr_t uid;
  const std::string eventName;
};

struct SinkObject
{
  SinkObject(const ::Sink* _sink, std::vector<std::string_view>& _linkedEvents)
    : sink(_sink)
    , linkedEvents(_linkedEvents)
  {
  }
  const ::Sink* sink;
  const std::vector<std::string_view> linkedEvents;
};

struct CallbackObject
{
  CallbackObject(const JsValue& _callback, bool _runOnce)
    : callback(_callback)
    , runOnce(_runOnce)
  {
  }

  JsValue callback;
  bool runOnce;
};

using CallbackObjMap =
  robin_hood::unordered_map<uintptr_t, std::unique_ptr<CallbackObject>>;

struct EventState
{
  explicit EventState(const SinkObject* _sink)
    : sinkObj(_sink)
  {
    callbacks.reserve(5);
  }

  const SinkObject* sinkObj;
  CallbackObjMap callbacks;
};

using EventMap = robin_hood::unordered_map<std::string_view, EventState*>;

/**
 * TODO: if we want to modularize platform further
 * we should not let EventManager use JsValue here
 * and instead store everything coming from JS side as
 * some generic objects
 *
 * this would require some refactored translation mechanism
 * native->generic->jsValue
 * jsValue->generic->native
 */
class EventManager
{
public:
  [[nodiscard]] static EventManager* GetSingleton()
  {
    static EventManager singleton;
    return &singleton;
  }

  static void Init();
  static void InitCustom();

  std::unique_ptr<EventHandle> Subscribe(const std::string& eventName,
                                         const JsValue& callback,
                                         bool runOnce);

  void Unsubscribe(uintptr_t uid, const std::string_view& eventName);

  void ClearCallbacks()
  {
    for (const auto& event : events) {
      event.second->callbacks.clear();
    }

    auto handler = EventHandler::GetSingleton();
    handler->DeactivateAllSinks();
  }

  CallbackObjMap* GetCallbackObjMap(const char* eventName)
  {
    auto event = events[eventName];

    if (!event) {
      return nullptr;
    }

    return &event->callbacks;
  }

  void EmplaceEvent(const std::string_view& name,
                    EventState* state = new EventState(nullptr))
  {
    events.emplace(name, state);
  }

  EventMap* GetEventMap() { return &events; }

private:
  // last i checked we had ~97 events
  EventManager() { events.reserve(100); }
  EventManager(const EventManager&) = delete;
  EventManager(EventManager&&) = delete;

  ~EventManager() = default;

  EventMap events;
};