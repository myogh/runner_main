//------------------------------------------------------------------//
//-----------  Go Straight With PID ultrasonic adied  -------------//
//-----------------------------------------------------------------//

void goStraight(bool sonarHelp){
  /*
      output: required bits (volts) to the motors
              bit_right and bit_left in the range of 0 to 255
   */


   now_time = millis();



    // from count to angle of the rotation of the wheels
   angle_left = (360.0/660.0) * countLeft; //check both speeds again.
   angle_right = (360.0/660.0) * countRight;



   // from degree to radian conversion
   radian_left = angle_left * (PI/180.0);
   radian_right = angle_right * (PI/180.0);



   dt = now_time - prv_time; // dt is fine



   // angular velocity = d(angle) / dt --> unit = rad/s
   actual_velo_left = (radian_left - prv_radian_left)   / (double)(dt*0.001);
   actual_velo_right =  (radian_right - prv_radian_right)  / (double)(dt*0.001);


   // get the distance from the walls to the robot in cm
   // getSonar();


   // returns the desire left and right speed based on dist_left - dist_right
   sonarGuided();

   if (sonar_help == false){
     error_left = desire_speed - actual_velo_left;
     error_right = desire_speed - actual_velo_right;
   }

   else if (sonar_help == true){
     error_left = desire_left_speed - actual_velo_left;
     error_right = desire_right_speed - actual_velo_right;
   }



   volt_left = (Kp1*error_left) +
               (Ki1*(error_left + prv_error_left))+
               (Kd1*(error_left - prv_error_left));


   volt_right = (Kp2*error_right) +
                (Ki2*(error_right + prv_error_right)) +
                (Kd2*(error_right - prv_error_right));


   bit_left = (volt_left / 12.0) * 255.0;
   bit_left = constrain(bit_left, 0, 255);


   bit_right = (volt_right / 12.0) * 255.0;
   bit_right = constrain(bit_right, 0, 255);


   digitalWrite(cw_left, HIGH);
   digitalWrite(ccw_left, LOW);
   analogWrite(pwm_left, bit_left);

   digitalWrite(cw_right, LOW);
   digitalWrite(ccw_right, HIGH);
   analogWrite(pwm_right, bit_right);


   Serial.print(desire_speed);
   Serial.print(" ");
   Serial.print(actual_velo_left);
   Serial.print(" ");
   Serial.println(actual_velo_right);


   prv_time = now_time;

   prv_radian_left = radian_left;
   prv_radian_right = radian_right;

   prv_error_left = error_left;
   prv_error_right = error_right;

}


void readEncoder1(){
  /* generates counts from the left motor encoder*/

  if(digitalRead(encoderPinA1) == digitalRead(encoderPinB1))
  {
    countLeft = countLeft + 1;
  }
  else
  {
    countLeft = countLeft - 1;
  }
}


void readEncoder2(){
  /* generates counts from the right motor encoder */

  if(digitalRead(encoderPinA2) == digitalRead(encoderPinB2))
  {
    countRight = countRight - 1;
  }
  else
  {
    countRight = countRight + 1;
  }
}


void displayCounts(){
  /* prints the value of counts of both motors on serial monitor */

  Serial.print("Left Count:");
  Serial.print(countLeft);
  Serial.print(" || ");
  Serial.print("Right Count:");
  Serial.println(countRight);
}


void driveForward(double leftSpeed, double rightSpeed){

  /* drives both motors with the desire speed (without PID)*/

  digitalWrite(cw_left, HIGH);
  digitalWrite(ccw_left, LOW);
  analogWrite(pwm_left, leftSpeed);

  digitalWrite(cw_right, LOW);
  digitalWrite(ccw_right, HIGH);
  analogWrite(pwm_right, rightSpeed);
}

void turnLeft(){



  desire_angle = 270.0;

  //Angle Travelled
  angle_left = (360.0/660.0) * abs( countLeft );
  angle_right = (360.0/660.0) * abs ( countRight );


  //PID Calculations for left and right motors
  left_error = desire_angle - angle_left;
  right_error = desire_angle - angle_right;


  Kp_left = 0.02;
  Kd_left = 0.025;


  Kp_right = 0.02;
  Kd_right = 0.04;



  left_diff_error = (left_error - prv_left_error);
  right_diff_error = (right_error - prv_right_error);



  leftMotorVolt = Kp_left *( left_error ) + Kd_left *( left_diff_error );
  rightMotorVolt = Kp_right *( right_error ) + Kd_right *( right_diff_error );



  leftBit = (abs(leftMotorVolt) / 12.0) * 255;
  rightBit = (abs(rightMotorVolt) / 12.0)*255;



  leftBit = constrain (leftBit,0,255);
  rightBit = constrain (rightBit,0,255);


  //Turn both motors to position
  if ( left_error > 0 ){

    digitalWrite (cw_left,LOW);
    digitalWrite (ccw_left,HIGH);
    analogWrite (pwm_left,leftBit);

  }

  else{

    digitalWrite (cw_left,HIGH);
    digitalWrite (ccw_left,LOW);
    analogWrite (pwm_left,leftBit);
  }

  if ( right_error > 0 ){

    digitalWrite (cw_right,LOW);
    digitalWrite (ccw_right,HIGH);
    analogWrite (pwm_right,rightBit);

  }

  else{

    digitalWrite (cw_right,HIGH);
    digitalWrite (ccw_right,LOW);
    analogWrite (pwm_right,rightBit);

  }



Serial.print (left_error);
Serial.print(" ");
Serial.print (Kd_left *( left_diff_error ));
Serial.print(" ");
Serial.println(right_error);



prv_left_error = left_error;
prv_right_error = right_error;


}

void resetCounts(){
  // digitalWrite(cw_left, LOW);
  // digitalWrite(ccw_left, LOW);
  // analogWrite(pwm_left, 0.0);
  //
  // digitalWrite(cw_right, LOW);
  // digitalWrite(ccw_right, LOW);
  // analogWrite(pwm_right, 0.0);

  countLeft = 0.0;
  countRight = 0.0;
}

//---------------------Upper Mechanism Control------------------------//

void runStepper (float degree, char direct) //Run 45 degrees turns every 1 second
{
  if ( direct == 'C' )
  {
    digitalWrite(dir,HIGH);
  }
  else if ( direct == 'A' )
  {
    digitalWrite(dir,LOW);
  }

  int steps = 1600*degree/360;

  for (i = 0; i < steps; i++ ){
    digitalWrite(stp,HIGH);
    delay(1);
    digitalWrite(stp,LOW);
    delay(1);
  }
  delay(1000);
}

void runServo(){
    myServo.write(30);
    delay(2000);
    myServo.write(98);
}
