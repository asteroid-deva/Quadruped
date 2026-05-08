# Serial Communication Protocol

The Raspberry Pi communicates with the ESP32 using serial USB communication.

## Supported Commands

| Command | Function |
|---|---|
| SIT | Move robot into sitting pose |
| STAND | Move robot into standing pose |
| WAVE | Trigger waving animation |

## Example

```python
esp32.write(b"STAND\n")
```
the pi will be connectd to the same wifi network as the laptop in which we will be opening the terminal to run the command to start the gesture_control.py
```
python3 gesture_control.py
```
