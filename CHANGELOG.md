# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.0.3] - 2023-05-23

### Added

#### Protocol

- READY now sends the teensy's UID64

### Changed

- Updated mkshft_ctrl send function to safely switch between Serial and SLIP depending on connection status

### Fixes

- Unfucked version 0.0.2's segfault issue

## [0.0.2] - 2023-05-18

**This version does not work, segfaults due to an overflow issues**


#### Protocol

- STATE_UPDATE packet now contains the relative dial state in addition to absolute dial state - enables makeshift-serial to emit increment/decrement events even when absolute dial state hits the min/max values

  - new protocol: 
  ```
  Size: 23 bytes
   
  [0|1] [2|3|4|5] [6|7|8|9| 10|11|12|13| 14|15|16|17| 18|19|20|21] [22]
   |
   |-> 0, 1 - binary encoded button states for buttons 0 ~ 15 (4 dials, 12 switches)
   |
   |-> 2 ~ 5 - relative dial changes for dials 0 ~ 3 
   |           (i.e. the amount increased/decreased since last update)
   |
   |-> 6 ~ 21 - absolute dial states for dials 0 ~ 3
   |
   |-> 22 - endline
  ```