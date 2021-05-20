#ifndef CHANNEL_H
#define CHANNEL_H
#include <string>

class message_channel {
public:
    virtual std::string read() =0;
    virtual bool has_data_to_read() const =0;
    virtual void write(const std::string& msg) =0;
};

#endif // CHANNEL_H
