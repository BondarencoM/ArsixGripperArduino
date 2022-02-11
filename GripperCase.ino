#include <Servo.h>

class ServoMG996R{
  const int port = 3;

  Servo* servo = new Servo();
  
  public:    
    int minAngle = 0;
    int maxAngle = 120;
    
    ServoMG996R(){
      servo->attach(this->port);
    }

    void Turn(int degreeAngle){
      this->servo->write(degreeAngle);
    }
};

class PressureSensor{
    const int dataPort = 4;
    
    int minAngle = -60;
    int maxAngle = 60;

  public:
      PressureSensor(){
          pinMode(dataPort, INPUT);
      }
  
      float GetPressure(){
        // transform pressure
        return analogRead(this->dataPort);
      }
};

class Gripper {
  ServoMG996R* servo;
  PressureSensor* pressure;
  
  public:
    Gripper(){
      this->servo = new ServoMG996R();
      this->pressure = new PressureSensor();
    }

    void Pick(){
      int angle = this->servo->minAngle;
      while(this->pressureNotReadyToPick()){
        this->servo->Turn(angle);
        angle++;
      }
    }

    void Release(){
      while(this->pressureNotReadyToRelease()){
        this->servo->Turn(this->servo->minAngle);
      }
    }

  private:
    bool pressureNotReadyToPick(){
      return this->pressure->GetPressure() < 10;
    }

    bool pressureNotReadyToRelease(){
      return this->pressure->GetPressure() >= 1;
    }
};

const int PICKING_PIN = 1;
const int BUSY_PIN = 2;

Gripper* gripper;

void setup(){
  pinMode(PICKING_PIN, INPUT);
  pinMode(BUSY_PIN, OUTPUT);
  gripper = new Gripper();
}

void loop(){
  if (digitalRead(PICKING_PIN) == HIGH) {
    digitalWrite(BUSY_PIN, HIGH);
    gripper->Pick();
    digitalWrite(BUSY_PIN, LOW);
  } else {
    digitalWrite(BUSY_PIN, HIGH);
    gripper->Release();
    digitalWrite(BUSY_PIN, LOW);
  }
}
