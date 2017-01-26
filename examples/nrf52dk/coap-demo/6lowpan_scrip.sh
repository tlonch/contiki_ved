# Log in as a root user.
sudo su

# Mount debugfs file system.
mount -t debugfs none /sys/kernel/debug

# Load 6LoWPAN module.
modprobe bluetooth_6lowpan

# Enable the bluetooth 6lowpan module.
echo 1 > /sys/kernel/debug/bluetooth/6lowpan_enable

# Look for available HCI devices.
hciconfig

echo test1

# Reset HCI device - for example hci0 device.
hciconfig hci0 reset

# Read 00:AA:BB:XX:YY:ZZ address of the nRF5x device.
hcitool lescan

echo 6LoWPAN module enable

exit 0