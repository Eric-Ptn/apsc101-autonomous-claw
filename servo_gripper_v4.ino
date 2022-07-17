//sensor reading comes from slightly in front of the cylinders
#include <Servo.h>

Servo servo_ily;

const int sensorEcho = 3;
const int sensorTrig = 4;
const int servoPin = 10; //must have PWM

const int criticalDistance = 10; //in cm

//sort 5 readings in increasing order
void bubble_sort(double a[], int n) {
  
    int i = 0, j = 0, temp;
    
    for (i = 0; i < n; i++) {
      
        for (j = 0; j < n - i - 1; j++) {
          
            if (a[j] > a[j + 1]) {
                temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }
            
        }
    }
}

void moveServo(int moveDir){

  int movingTime = 1500; //in milliseconds
  
  unsigned long moveStartTime = millis();
  unsigned long currentTime = millis();

  do{
    currentTime = millis();
    long angle = map(currentTime - moveStartTime, 0, movingTime, 0, moveDir*270);
    servo_ily.write(angle); 
    
  }while(currentTime - moveStartTime < movingTime);
  
}

void setup() {
  pinMode(sensorEcho, INPUT);
  pinMode(sensorTrig, OUTPUT);

  servo_ily.attach(servoPin);
  
  Serial.begin(9600);
}

void loop() {

  static double lastMedian = 10;
  int distance;
  static int clawOpen = 1;
  static double last5Readings[5];

  Serial.println("Readings:");
  for(int i = 0; i < 5; i++){

    //take a reading
    digitalWrite(sensorTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensorTrig, LOW);

    double sensorTime = pulseIn(sensorEcho, HIGH); //sensorTime is in microseconds

    distance = sensorTime * 0.034/2; //0.034 is speed of sound in cm/microsecond, 
    //divide by two for the wave to leave and come back

    last5Readings[i] = distance; //store readings in array
    Serial.println(last5Readings[i]);
    
  }

  bubble_sort(last5Readings, 5); //sort readings

  double median = last5Readings[2]; //take median of readings
  //any sensor spikes should not affect the reading then
  Serial.println("Median:");
  Serial.println(median);

  if(median < criticalDistance && lastMedian >= criticalDistance && clawOpen == 1){

    moveServo(1);
    clawOpen = 0;
    delay(1500);
    
  }

  else if(median < criticalDistance && lastMedian >= criticalDistance && clawOpen == 0){
  //open the claw if it's closed and you're near the floor
    
    moveServo(0);
    clawOpen = 1;
    delay(1500);
    
  }

  lastMedian = median;
    

  delay(500);

}
