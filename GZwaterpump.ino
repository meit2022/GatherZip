int trigPin = 12; 
int echoPin = 11;
int Relaypin=3;

void setup()
{
  Serial.begin(9600);
  pinMode(Relaypin, OUTPUT);
  digitalWrite(Relaypin, HIGH);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
}

void loop()
{
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);
  
  float duration = pulseIn(echoPin, HIGH); 
  
  float distance = ((float)(340 * duration) / 10000) / 2;
  Serial.println(distance);
  
  if (distance <= 5)
  {
    digitalWrite(Relaypin, LOW);
    delay(500);
    digitalWrite(Relaypin, HIGH);
  }
}
