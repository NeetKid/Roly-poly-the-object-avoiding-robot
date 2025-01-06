/*
Owen McNeal, 12/3/2024
Main_Rolly_Polly_Code

This code was developed with the sould purpose to at least make your robot run after you put it together but tangentially 
it was developed to help push your coding knowledge further aswell as your understanding of the components involved. 
Please read through the comments if you are new to this coding environment and please make tweaks to the code to experiment 
and learn about how it all works. In the GitHub where you downloaded this code, there is also an experimental file with 
more fun functions that I haven't quite finished and some that are a bit too advanced for this introductory course.

*/


const int LeftMotorForward = 2; //Good utilization of variable names can help overall in the building and reading of code.
const int LeftMotorBackward = 3;
const int RightMotorForward  = 4;
const int RightMotorBackward = 5;

const int ultraSonicTrig = A3; //Ultra sonic sensor pins I/O.
const int ultraSonicSense = A2;
long duration; //Duration of time from when the ultrasonic sensor triggered a pulse to when it sensed the pulse.

double distance; //Aribtrary variable used for sensing and logic.

double maxDist; //This is an abritrary variable dipendent on value declared for specific aplication.
double minDist; //This is an abritrary variable dipendent on value declared for specific aplication.
double degreeMs = 10; //Some value calculated for right now its 10, units:  ((x) ms) /  (1 degree).

int degreeInc = 0; //This will change when trying to find where a object is.
bool object = true; //Determines if the object is infront of the sensor.

//void setup() runs a single time right after the arduino is powered.
void setup() {

  Serial.begin(9600); //while the robot is plugged in, look at the serial moniter to see what robot is supposed to do.

  pinMode(RightMotorForward, OUTPUT); //All output becuase we are just interacting with a logic gate
  pinMode(LeftMotorForward,  OUTPUT); //so they either are outputing power ("1") or they arent ("0").
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward,  OUTPUT);

  pinMode(ultraSonicTrig, OUTPUT); // This sends the signal so it's an output.
  pinMode(ultraSonicSense, INPUT); // This recieves the signal so it's an input.

}

//void loop() runs continuously while arduino is powered.
void loop() {
  moveForward();
}

void objectFollow(){

  //Minimum distance is defined as 4 inches. Change this value if your curious!
  minDist = 4;
  //Maximum distance an object can be before searching for the same object or a new one. Change this value if your curious!
  maxDist = 20;

  //This portion will loop as long as there is an object at a distance no greater than maxDist.
  while(object){

    //when the robot senses that the object at a distance greater the define maximum allowed.
    if(Sense() > maxDist){
      
      Stop();
      //object is now false, which signifies that there is no object within our defined distances.
      object = false;
      Serial.print("lost object");
      break; //This breaks the loop.

    }

    //If the object is at or closer than the minimum distance then the robot must stop
    if(Sense() <= minDist){
      
      Serial.print("stopped at object");
      Stop();
      delay(50);

    //If the object is not at or closer than the minimum distance move forward.
    } else {

      Serial.print("moving toward object");
      moveForward();
      delay(100);

    }
  }

  //If object isnt in sight then look back and fourth increasing the angle incriment until object is found.
  while(!object){
    //The angle the robot rotate in each direct with repect to the initial angle increase 5 degrees each time
    //it fails to find an object.
    degreeInc+= 5;

    //Turn left a degreeInc amount of degrees, then stop.
    Serial.print("left");
    turnLeft();
    delay(degreeInc * degreeMs);
    Stop();

    //If an object sensed is closer then or at the max distance allowed then the robot has found a object to follow.
    if(Sense() <= maxDist){
      
      
      Stop();
      delay(50);
      degreeInc = 0;
      //object is true, which signifies that we have found an object to follow.
      object = true;
      Serial.print("object found");
      break;

    }

    delay(50);
    
    //Turn right 2 times the degreeInc amount of degrees, then stop.
    //The "times 2" is becuase we want to be at degreeInc amount of degrees from initial to the right while
    //being at degreeInc amount of degrees to the left.
    Serial.print("Right");
    turnRight();
    delay((degreeInc * 2) * degreeMs);
    Stop();

    //If an object sensed is closer then or at the max distance allowed then the robot has found a object to follow.
    if(Sense() <= maxDist){
      
      Stop();
      delay(50);
      degreeInc = 0;
      //object is true, which signifies that we have found an object to follow.
      object = true;
      Serial.print("object found");
      break;

    }

    delay(50);

  }
}



//This code is a rudimentary way of object avoiding, check out the
//experiemental code in order to find a more advanced accurat method.
void objectAvoid(){

  //Minimum distance is defined as 7 inches. Change this value if your curious!
  minDist = 7;

  //When we are at or below the minimum distance stop and turn left
  if(Sense() <= minDist){
    
    Stop();
    turnLeft();
    //Given an acurate ms/degree this would make the robot turn 50 degrees left.
    //Play around with this value!
    delay(50 * degreeMs);
    Stop();
    //After turning sense again to determine if you need to turn more or move forward.

    if(Sense() <= minDist){
      
      Stop();
      turnRight();
      //I made the degree of rotation greater than the initial 50 degrees because I want the turn to be
      //more extreme. Again play around with this value!
      delay(100 * degreeMs);
      Stop();

    }
  } else {

    //If all is in order and we arent at or beyond our minimum distance then just keep going forward.
    moveForward();
    delay(50);

  }
}


double Sense() {

  //trigger a pules of 40 kilo hz
  digitalWrite(ultraSonicTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(ultraSonicTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultraSonicTrig, LOW);

  //determine time it took from when it trigger to when it was recieved
  duration = pulseIn(ultraSonicSense, HIGH);

  //each other duration is for some reason 0 so this 
  //counteracts that effect and you only get values greater than 0
  if(duration == 0){
    
    digitalWrite(ultraSonicTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(ultraSonicTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultraSonicTrig, LOW);

    duration = pulseIn(ultraSonicSense, HIGH);

  }

  //This equation uses the duration to determine distance.
  //Since sound can only travel at a certain speed we can find distance
  //by multiplying duration by a constant that is already determined 
  //and since the duration is the total time and not the time from when
  //the sensor was trigger to when it bounced off an object we divide by two.
  //This will give you distance in Cm so I multiplied that value by the ration of In / Cm.
  //to convert to inches (freedom units).
  distance = ((duration*.0343)/2) * 0.39;
  Serial.print("Distance: ");
  Serial.println(distance);

  return(distance);

}


//All functions under this comment are movement functions.
//Try to rationalize these functions and make more sense of how the H-bridge works
//aswell as how the robot maneuvers.
void moveBackward() {

    // Code to drive motors backward
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, HIGH);
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, HIGH);

}

void moveForward() {

    // Code to drive motors forward
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);

}

void Stop() {

    // Code to stop motors
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, LOW);

}

void turnLeft() {

    // Code to turn left
    digitalWrite(LeftMotorForward, LOW);
    digitalWrite(LeftMotorBackward, HIGH);
    digitalWrite(RightMotorForward, HIGH);
    digitalWrite(RightMotorBackward, LOW);

}

void turnRight() {

    // Code to turn right
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorForward, LOW);
    digitalWrite(RightMotorBackward, HIGH);

}

