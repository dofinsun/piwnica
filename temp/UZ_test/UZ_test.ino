int echoPin = 9; 
int trigPin = 8; 
void setup() { 
Serial.begin (9600); 
pinMode(trigPin, OUTPUT); 
pinMode(echoPin, INPUT); 
} 
void loop() { 
int duration, cm; 
digitalWrite(trigPin, LOW); 
delayMicroseconds(2); 
digitalWrite(trigPin, HIGH); 
delayMicroseconds(10); 
digitalWrite(trigPin, LOW); 
duration = pulseIn(echoPin, HIGH); 
cm = duration / 58;
Serial.print(cm); 
Serial.print(" cm"); 
Serial.print("  Duration: "); 
Serial.println(duration); 
delay(500);
}
