//
// Created by Salih on 06-Jan-24.
//

#include <functional>
#include <vector>
#include <string>

#ifndef SRC_EVENT_H
#define SRC_EVENT_H




// Define a template for event handlers with parameters
template <typename... Args>
class Event {
public:
    // Define a function signature for event handlers
    using EventHandler = std::function<void(Args...)>;

    // Add a listener to the event
    void AddListener(const EventHandler& listener);

    // Remove a specific listener from the event
    void RemoveListener(const EventHandler& listener);

    // Remove all listeners from the event
    void RemoveAllListeners();

    // Invoke the event, calling all subscribed functions with parameters
    void Invoke(Args... args);

private:
    std::vector<EventHandler> listeners;
};

#endif //SRC_EVENT_H
