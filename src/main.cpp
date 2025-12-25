#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Sender.h"
#include "Reciver.h"

#define relay_pin 23
#define iR1_pin 2
#define iR2_pin 3
#define Msensor_pin 5 // NPN mode
#define Switch_pin 4

#define forwardTime 1000
#define BackwardTime 2000

LiquidCrystal_I2C lcd(0x27, 16, 2);

class part
{
public:
  bool isWhite;
  bool isBase;
};

part currentPart;

void sort(void *parameter)
{
  while (1)
  {
    bool part_Sorting = (digitalRead(Switch_pin) == LOW);
    Serial.println("Part Sorting:");
    Serial.println(part_Sorting);
    if (part_Sorting)
    {
      currentPart.isBase = (digitalRead(Msensor_pin) == HIGH);
      currentPart.isWhite = (digitalRead(iR2_pin) == LOW);

      if (currentPart.isWhite && currentPart.isBase)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("White Base");
        Serial.println("White Base");
        Send("A");
      }
      else if (currentPart.isWhite && !currentPart.isBase)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("White Lid");
        Serial.println("White Lid");
        Send("B");
      }
      else if (!currentPart.isWhite && currentPart.isBase)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Black Base");
        Serial.println("Black Base");
        Send("C");
      }
      else if (!currentPart.isWhite && !currentPart.isBase)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Black Lid");
        Serial.println("Black Lid");
        Send("D");
      }
    }
    delay(1000);
  }
}

void feed(void *parameter)
{
  while (1)
  {
    String got = Receive();
    Serial.println(got);
    bool part_Detected = (digitalRead(iR1_pin) == HIGH);
    bool no_part_Sorting = (digitalRead(Switch_pin) == HIGH);

    // F as finished postining, S as start in the beging to feed
    if (no_part_Sorting && part_Detected && (got == "F" || got == "S"))
    {
      digitalWrite(relay_pin, LOW);
      delay(BackwardTime);
      digitalWrite(relay_pin, HIGH);
      delay(forwardTime);
    }
    else if (!part_Detected && no_part_Sorting) // if no part & no sorting means no parts
    {
      Serial.println("no parts");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No Parts");
      Send("N");
    }
    delay(1500);
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
  pinMode(Switch_pin, INPUT_PULLUP);

  digitalWrite(relay_pin, HIGH);
  delay(forwardTime);
  Serial.println("ready");
  Sender_Init();
  Receiver_Init();

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();

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
      2,      // Task priority
      NULL    // Task handle
  );
}

void loop()
{
}
