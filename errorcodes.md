| Error Code  | Description | Process       |
| :---        |    :----:   |          ---: |
| 0      | Error, too many commands       | ParseInput   |
| 1   | Bad command        | ParseInput      |
| 2   | Bad number, must be an integer value        | ParseInput      |
| 3   | Bad LED parameters        | ProcessCmd      |
| 4   | Bad parameter for RGB color set        | ProcessCmd      |
| 5   | RGB values must be between 0-255        | ProcessCmd      |
| 6   | Bad Temp parameters        | ProcessCmd      |
| 7   | Bad Add parameters        | ProcessCmd      |
| 8   | Error, delay cannot exceed 10000ms        | ProcessCmd      |
| 9   | Bad Parameter for Set Blink        | ProcessCmd      |
| 10   | Bad parameter for Year        | ProcessCmd      |
| 11   | Bad parameter for Month        | ProcessCmd      |
| 12   | Bad parameter for Day        | ProcessCmd      |
| 13   | Bad parameter for Hour        | ProcessCmd      |
| 14   | Bad parameter for Minute        | ProcessCmd      |
| 15  | Bad parameter for Second        | ProcessCmd      |
| 16   | Year must be 0-199        | ProcessCmd      |
| 17   | Month must be 1-12        | ProcessCmd      |
| 18   | Day must be 1-31        | ProcessCmd      |
| 19   | Hour must be 0-23        | ProcessCmd      |
| 20   | Min must be 0-59        | ProcessCmd       |
| 21   | Sec must be 0-59        | ProcessCmd |
| 22   | Bad parameter for SET        | ProcessCmd |
| 23   | Bad parameter for status        | ProcessCmd |
| 24   | Not viable commands        | ProcessCmd |
| 25   | Ethernet Cable Unplugged        | a_udp |
| 26   | Bad parameter for Day of Week        | ProcessCmd |
| 27   | Dow must be 1-7       | ProcessCmd |