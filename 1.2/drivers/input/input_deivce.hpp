#ifndef INPUT_DEVICE_HPP
#define INPUT_DEVICE_HPP

class InputDevice {
public:
    virtual void init() = 0;            // installs the handler
    virtual void handleInterrupt(struct interrupt_register* regs) = 0; // interrupt service routine
    // virtual ~InputDevice() {};

};

#endif