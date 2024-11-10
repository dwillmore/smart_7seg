# smart_7seg
Smart 7 segment display driver board

This board carries two 2 digit 7 segment displays and multiplexes them to display up to four characters.
Input is serial ASCII at 115200 baud.  The board will display the first four characters it sees after a newline or carrage return.  It will then pass through any remaining characters until the next carrage return or newline.  This allows boards to be cascaded from left to right to add more digits to the display in a manner that is invisible to the user.

The board is based off of the WCH CH32V003 chip in a TSSOP20 pin package.  It has an 0604 1uF capacitor as a decoupling cap and four AO3416 mosfets for the digit drivers.  It is designed to run at 3.3V and works with standard red and green displays.  It may also work with modern GaN displays, but that has not been tested.

The boards were fabricated by JLCPCB and designed with KiCAD 8.0.
