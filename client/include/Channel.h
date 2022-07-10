#ifndef CHANNEL_H
#define CHANNEL_H
#include <string>
#include <memory>
#include <deque>
#include <mutex>

template <class T>
class restricted_queue : protected std::deque<T> {
public:

    T pull(){
      const std::lock_guard<std::mutex> lock(mutex);
      auto return_value = std::deque<T>::front();
      std::deque<T>::pop_front();
      return return_value;
    }

    bool is_empty() const {
        return std::deque<T>::empty();
    }

    void push(const T& elem) {
        const std::lock_guard<std::mutex> lock(mutex);
        std::deque<T>::push_back(elem);
    }

private:
    std::mutex mutex;
};

struct message_channel {
    restricted_queue<std::string> write_queue;
    restricted_queue<std::string> read_queue;
};

#endif // CHANNEL_H
