# David-3D-laserscanner-automitive-scan-program
David-3D-laserscanner-automitive-scan-program-use-arduino Uno

/////////////////////////////////////////////////////////////////
//Arduino Stepper Motor skecth listening to COM Port
//for use with the A4988 and DAVID-laserscanner to automatic scan
/////////////////////////////////////////////////////////////////
//modified by Dark 07.2015
//this is arduino Uno program the run with David 3D laserscanner for automatic scan
//David 3D laserscanner version :v3.10.4
//David 3D laserscanner use the Motorized Laser Setup
//arduino IDE version:1.0.5-r2
//more details http://tieba.baidu.com/p/3916238768?pid=72116606973&cid=0#72116606973
//any problems https://github.com/Dark-Guan/David-3D-laserscanner-automitive-scan-program/tree/4c9ae7f6e2a4a8083a65434cdbd340a0bcc05c8b
//Added: One key starts a whole cicle scan
//Added: Motor disenable when there is no cmd for 10 seconds
//Added: Set initial point function
//Added: Set the scan range
//Added: Switch on or off the laser
//Added: Switch on or off the light

/////////////////////
//funition available 
////////////////////
//1.Manual Mode:Connected to David or Send commands through a serial debugger
/////////////////////////////////////////////////////////////////////////////

//a.Receive "O"(David 3D default setting) to initial the laser stepper
//  laser stepper go up hit the end switch then go down to initialPoint
#define initialPoint  36000 //measure form the end switch to the initialPoint
//b.Receive "l"(David 3D default setting) to turn off the laser
//c.Receive "L"(David 3D default setting) to turn on the laser
//d.Receive "d"(David 3D not support default) to turn off the light
//e.Receive "D"(David 3D not support default) to turn on the light
//f.Receive "t"(David 3D not support default) to turn the turnalbe stepper manualTurnSteps steps
#define manualTurnSteps 200 //set the manual turn turnalble stepper
//j.Receive "0"(David 3D default setting) to set the initial point 
//h.Receive "P"(David 3D default setting) so go to the initial point that "0" command set 
    //or the default initial point
//i.Receive "-"(David 3D default setting) ,laser stepper to go down 100 steps 
//j.Receive "+"(David 3D default setting) ,laser stepper to go up 100 steps
//k.Receive "mxxxx"(David 3D default setting),for example,"m1000","m-1000","m+1000"
    //"m1000" is same as "m+1000",they all get the laser stepper go up 1000 steps
    //and "m-1000" get the laser stepper go down 1000 steps

//2.Auto Mode:Connected to David 
///////////////////////////////////
//Before use the Auto Mode,
//you should choose Motorized Laser Setup,connect the serial port of your Arduino Uno
//trigger Enable Texturing on Hardware check box setup tab
//and cancel the Align to previous Scan check box.on Texturing tab


//How to enter the Auto Mode?
//Trigger the button (or connect pin 7 to ground for a while )

//a.only "mxxx"set by david works
#define scanTime  17 //scan for scanTime times and one time overlap
#define scanRange 25000 //set the scan to scanRange steps
