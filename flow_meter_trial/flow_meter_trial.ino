#define PULSE_PIN D2  //gpio4


volatile long pulseCount=0;
float calibrationFactor = 4.5;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float totalLitres;

unsigned long oldTime;

void ICACHE_RAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup()
{
  Serial.begin(9600);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0; 

//  pinMode(LED_PIN, OUTPUT);
//  digitalWrite(LED_PIN, HIGH);  // We have an active-low LED attached
 
  pinMode(PULSE_PIN, INPUT);
//  pinMode(PULSE_PIN, INPUT_PULLUP);
//  digitalWrite(PULSE_PIN, HIGH);   ????
  attachInterrupt(PULSE_PIN, pulseCounter, FALLING);
}

void loop()
{
   if((millis() - oldTime) > 1000)    // Only process counters once per second
  {
    detachInterrupt(PULSE_PIN);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;     
    totalLitres = totalMilliLitres * 0.001;
    unsigned int frac;   
    Serial.print("flowrate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");
    Serial.print("  Current Liquid Flowing: ");             // Output separator
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");
    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalLitres);
    Serial.println("L");
    //Serial.print(totalMilliLitres);
    //Serial.println("mL");

    pulseCount = 0;

    attachInterrupt(PULSE_PIN, pulseCounter, FALLING);
  }
}
