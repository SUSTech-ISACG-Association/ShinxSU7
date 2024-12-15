# Shinx SU7

## Proto for Bluetooth

Control Code -> (ACK1 -> DATA ->) ACK2 -> Control Code -> (ACK1 -> DATA ->) ACK2 -> ...

If no data sent, skip ACK1 and DATA transmit.

Control Code sent by LeiJun, ACK1/2 sent by SU7, DATA can be sent by both.

| Control Code | SU7 <-> LeiJun               | Data Length            |
| ------------ | ---------------------------- | ---------------------- |
| 0x00         | Greeting, asking data        | 0byte                  |
| 0x01         | Set control mode             | 0byte                  |
| 0x02         | Set waypoint mode            | 0byte                  |
| 0x03         | Set auto avoid mode          | 0byte                  |
| 0x04         | Set auto race mode           | 0byte                  |
| 0x10         | Set all wheel control        | 4byte                  |
| 0x11         | Set left1 wheel control      | 1byte                  |
| 0x12         | Set left2 wheel control      | 1byte                  |
| 0x13         | Set right1 wheel control     | 1byte                  |
| 0x14         | Set right2 wheel control     | 1byte                  |
| 0x20         | Set waypoint                 | n byte, `0xFF` for end |
| 0x21         | Set waypoint follow start    | 0byte                  |
| 0x30         | Set auto avoid mode position | 2byte                  |
| 0x31         | Set auto avoid mode start    | 0byte                  |
| 0x80         | Get obstacle distance        | 4byte                  |
| 0x90         | Get obstacle position        | 1byte                  |
| 0x9x         | Get SU7 state                | Reserved               |

ACK Code:

| ACK1/2 Code | Function     |
| ----------- | ------------ |
| 0x00        | ACK          |
| 0xF0        | Out of range |
| 0xFF        | NACK         |

| ACK2 Code | Function                                                     |
| --------- | ------------------------------------------------------------ |
| 0x80~0xEF | Su7 need send data, after LeiJun receive this, send this as control code to fetch data. |
