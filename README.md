# Shinx SU7

## Proto for Bluetooth

control code -> data -> ack -> control code -> data -> ack -> ...

control code send by LeiJun, ack send by SU7, data can be sent by anyone.

control code: feature discribe, SU7 <-> LeiJun, data length

- 0x00: greeting, asking data -> 0byte
- 0x01: set control mode -> 0byte
- 0x02: set waypoint mode -> 0byte
- 0x03: set auto avoid mode -> 0byte
- 0x04: set auto race mode -> 0byte

- 0x10: set all wheel control <- 4byte
- 0x11: set left1 wheel control <- 1byte
- 0x12: set left2 wheel control <- 1byte
- 0x13: set right1 wheel control <- 1byte
- 0x14: set right2 wheel control <- 1byte

- 0x20: set waypoint <- n byte, 0xff for end
- 0x21: set waypoint follow start -> 0byte

- 0x30: set auto avoid mode position <- 2byte
- 0x31: set auto avoid mode start -> 0byte

- 0x80: get obstacle distance -> 4byte
- 0x81: get obstacle position -> 1byte

- 0x90: get SU7 position -> 1byte
- 0x9x: get SU7 state TODO


ack code:

- 0x00: ack
- 0xf0: out of range
- 0xff: nack

- 0x80 ~ 0xef: Su7 need send data, after LeiJun receive this, send this as control code to fetch data.