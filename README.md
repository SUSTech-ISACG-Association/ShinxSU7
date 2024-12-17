# Shinx SU7

1.7s for 0.8m

1.53s for 360 deg

## Proto for Bluetooth

Control Code -> (ACK1 -> DATA ->) ACK2 -> Control Code -> (ACK1 -> DATA ->) ACK2 -> ...

If no data sent, skip ACK1 and DATA transmit.

Control Code sent by LeiJun, ACK1/2 sent by SU7, DATA can be sent by both.

| Control Code | SU7 <-> LeiJun               | Data Length            |
| ------------ | ---------------------------- | ---------------------- |
| 0x00         | Greeting, asking data        | 0byte                  |
| 0x01         | Set to control mode          | 0byte                  |
| 0x02         | Set to waypoint mode         | 0byte                  |
| 0x03         | Set to auto avoid mode       | 0byte                  |
| 0x04         | Set to auto race mode        | 0byte                  |
| 0x05         | Set mode start               | 0byte                  |
| 0x06         | Set mode STOP                | 0byte                  |
| 0x10         | Set all wheel control        | 4byte                  |
| 0x11         | Set left1 wheel control      | 1byte                  |
| 0x12         | Set left2 wheel control      | 1byte                  |
| 0x13         | Set right1 wheel control     | 1byte                  |
| 0x14         | Set right2 wheel control     | 1byte                  |
| 0x20         | Set waypoint                 | n byte, `0xFF` for end |
| 0x30         | Set auto avoid mode position | 2byte                  |
| 0x80         | Get obstacle distance        | 4byte                  |
| 0x81         | Get obstacle position        | 1byte                  |
| 0x9x         | Get SU7 state                | Reserved               |

ACK Code:

| ACK1/2 Code | Function                               |
| ----------- | -------------------------------------- |
| 0x00        | ACK                                    |
| 0xF0~0xFF   | Error codes                            |
| 0xF0        | Out of range                           |
| 0xF1        | set mode failed, wait for end or STOP  |
| 0xFF        | NACK                                   |

| ACK2 Code | Function                                                     |
| --------- | ------------------------------------------------------------ |
| 0x80~0xEF | Su7 need send data, after LeiJun receive this, send this as control code to fetch data. |
