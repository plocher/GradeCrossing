# GradeCrossing
## License: MIT License

Arduino Grade Crossing Controller

Initialize the hardware to use the following I/O pins:

   * digital pin 2:  Timed Detection Input: Use with an active low open-collector detection circuit on BOTH sides of grade crossing.
     A low on this input will trigger a 10 cycle timer whenever triggered‚Äî if you are only using a single detector,
     be sure to also connect this pin to digital pin 3, the "Road Blocked" detector.
   * digital pin 3:  Road Blocked Detection Input: "Always Flash" Detection‚Äî whenever this input is low, the flasher will continue to operate
   * digital pin 5:  Crossing LED
   * digital pin 6:  "Other" crossing LED
   * digital pin 13:  An optional; debug LED; this is usually visible on the Arduino board
