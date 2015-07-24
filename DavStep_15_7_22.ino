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

////////////////////////////////////////////////////////
//Arduino Stepper Motor skecth listening to COM Port
//for use with the EasyDriver 3.1 and DAVID-laserscanner
////////////////////////////////////////////////////////

// based on Dan Thompson 2008 http://danthompsonsblog.blogspot.com/
// modified by MagWeb 02.2009
// Added : Pushbutton to start a single circle or to stop it while it is running
// Added : Endswitch to get zero-value of motion
// Added : LED enlighted while motor moves forward
// Added : Potentiometer to controll motor speed by hand
// Added : second EasyDriver to controll a turntable
// For all the DAVID-laserscanner product details http://www.david-laserscanner.com

//Use this code at your own risk and have fun

// Pins
int LED = 11;           // names pin 11 as LED laser (placeholder for the laser)
int BUTTON = 7;        // names pin 7 as BUTTON 
int END = 8;           // names pin 8 as the endswitch
int LIGHT = 2;         // names pin 2 as the lightswitch //the program support light but won't control the light when run scan cycle
int dirpin = 3;        // names pin 3 as "dirpin" -Stepperdirection laser
int steppin = 4;      // names pin 12 as "steppin" -Steppersteps laser
int stepEnable = 9;      //step motor set enable
int ttstepEnable = 10;      //turnable plan step set enalble
int ttdir = 5;          // names pin 4 as "dirpin" -Stepperdirection turntable
int ttstep = 6;        // names pin 5 as "dirpin" -Steppersteps turntable

//variables
long range = scanRange; // Stores how many steps will be made in total -Stepper laser same as the scan range
int ttrange = manualTurnSteps; // Stores how many steps will be made in total -Stepper Turntable
int incomingByte = 0;	// for incoming serial data
int val = 0;           // stores the state of the BUTTON pin
int old_val = 0;   //stores the old state of the BUTTON pin
//if state ==0 the program is on the manual mode,else if state==1 the program is on the cycle scanning mode
int state = 0; //stores the state of the program 

long stepsRecord = 0;	//steps traveled from the init point
long stepsInit = initialPoint;	//line stepper moves form 0 to the set point default value
int turnCount = scanTime;	//model will turn turnCount times and scan 17 times


long lastMessageTime = 0;	//record the last message time
int slowSpeed = 1000; //slow travel speed
int fastSpeed = 6000; //fasr travel speed
int normalSpeed = 3000;//nomalSpeed

void enableMotors() {
  digitalWrite(stepEnable, HIGH);
  digitalWrite(ttstepEnable, HIGH);
}

void disenableMotors() {
  digitalWrite(stepEnable, LOW);
  digitalWrite(ttstepEnable, LOW);
  //Serial.println("disableMotors");
}
int speed2microseconds(int travalSpeed) {//speed to milliseconds function mm/min
  long microseconds = 1000 * 10;
  microseconds = microseconds / travalSpeed;
  microseconds = microseconds * 60;
  //Serial.println("microseconds is");
  //Serial.println(microseconds);
  return microseconds;
}

void moveSevaralSteps(boolean dir, long steps, int travalSpeed,
boolean isbypassEndStop) {    //laser move Sevaral Steps
  enableMotors();    //enable the step
  boolean isEnd = digitalRead(END);    //if end switsh is hitted
  if (isEnd && !isbypassEndStop) {    //||stepsRecord ==0)  
    // end switch hit then stop or //reach the initial point
    return;
  }
  int microseconds = speed2microseconds(travalSpeed);

  digitalWrite(dirpin, dir);
  int i;
  for (i = 0; i < steps; i++) {        // pulse
    digitalWrite(steppin, HIGH);     //   :
    delayMicroseconds(microseconds / 2);          //   :
    digitalWrite(steppin, LOW);      //   :
    delayMicroseconds(microseconds / 2);          // pulse

    if (dir) {          //record the steps have turned
      stepsRecord++;          //laser up stepsRecord minus one
    } 
    else {
      stepsRecord--;          //laser down  stepsRecord plus one
    }

    boolean isEnd = digitalRead(END);          //if end switsh is hitted
    if (isEnd && !isbypassEndStop) {          //||stepsRecord ==0)  
      // end switch hit then stop or //reach the initial point
      break;
    }
  }
  //Serial.println("stepsRecord");
  //Serial.println(stepsRecord);
}

void turnSevaralSteps(boolean dir, long steps, int travalSpeed) { //rurnalbe plan turn Sevaral Steps
  enableMotors();        //enable the ttstep
  digitalWrite(ttdir, dir);
  int microseconds = speed2microseconds(travalSpeed);
  int i;
  for (i = 0; i < steps; i++) {        // pulse
    digitalWrite(ttstep, HIGH);          //   :
    delayMicroseconds(microseconds / 2);              //   :
    digitalWrite(ttstep, LOW);           //   :
    delayMicroseconds(microseconds / 2);              // pulse

  }
}

void onOrOffLaser(boolean state) {       //ture for laser on;false for laser off
  digitalWrite(LED, state);
}

void onOrOffLight(boolean state) {        //turefor light on;false for laser off
  digitalWrite(LIGHT, state);
}

void goToInitialPoint() {   //return to the initial point ,need at last one home
  if (stepsRecord > 0) {
    moveSevaralSteps(false, stepsRecord, 1000, true);
  } 
  else {
    moveSevaralSteps(true, -stepsRecord, 1000, true);
  }
}

void setInitialPoint() {
  stepsRecord = 0;
}

void setup() {
  Serial.begin(115200);    // opens serial communication

  pinMode(dirpin, OUTPUT);  // Sets pins to in or out modes
  pinMode(steppin, OUTPUT); //
  pinMode(LED, OUTPUT);     //
  pinMode(BUTTON, INPUT);   //
  pinMode(LIGHT, OUTPUT);   //
  pinMode(END, INPUT);      //
  pinMode(ttdir, OUTPUT);   //
  pinMode(ttstep, OUTPUT);  //

  pinMode(stepEnable, OUTPUT);
  pinMode(ttstepEnable, OUTPUT);

  digitalWrite(BUTTON, HIGH);  //open the pull-up resistor
  digitalWrite(END, HIGH);  //open the pull-up resistor
}

void loop() {

  long Time = millis();  //record the runtime

  val = digitalRead(BUTTON);   // read Button input value and store it
  //from Low to HIGH charge the state
  if ((val == LOW) && (old_val == HIGH)) {
    state = 1 - state;
    //Serial.println("state charges!");
    // Serial.println("state is");
    // Serial.println(state);
    delay(10);
  }
  old_val = val;

  if (Serial.available() > 0) {
    lastMessageTime = millis();
    // read the incoming byte:
    incomingByte = Serial.read();
    // say what you got:
    //Serial.print("I received: ");
    //Serial.println(incomingByte);   
  }
  delay(1);

  long timedif = Time - lastMessageTime;

  if (timedif > 10000) {//in six second there is no anthor cmd ,disable all motors 
    disenableMotors();
    Time = millis();
    lastMessageTime = Time;
  }

  if (incomingByte == 79) {      // initial  when "O" is sent by DAVID
    //Serial.println("initial the positon");
    boolean isEndNothit = digitalRead(END);
    while (!isEndNothit) {
      //Serial.println("up");
      moveSevaralSteps(true, 1, fastSpeed, true);
      isEndNothit = digitalRead(END);
      //Serial.println("End stop state");
      //Serial.println(isEndNothit);
    }
    moveSevaralSteps(false, stepsInit, normalSpeed, true);
    setInitialPoint();
    incomingByte = 0;
  }

  if (incomingByte == 108) {     // switches of the laser when "l" is sent by DAVID
    onOrOffLaser(false);
    //Serial.println("turn off the laser");
    incomingByte = 0;
  }

  if (incomingByte == 76) {   //switches off light when "L" is sent by DAVID
    onOrOffLaser(true);
    //Serial.println("turn on the laser");
    incomingByte = 0;
  }

  if (incomingByte == 100) {    //switches off light when "d" is sent by DAVID
    onOrOffLight(false);
    //Serial.println("turn off the light");
    incomingByte = 0;
  }

  if (incomingByte == 68) {      //switches on light when "D" is sent by DAVID
    onOrOffLight(true);
    //Serial.println("turn on the light");
    incomingByte = 0;
  }

  if (incomingByte == 116) {  // when "t" was sent by DAVID>> turntable travel
    //this function is not used  
    //Serial.println("in motion");           // sends "in motion" to PC
    turnSevaralSteps(true, 200, 1000);    //turn 200 steps 
    incomingByte = 0;
  }

  if (incomingByte == 80) { // when "P" was sent by DAVID>> go to the initial point
    //Serial.println("to to the INITIAL point");         // sends "INITIAL" to PC
    goToInitialPoint();
  }

  if (incomingByte == 48) { // when "0" was sent by DAVID>> set the initial point
    //Serial.println("SET THE INITIAL POINT");         
    setInitialPoint();
  }

  if (incomingByte == 45) { // when "-" was sent by DAVID>> move down a small step
    // Serial.println("go down a small step");        
    moveSevaralSteps(false, 100, 1000, false); //100 means one mm,when the A4988 on the 1/16 mode
  }

  if (incomingByte == 43) { // when "+" was sent by DAVID>> move up a small step
    //Serial.println("go up a small step");        
    moveSevaralSteps(true, 100, 1000, true); //100 means one mm,when the A4988 on the 1/16 mode
  }

  if (incomingByte == 109) {
    int lastByte = 0;    // when "m" was sent by DAVID>> move
    if (Serial.available() > 0) {
      lastByte = Serial.read();    //check the sign
    }
    boolean sign = 0;
    int num = 0;
    if (lastByte == 45) {
      sign = false;
    } 
    else if (lastByte > 48 && lastByte <= 57) {
      num = lastByte - 48;
      sign = true;
    } 
    else if (lastByte == 43) {
      sign = true;
    }

    if (Serial.available() > 0) {
      lastByte = Serial.read();    //record the num
    }

    while (lastByte >= 48 && lastByte <= 57) //if lastByte >0 and lastByte <=9
    {
      num = num * 10 + (lastByte - 48);    //only record the num
      if (Serial.available() > 0) {
        lastByte = Serial.read();    //record the num
        if (!(lastByte >= 48 && lastByte <= 57)) {
          break;
        }
      } 
      else {
        break;
      }
    }
    //Serial.println(num);

    moveSevaralSteps(sign, num, 1000, true); //100 means one mm,when the A4988 on the 1/16 mode
    incomingByte = 0;
  }

  if (state == 1)    //button has hit then run scanning system
  {
    //first;refresh the necessary data turnCount decide the cycle time
    int onecycleSteps = 3200 / (turnCount - 1);
    int cycleTime = turnCount;
    int cycleRange = range;
    //second:enter the scanning cycle
    while (cycleTime > 0 && state == 1)    //enter the scanning cycle
      //exitting condition 
    {
      //1
      Serial.println("2"); //send to davide ,set the camera prepare for scan
      delay(2000);

      //2
      goToInitialPoint();    //set laser to InitialPoint
      delay(1000);

      //3
      onOrOffLaser(true);
      Serial.println("S");    //send to davide ,start the first scaning
      delay(100);

      while (true) {
        //
        Time = millis();    //record the runtime
        if (Serial.available() > 0) {
          lastMessageTime = millis();
          // read the incoming byte:
          incomingByte = Serial.read();
          // say what you got:
          //Serial.print("I received: ");
          //Serial.println(incomingByte);   
        }
        delay(1);
        long timedif = Time - lastMessageTime;

        if (timedif > 10000) {//in 60 second there is no anthor cmd ,disable all motors 
          //and exit the cycle
          //disenableMotors();
          Time = millis();
          lastMessageTime = Time;
        }

        if (incomingByte == 109) {
          int lastByte = 0;    // when "m" was sent by DAVID>> move
          if (Serial.available() > 0) {
            lastByte = Serial.read();    //check the sign
          }
          boolean sign = 0;
          int num = 0;
          if (lastByte == 45) {
            sign = false;
          } 
          else if (lastByte > 48 && lastByte <= 57) {
            num = lastByte - 48;
            sign = true;
          } 
          else if (lastByte == 43) {
            sign = true;
          }

          if (Serial.available() > 0) {
            lastByte = Serial.read();    //record the num
          }

          while (lastByte >= 48 && lastByte <= 57) //if lastByte >0 and lastByte <=9
          {
            num = num * 10 + (lastByte - 48);  //only record the num
            if (Serial.available() > 0) {
              lastByte = Serial.read();    //record the num
              if (!(lastByte >= 48 && lastByte <= 57)) {
                break;
              }
            } 
            else {
              break;
            }
          }
          //Serial.println(num);
          //if(sign){
          cycleRange -= num;
          //}
          //else
          //{
          //cycleRange+=num;
          //}
          moveSevaralSteps(sign, num, slowSpeed, true); //100 means one mm,when the A4988 on the 1/16 mode
          incomingByte = 0;
        }

        boolean isEnd = digitalRead(END);      //if end switsh is hitted
        if (isEnd || cycleRange < 0) {
          Serial.println("T");          //send to davide ;stop scan
          delay(4000);

          Serial.println("3");          //prepare to grab the texture
          delay(4000);

          onOrOffLaser(false);
          delay(1000);
          Serial.println("G");          //grab the texture
          delay(4000);
          onOrOffLaser(true);
          delay(100);
          Serial.println("A");
          delay(4000);

          Serial.println("2");
          delay(1000);
          Serial.println("E");          //erase the last scan
          delay(2000);
          break;          //exit this scan,and enter the next scan
        }
      }

      cycleRange = range;          //refresh the laser range
      cycleTime--;

      turnSevaralSteps(true, onecycleSteps, 500); //turnable plan turns a angle
    }
  }

  val = LOW;
  incomingByte = 0;
  state = 0;
}

