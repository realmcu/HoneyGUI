
# Change Log



## [2.1.3] - 2022-03-11

### Changed

- Optimized  vos components.

  

## [2.1.2] - 2022-01-11

### Added

- The length of the name in advertising data can be extended to a maximum of 14 characters in V2 encryption mode.

### Changed

- Optimized encryption status reporting under the LINK layer pairing mechanism.



## [2.1.1] - 2021-12-16

### Added

- Added api for  bulk data request to read.

### Fixed

- Fixed the bug of unsuccessful compilation when VOS was disabled.



## [2.1.0] - 2021-12-03

### Added

- Added file sending (mobile app to device) channel.
- Added attachment file upgrade channel.
- Added support for scanning QR code to register and bind devices.
- Added support for working in the mandatory bluetooth LINK layer pairing and bonding environment.
- Added support for Alexa voice service.
- Added IOT channel, currently only supports the request and control of scene data.
- Added communication interfaces for other protocol expansion modules (currently only supports Tuya NBIoT modules).

### Fixed

- Fixed the bug that may cause memory leaks in certain scenarios in the weather component.



## [2.0.0] - 2021-04-24

### Added

- Added support for MTU larger than 20 bytes for adapting applications running BLE4.2 and above.
- Added a new dp data receiving and sending function, which can send and receive dp data with a length of more than 255 bytes, and supports the sending mode without response.
- Added device unbinding function, which is different from the factory reset function and will not lose cloud historical data after binding again.
- Added asynchronous response of unbinding and factory reset functions.
- Added support for BEACON KEY.
- Added support for uart-based test command channel.
- Added new API to query whether sleep is allowed.
- Added support for custom production test channel encryption.
- Added OEM capabilities.
- Added new interface to request the local time of the mobile phone.
- Added support for setting the device name in the Bluetooth advertisement (up to 5 bytes).
- Added support for working in the BLE LINK layer pairing and bonding environment.
- Added bulk data upload channel.
- Added interface for querying the weather.
- Added support for advanced encryption based on security chips.

### Changed

- Changed the queue scheduling mechanism under non-RTOS systems to prevent SDK from occupying CPU for a long time.
- Modify `TUYA_BLE_GATT_SEND_DATA_QUEUE_SIZE` to be automatically configured by the SDK.
- Modify the critical section interface definition.





## [1.2.4] - 2020-07-06

### Added

- Added support for transmitting character dp point data with length 0.
- Added support for Tuya v2 version protocol.
- Added support for using authorization information managed by the application in Bluetooth channel registration device configuration mode.

### Changed

- Changed the dynamic memory release logic in some event handlers, the previous method may cause the dynamic memory free space to become smaller in extreme scenarios.

### Fixed

- Fixed the bug that the single packet data instruction cannot be parsed.
- Fixed an issue that could not be compiled in the non-Bluetooth channel registered device configuration mode.
- Fixed the bug of test link failure based on Tuya protocol.



## [1.2.3] - 2020-05-19

### Added
- Added support for shared devices.
- Added dp point data reporting API with serial number and processing mode.

### Changed

- Clear the binding information when burning the authorization information, if it exists.




## [1.2.2] - 2020-04-23
### Added
- Added on-demand connection interface.
- Added the interface to get the total size, space and number of events of the scheduling queue under no OS environment.
- Added configuration item to configure whether to use the MAC address of tuya authorization information.
- Added tuya app log interface.

### Changed
- Changed the production test code structure, and remove the application layer header file to obtain the application version number, fingerprint and other information methods.
- Changed the encryption method of ble channel used in production test to support unencrypted transmission.

### Fixed
- Fix a problem that caused a compilation error in the SEGGER Embedded Studio environment.

## [1.2.1] - 2020-03-20
### Changed
- Optimized production test function, added data channel parameters


## [1.2.0] - 2020-03-06
### Added
- New platform memory management port.
- Added gatt send queue.
- New production test interface for customer-defined applications.
- Added tuya app log interface.

### Changed
- The maximum length of dp point array data that can be sent at one time increases to 258.
- Optimize RAM usage.
- Stripping uart general processing functions to customer applications.


## [1.1.0] -
### Added


### Changed


### Fixed

## [1.0.0] -
### Added


### Changed


### Fixed

## [0.3.0] -
### Added


### Changed


### Fixed


## [0.2.0] -
### Added


### Changed


### Fixed

