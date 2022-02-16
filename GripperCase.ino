#include <Servo.h>

class ServoMG996R{
  const int port = 3;

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
    int dataPort;
    
  public:
      PressureSensor(int dataPort){
        this->dataPort = dataPort;
        pinMode(dataPort, INPUT);
      }
  
      float GetPressure(){
        // transform pressure
        float pressure = analogRead(this->dataPort);
        
        Serial.print("Pressure is:");
        Serial.println(pressure);
        return pressure;
      }

      int GetAveragePressure(int milliseconds){
        int numReadings = milliseconds/10;
        int readings = 0;
        for (int i = 0; i < numReadings; i++){
          readings += this->GetPressure();
        }

        int average = readings / numReadings;
        Serial.print("Average reading is");
        Serial.println(average);
        return average;
      }
};

class Gripper {
  ServoMG996R* servo;
  PressureSensor* pressure;
  PressureSensor* pressure2;

  int timeToSpin(int deciseconds){
    const float a = -0.2;
    const float b = 4.2;
    const float c = 4; 
    int spin = a * deciseconds * deciseconds + b * deciseconds + c;
    return max(4, spin); 
  }

    bool pressureNotReadyToPick(){
      return this->pressure->GetPressure() < 300 && (this->pressure2->GetPressure() < 300);
    }
  
  public:
    Gripper(){
      this->servo = new ServoMG996R();
      this->pressure = new PressureSensor(A1);
      this->pressure2 = new PressureSensor(A2);
      this->Release();
    }

    void Pick(){
      int timePassed = 0;
      while(this->pressureNotReadyToPick()){
        int spin = this->timeToSpin(timePassed);
        this->servo->Turn(spin);
        timePassed++;
        delay(100);
      }
      this->servo->Stop();
    }

    void Release(){
      this->servo->Turn(-30);
      delay(1200);
      this->servo->Stop();
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
