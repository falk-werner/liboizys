#ifndef COM_LISTENER_I_HPP
#define COM_LISTENER_I_HPP

namespace com
{

class listener_i
{
public:
    virtual ~listener_i() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
};

}

#endif
