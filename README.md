# Introduction
Data Acquisition System realized for the prototype [Escorpio](https://www.facebook.com/TeamZeroCEscorpioEvo/?fref=ts) in 2015.

The DAQ System was realized to get real-time information from the prototype Escorpio during its races and visualize the data on a PC through
charts and widgets.

In the **HW** folder are placed the electronics boards realized with eagle. All the boards where controlled by an Arduino Due and placed inside 
the prototype.

In the **SW** folder are placed the software sources file:
- the Arduino Due software was in charge of collecting all the prototype data through the [Can Bus](https://en.wikipedia.org/wiki/CAN_bus) inside Escorpio and then to send the data 
to a remote PC using a [GSM SIM 900 module](http://www.geeetech.com/wiki/index.php/Arduino_GPRS_Shield);
- the remote PC was connected to another GSM SIM 900 module via USB and the software was in charge of controlling it, parse all the data 
coming from the other module and show it with charts and widgets. All the visualization views are completely personalizable thanks to the
user interface. All the data received is also saved in a local SQLite database.

All the code is C++, using [Qt Library](https://www.qt.io/developers/) for the PC software.
