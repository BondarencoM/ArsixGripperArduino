#include <Servo.h>

class ServoMG996R{
  const int port = A0;

  Servo* servo = new Servo();
  int minSignal = 88;
  int maxSignal = 97;

  int spinToSignal(int spin){
    if(spin < 0) return this->maxSignal + abs(spin);
    else if (spin > 0) return this->minSignal - abs(spin);
    else return 90;
  }
  
  public:    
    ServoMG996R(){
      servo->attach(this->port);
    }

    void Turn(int spin){
      Serial.print("Turning servo to:");
      Serial.println(spin);
      this->servo->write(this->spinToSignal(spin));
    }

    void Stop(){
      this->Turn(0);
    }
};

class PressureSensor{
    const int dataPort = A1;
    
  public:
      PressureSensor(){
          pinMode(dataPort, INPUT);
      }
  
      float GetPressure(){
        // transform pressure
        float pressure = analogRead(this->dataPort);
        
        Serial.print("Pressure is:");
        Serial.println(pressure);
        return pressure;
      }
};

class Gripper {
  ServoMG996R* servo;
  PressureSensor* pressure;
  
  public:
    Gripper(){
      this->servo = new ServoMG996R();
      this->pressure = new PressureSensor();
      this->servo->Turn(-10);
      delay(1000);
      this->servo->Stop();
    }

    void Pick(){
      int spin = 0;
      while(this->pressureNotReadyToPick()){
        this->servo->Turn(spin);
        spin++;
      }
      this->servo->Stop();
    }

    void Release(){
      while(this->pressureNotReadyToRelease()){
        this->servo->Turn(-10);
      }
      this->servo->Stop();
    }

  private:
    bool pressureNotReadyToPick(){
      return this->pressure->GetPressure() < 500;
    }

    bool pressureNotReadyToRelease(){
      return this->pressure->GetPressure() >= 500;
    }
};

const int PICKING_PIN = 1;
const int BUSY_PIN = 2;

Gripper* gripper;

void setup(){
  pinMode(PICKING_PIN, INPUT);
  pinMode(BUSY_PIN, OUTPUT);
  gripper = new Gripper();

  Serial.begin(9600);
}

void loop(){
  Serial.read();
  Serial.println("Waiting for command");
  
  while(Serial.available() == 0){}
  Serial.println("Command is:");

  int command = Serial.parseInt();
  Serial.println(command);
  
  if (command) {
    Serial.println("Picking");
    digitalWrite(BUSY_PIN, HIGH);
    gripper->Pick();
    digitalWrite(BUSY_PIN, LOW);
    Serial.println("Finished Picking");
  } else {
    Serial.println("Releasing");
    digitalWrite(BUSY_PIN, HIGH);
    gripper->Release();
    digitalWrite(BUSY_PIN, LOW);
    Serial.println("Finished Releasing");
  }
}
