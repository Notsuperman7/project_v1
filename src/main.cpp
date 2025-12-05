#include <Arduino.h>
#define relay_pin 23
#define iR1_pin 2
#define iR2_pin 3
#define Msensor_pin 5 // NPN mode
#define Switch_pin 6

#define forwardTime 1000
#define BackwardTime 1000

class part
{
public:
  bool isWhite;
  bool isBase;
};

part currentPart;

void sort(void *parameter)
{
  if (digitalRead(Switch_pin) == LOW)
  {
    currentPart.isBase = (digitalRead(Msensor_pin) == HIGH);
    currentPart.isWhite = (digitalRead(iR2_pin) == HIGH);
  }
  // sorting
  if (currentPart.isWhite)
  {
    Serial.println("White");
  }
  else
  {
    Serial.println("Black");
  }
  if (currentPart.isBase)
  {
    Serial.println("Base");
  }
  else
  {
    Serial.println("Lid");
  }
}

void feed(void *parameter)
{
  bool part_Detected = (digitalRead(iR1_pin) == LOW);
  bool no_part_Sorting = (digitalRead(Switch_pin) == HIGH);
  if (no_part_Sorting && part_Detected)
  {
    digitalWrite(relay_pin, LOW);
    delay(BackwardTime);
    digitalWrite(relay_pin, HIGH);
    delay(forwardTime);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(relay_pin, OUTPUT);
  digitalWrite(relay_pin, HIGH);

  pinMode(iR1_pin, INPUT);
  pinMode(iR2_pin, INPUT);
  pinMode(Msensor_pin, INPUT);
  delay(forwardTime);
  Serial.println("ready");

  xTaskCreate(
      feed,   // Function name of the task
      "feed", // Name of the task (e.g. for debugging)
      2048,   // Stack size (bytes)
      NULL,   // Parameter to pass
      1,      // Task priority
      NULL    // Task handle
  );
  xTaskCreate(
      sort,   // Function name of the task
      "sort", // Name of the task (e.g. for debugging)
      2048,   // Stack size (bytes)
      NULL,   // Parameter to pass
      1,      // Task priority
      NULL    // Task handle
  );
}

void loop()
{
}
