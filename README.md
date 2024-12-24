## ShinxSU7

This project involves the design and development of a **STM32-based smart vehicle** that can be controlled manually or autonomously using Bluetooth communication. The vehicle is equipped with ultrasonic sensors for obstacle detection, and it can navigate various paths using Waypoint and Auto modes. The project is based on an innovative communication protocol, system control codes, and advanced sensor calibration to ensure optimal performance.

The vehicle supports various motion tasks and can be controlled in different operational modes: Manual, Waypoint, Auto, and Race.

### Contributors

| Name                                            | Contributions                                                |
| ----------------------------------------------- | ------------------------------------------------------------ |
| [IskXCr](https://github.com/IskXCr)             | Vehicle-side autopilot and calibration design and implementation |
| [ShadowStorm](https://github.com/Jayfeather233) | Driver migration, protocol design and framework implementation |
| [Chanbengz](https://github.com/chanbengz)       | Controller- and vehicle-side Bluetooth interaction, documentation |
| [GuTaoZi](https://github.com/GuTaoZi)           | Controller-side design, implementation and protocol definition |

### Key Features:

- **Bluetooth control** for manual and autonomous driving
- **Auto obstacle detection** using ultrasonic sensors
- **Waypoint navigation** and **Auto mode** with obstacle avoidance
- **Advanced calibration** algorithms for improved motion accuracy

### System Functionality

#### Feat 1: Controller and Vehicle Connection

**Bluetooth Communication**: Connects the remote controller (*LeiJun*) and vehicle (*SU7*). If the handshake times out, an error message is displayed.

**Vehicle Movements**: The vehicle can move forward, backward, rotate, and stop in multiple directions.

#### Feat 2: Obstacle Detection

**Real-time Distance Display**: The vehicle detects obstacles using ultrasonic sensors, displaying distance readings on the screen.

#### Feat 3: Waypoint Mode

**Grid-Based Navigation**: The ground is divided into a 4x4 grid, where obstacles are set manually. The vehicle follows predefined waypoints.

**Route Setting**: Users can set, modify, and send routes to the vehicle.

#### Feat 4: Auto Mode

**Obstacle Detection in Auto Mode**: The vehicle autonomously detects obstacles and selects the optimal route to reach the destination using a breadth-first search algorithm.

#### Feat 5: Race Mode

**Line Tracking**: The vehicle can track guide lines using grayscale sensors for autonomous movement in Race mode.

### Communication Protocol Design

The Bluetooth communication between the controller (*LeiJun*) and the vehicle (*SU7*) follows a cyclic protocol where control codes are sent from LeiJun, SU7 responds with acknowledgment codes, data can be sent in both directions.

The controller sends greeting messages every  ⅛ second, consuming the message buffer of the vehicle.

A typical communication workflow:

1. The controller sends a control code to the vehicle;
2. The vehicle responds an ACK code;
   - if no data transmission needed, enter the next cycle.
3. Data transmission between the controller and the vehicle;
4. The vehicle sends another ACK, communication completed.

| Control Code | Descriptions                                    |                      Data Length                       |
| :----------: | ----------------------------------------------- | :----------------------------------------------------: |
|     0x00     | Greeting, check if any data needs to be fetched |                         0 byte                         |
|     0x01     | Set to control mode                             |                         0 byte                         |
|     0x02     | Set to waypoint mode                            |                         0 byte                         |
|     0x03     | Set to autopilot mode                           |                         0 byte                         |
|     0x04     | Set to auto race mode                           |                         0 byte                         |
|     0x05     | Set mode start                                  |                         0 byte                         |
|     0x06     | Set mode STOP                                   |                         0 byte                         |
|     0x10     | Set all wheel control                           |                 4 bytes (4 \* uint8_t)                 |
|     0x11     | Set left1 wheel control                         |                    1 byte (uint8_t)                    |
|     0x12     | Set left2 wheel control                         |                    1 byte (uint8_t)                    |
|     0x13     | Set right1 wheel control                        |                    1 byte (uint8_t)                    |
|     0x14     | Set right2 wheel control                        |                    1 byte (uint8_t)                    |
|     0x20     | Set a route with $n$ waypoints                  |              $n+1$ bytes, end with `0xFF`              |
|     0x30     | Set autopilot starting and ending point         |                 2 bytes (2 \* uint8_t)                 |
|     0x80     | Get obstacle distance                           |                    4 bytes (float)                     |
|     0x81     | Get obstacle position                           |                    1 byte (uint8_t)                    |
|     0x90     | Get SU7 position                                | 1 byte (2 padding \| 2 facing direction \| 4 position) |
|     0x9x     | Get SU7 state                                   |                        Reserved                        |

| ACK Code  | Descriptions                                                 |
| --------- | ------------------------------------------------------------ |
| 0x00      | ACK OK                                                       |
| 0x80~0xEF | Sent by the vehicle, notifying the controller to fetch data from the vehicle by echo this code |
| 0xF0~0xFF | Error codes                                                  |
| 0xF0      | Out of range                                                 |
| 0xF1      | set mode failed, wait for end or STOP                        |
| 0xFF      | NACK                                                         |

### Usage

#### Requirements

STM32F103RCT6-based vehicle (SU7), Bluetooth module, ultrasonic sensors, and a controller (LeiJun), compiled using **STM32Cube IDE v.1.17.0**.

#### Running the Vehicle

1. **Controller Setup**:
   - Power on the vehicle and controller.
   - Ensure Bluetooth communication is established.
   - Switch to the desired operational mode (Manual, Waypoint, Auto, or Race) using wake up key.
2. **Manual Mode**:
   - Press the buttons on the touch panel of the controller to drive the vehicle in all directions.
3. **Waypoint Mode**:
   - Set obstacles on the grid and define waypoints. Click "Go" to start the vehicle.
4. **Auto Mode**:
   - Set the start and end location. The vehicle will autonomously navigate the grid, avoiding obstacles.
5. **Race Mode**:
   - Switch to Race mode to enable line-following behavior for the vehicle.

------

For more details, please refer to the `doc/` directory and the source code repository.
