#ifndef EVENTS_H
#define EVENTS_H
#include <unordered_map>
#include <functional>
#include <deque>
#include <memory>


template <class T>
class Observable
{
public:
    using Listener = std::function<void(const T&)>;
    int subscribe(const Listener& listener){
        listeners[listeners.size()] = listener;
        return listeners.size()-1;
    }
    void unsubscribe(int subscription){
        listeners.erase(subscription);
    }
protected:
    std::unordered_map<int, Listener> listeners;
};


template <class T>
class EventEmitter: public Observable<T>
{
public:
     void emit(const T& event){
        for (const auto& pair: Observable<T>::listeners) {
            pair.second(event);
        }
    }
};





#endif // EVENTS_H
