#include "application.h"


SYSTEM_THREAD(ENABLED);
// We're going to start by declaring which pins everything is plugged into.

const pin_t LED_PIN = D6; // This is where your LED is plugged in. The other side goes to a resistor connected to GND.
const pin_t PHOTO_PIN = A3; // This is where your photoresistor or phototransistor is plugged in. The other side goes to the "power" pin (below).

int analogvalue; // Here we are declaring the integer variable analogvalue, which we will use later to store the value of the photoresistor or phototransistor.
int ledToggle(String command); // Forward declaration
bool isLight = "off"; //this is set to on when it detects sunlight.
bool old_isLight = false;
String command = "off"; //used to turn led on or off

// Next we go into the setup function.

void setup() {
	// This is here to allow for debugging using the USB serial port
	Serial.begin();

	// First, declare all of our pins. This lets our device know which ones will be used for outputting voltage, and which ones will read incoming voltage.
	pinMode(LED_PIN, OUTPUT); // Our LED pin is output (lighting up the LED)
	pinMode(PHOTO_PIN, INPUT);
	

    //check initial state of light, and toggle the led, and publish the light
    analogvalue = analogRead(PHOTO_PIN);
    
    if(analogvalue < 200){
        isLight = false;
        old_isLight = true;
    } else {
        isLight = true;
    }
	// This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.

}


// Next is the loop function...

void loop() {
	
	// This prints the value to the USB debugging serial port (for optional debugging purposes)
	Serial.printlnf("analogvalue%d", analogvalue);
	checkChange();

	// This delay is just to prevent overflowing the serial buffer, plus we really don't need to read the sensor more than
	// 10 times per second (100 millisecond delay)
	delay(1000);
}



//sends string to cloud
int sendChange(String value){
    
    if(value == "shade"){
        Particle.publish("light", "0", PUBLIC);
        return 0;
    }
    Particle.publish("light", "150", PUBLIC);
    return 1;
}




int checkChange() {
    command = "off";
    
    //get sensor value
    analogvalue = analogRead(PHOTO_PIN);
    
    //Particle.publish("analogvalue", String(analogvalue), PUBLIC);
    
    if(analogvalue < 200) {
        isLight = false;
        command = "on";
    } else {
        isLight = true;
    }
    
    //Particle.publish("isLight", String(isLight), PUBLIC);
    //Particle.publish("old_isLight", String(old_isLight), PUBLIC);
    
    //check if light value has changed
    if(isLight == old_isLight) return 0;
    
    if(isLight == true){
        sendChange("light");
    } else {
        sendChange("shade");
    }
    
    old_isLight = isLight;//store current value of light in temp variable
    ledToggle(command);
    
    return 1;
    
}

// Finally, we will write out our ledToggle function, which is referenced by the Particle.function() called "led"

int ledToggle(String command) {

	if (command=="on") {
		digitalWrite(LED_PIN,HIGH);
		return 1;
	}
	
	if (command=="off") {
		digitalWrite(LED_PIN,LOW);
		return 0;
	}
	
	return -1;
}