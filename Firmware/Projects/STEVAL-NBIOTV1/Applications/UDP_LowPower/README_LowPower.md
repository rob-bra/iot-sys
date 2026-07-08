
# UDP-Demo - Low-Power optimization


| FW version                                                | Current  |
|-----------------------------------------------------------|----------|
| Sleep (WFI)                                               | ~12 mA   |
| STOP2                                                     | ~300 µA  |
| STOP2, no Wakeup                                          | ~87 µA   |
| STOP2, no Wakeup, analog pins                             | ~86 µA   |
| STOP2, no Wakeup, analog pins, DBG Off                    | ~86 µA   |
| STOP2, no Wakeup, analog pins, DBG Off, no RTC            | ~83 µA   |
| *Dedicated "low-power only" FW – for comparison*          | ~30 µA   |