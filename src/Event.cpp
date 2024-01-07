// Event.cpp

#include "Event.h"

// Add a listener to the event
template <typename... Args>
void Event<Args...>::AddListener(const EventHandler& listener) {
    listeners.push_back(listener);
}

// Remove a specific listener from the event
template <typename... Args>
void Event<Args...>::RemoveListener(const EventHandler& listener) {
    auto it = std::remove_if(listeners.begin(), listeners.end(), [&](const EventHandler& elem) {
        return elem.target_type() == listener.target_type() &&
               elem.template target<std::function<void(Args...)>>() == listener.template target<std::function<void(Args...)>>();
    });

    listeners.erase(it, listeners.end());
}

// Remove all listeners from the event
template <typename... Args>
void Event<Args...>::RemoveAllListeners() {
    listeners.clear();
}

// Invoke the event, calling all subscribed functions with parameters
template <typename... Args>
void Event<Args...>::Invoke(Args... args) {
    if (listeners.empty()) {
        return;  // No listeners, nothing to invoke
    }

    for (const EventHandler& listener : listeners) {
        listener(args...);
    }
}

// Add a listener to the event (specialization for void events)
void Event<void>::AddListener(const EventHandler& listener) {
    listeners.push_back(listener);
}

// Remove a specific listener from the event (specialization for void events)
void Event<void>::RemoveListener(const EventHandler& listener) {
    auto it = std::remove_if(listeners.begin(), listeners.end(), [&](const EventHandler& elem) {
        return elem.target_type() == listener.target_type() &&
               elem.template target<std::function<void()>>() == listener.template target<std::function<void()>>();
    });

    listeners.erase(it, listeners.end());
}

// Remove all listeners from the event (specialization for void events)
void Event<void>::RemoveAllListeners() {
    listeners.clear();
}

// Invoke the event, calling all subscribed functions with no parameters (specialization for void events)
void Event<void>::Invoke() {
    if (listeners.empty()) {
        return;  // No listeners, nothing to invoke
    }

    for (const EventHandler& listener : listeners) {
        listener();
    }
}

// Explicit instantiation for common parameter types
template class Event<int>;
template class Event<const std::string&>;
// Add more explicit instantiations as needed for other parameter types
