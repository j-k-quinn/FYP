# ZED X GMSL2 Drivers

The ZED X can currently only be used on Jetson. It communicates with the host with a serialized connection. The serializer is inside the camera and the deserializer needs to be added to the host. The driver is the configuration of those devices and therefore depends on the hardware used, including the jetson carrier board, the deserializer card, and other GMSL cameras configuration.

We provide a driver for some configurations, here's a list of the supported and unsupported configurations and some pointers to help troubleshoot issues.

Contact support@stereolabs.com if needed for help.


## Installation

Select the driver corresponding to your configuration and run :

```sh
sudo dpkg -i drivers/stereolabs-zedx-L4T35.1-vX.X.X_YYYYYYY.deb
```

The Jetson should then be rebooted. It's also important to note that the GMSL2 cameras are less flexible than USB cameras. **Any hardware configuration change** as in: plugging/unplugging a camera, changing camera order etc **requires either a jetson reboot or restarting the installed deamon** ;
```
sudo systemctl restart zed_x_daemon
```

## Case handled by the driver

- AGX devkit (Orin/Xavier), Xavier NX (ZEDBox)
- JP 5.0 / L4T 35.1
- JP 5.1 / L4T 35.2
- JP 5.1.1 / L4T 35.3
- Stereolabs GMSL capture card based on MAX96712 deserializer
- Leopard Imaging GMSL capture card based on MAX96712 deserializer
- Leopard Imaging GMSL capture card based on MAX9296 deserializer 

See here the supported configuration and status :
https://docs.google.com/spreadsheets/d/1ASLwgtGnGhY0u-WBsAj1pE7VNk6xd1Kx/edit#gid=1325365446


## Case not handled by the driver yet
 
- Other GMSL Cameras
- Custom Jetson module carrier boards that do not support Samtec CSI configuration
- Custom GMSL2 Deserializer


## Case for other GMSL cameras

- The compiled driver (deb file) is provided with a DTS that enumerates ZED-X camera on `i2c30` and `i2c31`. It will override any other configuration for cameras in custom DTS

To create a custom DTS, use the diff file provided by Stereolabs to merge both DTS together
You can also request access to the kernel github repository (private / under NDA)

## Troubleshooting commands

```sh
sudo dmesg | grep zedx
```

ZED Diagnostic tool dmesg option in admin:
```
sudo ./ZED_Diagnostic --dmesg
```


A ZED-X is made of 2 video sensors. Both must appear as “bound” in the dmesg log

Each ZED-X is linked to an i2c bus with `0x10` and `0x18` sub addresses, therefore the first ZED-X detected will be under the devices :

- `30-0018` (left) and `30-0010` (right)

The second ZED-X will be under : 

- `31-0018` (left) and `31-0010` (right)
