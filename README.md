# ZF_JmsDecoder
This is a very simple example on how to decode the axis values of a ZF JMS Joystick, from a Linux machine

The ZF JMS (Joystick Maneuvering System) are a joysticks for motor yachts

The linux machine is assumed to be connected to the CANBus through a SocketCAN compatible USB-to-CAN Adapter

The initial setup of the CAN interface must be done as such:

```
sudo ip link set can0 type can bitrate 500000
sudo ip link set up can0
```
This code was produced by reverse-engineering the CAN frames sent by the CAN Joystick, since the 
manufacturer did not want to provide information about the protocol.

The joystick sends the frames at a rate of 2Hz, and emits an sound alarm, 30 seconds after power up.
There is not documentation on how to acknowledge that alarm, or how to prevent it to trigger.
It is still not known if it is possible to raise the frames frequency, to gain a better user experience.



