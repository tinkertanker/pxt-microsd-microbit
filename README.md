how do i sdcard????

**important notes about connecting the SF breakout**

 - vcc -> vcc
 - gnd -> gnd
 - cmd -> MOSI (labelled MISO on the breakout)
 - D0 -> MISO (labelled MOSI on the breakout)
 - CD -> gnd
 - CLK -> SCLK
 - D3 -> CS (12/16)
 - D1, D2 and WP can be left floating.



the EF breakout probably won't work without stepping it up to 5V first, because there is an unbypassable 3V3 LDO on the VCC line.


known issues:

 - crashes after a couple of fwrites

 - there is apparently a memory leak in fclose() in certain mbedos versions on ARM



potential solutions:

 - use petit-fatfs to reduce footprint even further in memory (potential reason for fwrite crash with pxt)

 - ???
