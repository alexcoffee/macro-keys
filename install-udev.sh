echo "Updating udev rules to allow read permissions to Raspberry Pi Pico HID device."

sudo cp 99-pi-pico-permissions.rules /etc/udev/rules.d/

sudo udevadm control --reload-rules && sudo udevadm trigger
