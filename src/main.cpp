#define DHTPIN 3

#include "Arduino_SensorKit.h"
#include "Adafruit_Sensor.h"

int button = 4;
int LED = 6;
int potentiometer = A0;
int pastPotentiometerValue = 0;

const int ledProgram = 10;
const int weatherProgram = 9;

String pastStringToPrint;
String stringToPrint;

/**
 * Update Oled with stringToPrint variable only if content has not changed
 */
void updateOled(bool clear = true) {
    if (stringToPrint.compareTo(pastStringToPrint) == 0) {
        return;
    }

    if (clear) {
        Oled.clear();
    }
    Oled.println(stringToPrint);
    Oled.refreshDisplay();
    pastStringToPrint = stringToPrint;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    while (!Serial);

    // Init Button and Potentiometer
    pinMode(button , INPUT);
    pinMode(potentiometer, INPUT);

    // Init LED
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    // Init Oled, use 1.0.8 lib to avoid bug when using Pressure I2C sensor
    Oled.begin();
    Oled.setFlipMode(true);

    // Init Temp/Humidity
    Environment.begin();

    //Init Altitude
    Pressure.begin();
}

void loop() {
    // calculate average value on 10 potentiometer reading
    int potentiometerValue = 0;
    for (int i = 0; i < 10; ++i) {
        int rawPotentiometerValue = analogRead(potentiometer);
        potentiometerValue += (int) map(rawPotentiometerValue, 0, 1023, 0, 10);
    }
    potentiometerValue = (int) lround((double) potentiometerValue / 10);

    // Oled.setFont(u8x8_font_victoriabold8_r);
    Oled.setFont(u8x8_font_chroma48medium8_r);
    Oled.setCursor(0, 0);
    stringToPrint = "";

    switch (potentiometerValue) {
        case ledProgram : {
            // LED Testing sub program
            stringToPrint.concat("LED Mode");

            if (digitalRead(button)) {
                stringToPrint.concat("\nButton pressed");
                updateOled();
                digitalWrite(LED, HIGH);
                delay(100);
                digitalWrite(LED, LOW);
                delay(100);
            } else {
                stringToPrint.concat("\nPush the button");
                updateOled();
                digitalWrite(LED, LOW);
            }
            break;
        }
        case weatherProgram : {
            stringToPrint.concat("Weather Mode\n");
            String temperature = String(Environment.readTemperature());
            String humidity = String(Environment.readHumidity());
            String altitude = String(Pressure.readAltitude());

            stringToPrint.concat("Temp. : " + temperature.substring(0, temperature.length() - 1) + "C\n");
            stringToPrint.concat("Humid.: " + humidity.substring(0, humidity.length() - 1) + "%\n");
            stringToPrint.concat("Alt.  : " + altitude + "m\n");
            updateOled(pastPotentiometerValue != potentiometerValue);
            delay(1000);
            break;
        }
        default :
            stringToPrint.concat("No other Mode");
            updateOled();
            delay(1000);
    }

    pastPotentiometerValue = potentiometerValue;

    return;

    // led intensity with potentiometer
    // int sensorValue = analogRead(potentiometer);
    // int value = map(rawPotentiometerValue, 0, 1023, 0, 100);
}