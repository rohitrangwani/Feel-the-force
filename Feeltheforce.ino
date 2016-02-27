#define MOTION_NOTIF "PIR"
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 60;        //maximum time to calibration

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 6000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = D1;    //the digital pin connected to the PIR sensor's output
int ledPin = D7;

/////////////////////////////
//SETUP
void setup(){
  pinMode(D0, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D0, HIGH);

  //give the sensor some time to calibrate
  Spark.publish("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      delay(1000);
      }
    Spark.publish(" done");
    delay(50);
  }

////////////////////////////
//LOOP
void loop(){

     if(digitalRead(pirPin) == HIGH){
       digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         motion_notify_user(); 
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       
       digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Spark.publish(MOTION_NOTIF, "Motion Ended",60, PRIVATE);
           delay(50);
           }
       }
  }
  
  int motion_notify_user()
{
    //send notification to user (this one goes to the dashboard)
    Spark.publish(MOTION_NOTIF, "Feel the force : Motion started!", 60, PRIVATE);

    //send push notification to user (this one goes to pushbullet servers)
    Spark.publish("pushbullet", "Feel the force!", 60, PRIVATE);
   
   return 0; 
}