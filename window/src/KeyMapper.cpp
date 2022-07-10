#include "KeyMapper.h"
#include <vector>
#include <memory>
#include <list>
#include <iostream>
#include "io.h"
#include <unordered_map>

struct KeyMapper::KeyMapperImpl {

    struct KeyEventHashFunction {
      size_t operator()(const KeyEvent& event) const {
        size_t rowHash = std::hash<int>()(event.key);
        size_t colHash = std::hash<int>()(event.state) << 1;
        return rowHash ^ colHash;
      }
    };
    template<typename T>
    using HashedKeyMap = std::unordered_map<KeyEvent, T, KeyEventHashFunction>;

    HashedKeyMap<std::string> keyToMsg;

    std::string createMessage(const KeyEvent& event) const {
        if (keyToMsg.count(event) == 0)
            return std::string();
        return keyToMsg.at(event);
    }

    static HashedKeyMap<std::string> createKeyToMsgMap(){
        HashedKeyMap<std::string> keyToMsg;
        keyToMsg[KeyEvent{ActionKey_t::StartT, State_t::Pressed}] = "t1";
        keyToMsg[KeyEvent{ActionKey_t::StartX, State_t::Pressed}] = "x1";

        keyToMsg[KeyEvent{ActionKey_t::Up, State_t::Released}] = "w0";
        keyToMsg[KeyEvent{ActionKey_t::Down, State_t::Released}] = "s0";
        keyToMsg[KeyEvent{ActionKey_t::Left, State_t::Released}] = "a0";
        keyToMsg[KeyEvent{ActionKey_t::Right, State_t::Released}] = "d0";
        keyToMsg[KeyEvent{ActionKey_t::Fire, State_t::Released}] = "k0";
        keyToMsg[KeyEvent{ActionKey_t::Up, State_t::Pressed}] = "w1";
        keyToMsg[KeyEvent{ActionKey_t::Down, State_t::Pressed}] = "s1";
        keyToMsg[KeyEvent{ActionKey_t::Left, State_t::Pressed}] = "a1";
        keyToMsg[KeyEvent{ActionKey_t::Right, State_t::Pressed}] = "d1";
        keyToMsg[KeyEvent{ActionKey_t::Fire, State_t::Pressed}] = "k1";
        return keyToMsg;
    }

    KeyMapperImpl(){
        keyToMsg = createKeyToMsgMap();
    }
};

KeyMapper::KeyMapper(): impl(new KeyMapperImpl()){}

std::string KeyMapper::mapKey(const KeyEvent& event) const {
    return impl->createMessage(event);
}



