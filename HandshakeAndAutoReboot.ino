/*
---------------------------------------------
Handshake & Auto Reboot
---------------------------------------------
by HUBRIS [hubris.xyz]
[github.com/youandhubris]
Lisbon / March / 2017
---------------------------------------------
*/

// FLOW
#define INTERVAL 10 // LOOP FREQUENCY
unsigned long currentMillis = millis();
unsigned long previousMillis = 0;

// PING PONG
#define PINGPONGINTERVAL 5000 // LOOP FREQUENCY
unsigned long previousPingPongMillis = 0;

// RELAY
void RelayControl();

#define RELAY 3 // PIN MODE
bool relayCurrentState = true; // TRUE == TURN OFF MACHINE
bool relayState = true;
String serialInput = "";

#define RELAYERESETINTERVAL 80000 // RELAY LOOP FREQUENCY
unsigned long relayResetTimer = 0;

String serialInTest = "";

// PROGRAM
void setup()
{
	Serial.begin(9600);
 
	pinMode(RELAY, OUTPUT);
	digitalWrite(RELAY, relayCurrentState);

  HandShake();
}


void loop()
{
	currentMillis = millis();
  serialInput = "";
  
  if (Serial.available() > 0) serialInput = Serial.readStringUntil('\n');
  
  RelayControl();
  CheckPingPong();
  
  delay(10);
}

void HandShake() {
  int noResponseTimer = 0;
  while (Serial.available() <= 0)
  {
    delay(100);
    noResponseTimer += 100;
    if (noResponseTimer > 8000) RebootArduino();
  }
}

void RebootArduino()
{
  pinMode(4, OUTPUT);
  digitalWrite(4, true);
}

void RelayControl()
{
  // INPUT ON / OFF
  if (serialInput == "off") relayState = true;
  else if (serialInput == "on")
  {
    relayState = false;
    relayResetTimer = millis();
  }

  if (relayCurrentState != relayState)
  {
    relayCurrentState = relayState;
    digitalWrite(RELAY, relayCurrentState);
  }

  // IF RELAY OFF FOR TOO LONG
  if (!relayCurrentState)
  {
    currentMillis = millis();
    if ((currentMillis - relayResetTimer) > RELAYERESETINTERVAL)
    {
      relayState = true;
      relayCurrentState = relayState;
      digitalWrite(RELAY, relayCurrentState);
    }
  }
}

void CheckPingPong()
{
  if (currentMillis - previousPingPongMillis > PINGPONGINTERVAL)
  {
    if (serialInput == "Pong") previousPingPongMillis = currentMillis;
    else
    {
      Serial.println("Ping");
      unsigned long pingPongTimeout = currentMillis - previousPingPongMillis - 5000;
      if (pingPongTimeout > PINGPONGINTERVAL) RebootArduino();
    }    
  }
}
