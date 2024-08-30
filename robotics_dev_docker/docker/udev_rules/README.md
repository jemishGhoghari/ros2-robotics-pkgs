## Joystick
For the ros2 joy package running inside docker to recognize a specific usb device 
we need to create a udev rule on the host.

1. Check which device we're trying to connect.
```
sudo evtest
```
2. Check the name of the device (e.g. **/dev/input/event11**)
```
udevadm info -a -n /dev/input/event11
```
3. Create a new udev rule from the example and copy the name from ATTRS{name}.
```
sudo cp 99-*.rules /etc/udev/rules.d/
```
4. Reload udev rules
```
udevadm control --reload-rules
```
5. All you need to do now is to map **/dev/input** to the docker container. `run_dev.sh` from **isaac_ros_common** will already do this for you.

## GPIO

1. Create a gpio group.
```
sudo groupadd gpio
sudo usermod -a -G gpio $USER
```
2. Copy the udev rule.
```
sudo cp 60-gpiod.rules /etc/udev/rules.d/
udevadm control --reload-rules
```
3. Map **/dev/gpiochip[1-4]** to the docker container. `run_dev.sh` from **isaac_ros_common** will already do this for you.