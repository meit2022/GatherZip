#include <Firebase_Arduino_WiFiNINA.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include<Wire.h>

// 파이어베이스의 CG24 연결 정보, 와이파이 연결 정보
#define FIREBASE_HOST "cg24-8875b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AxRJusOXkR0cUOgJH9PVqjf6dibJeYpnPETD1Pjv"
#define WIFI_SSID "Soso"
#define WIFI_PASSWORD "32700000"

FirebaseData CG24; // 파이어베이스의 CG24 

//서보모터 
Servo servoDoor1_1;
Servo servoDoor1_2;

Servo servoDoor2_1;
Servo servoDoor2_2;

// 1차문 연결핀
int servoPin1_1 = 8;
int servoPin1_2 = 9;
 
// 2차문 연결핀
int servoPin2_1 = 10;
int servoPin2_2= 11;

LiquidCrystal_I2C lcd(0x27, 16, 2); // lcd 연결핀

int trigPin = 12; //초음파 출력핀
int echoPin = 13; //초음파 입력핀

void setup() {
  
 // 파이어베이스 CG24 연결 중
 Serial.begin(9600);
  
  //Serial.print("Connecting to WiFi…");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    //Serial.print(".");
    delay(3000);
  }
  Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);

  // 서보모터 초기 설정
  servoDoor1_1.attach(servoPin1_1);
  servoDoor1_2.attach(servoPin1_2);

  servoDoor1_1.write(90);
  servoDoor1_2.write(90);

  servoDoor2_1.attach(servoPin2_1);
  servoDoor2_2.attach(servoPin2_2);
  
  servoDoor2_1.write(90);
  servoDoor2_2.write(90);

 // lcd 초기 설정 
  lcd.init();
  lcd.backlight();
  
 // 초음파 센서 초기 설정
  pinMode(trigPin, OUTPUT); // 트리거 핀을 출력으로 설정
  pinMode(echoPin, INPUT); // 에코핀을 입력으로 설정

}

void loop() {
  // 파이어베이스에서 사용자 확인 결과 값을 가져온다. "0"은 사용자 아님 또는 초기화, "1"은 사용자 맞음으로 판단한다
  Firebase.getString(CG24,"/barcode/barcode");
  String check_result = CG24.stringData();

  Serial.println(check_result);

  if(check_result == "0"){
    
       lcd.setCursor(3,0);
       lcd.print("CG 24");
       delay(2000);
       lcd.clear();
  }
  
  else if(check_result == "1"){

    // 사용자 확인이 되었다는 안내문이 출력된다.
       lcd.setCursor(3,0);
       lcd.print("User check");
       lcd.setCursor(3,1);
       lcd.print("Completed");
       delay(3000);
       lcd.clear();
       
    // 사용자에게 컵을 놓으라는 안내문이 출력된다.
       lcd.setCursor(2,0);
       lcd.print("Place the cup");
       lcd.setCursor(2,1);
       lcd.print("in a circle.");
       delay(3000);
       
       
    // 서보모터를 이용하여 문 1번이 열린다.
        servoDoor1_1.write(180);
        servoDoor1_2.write(0);
        lcd.clear();

    // 초음파 센서를 이용하여 컵의 위치를 확인한다.     
      while(true){
      digitalWrite(trigPin, HIGH);
      delay(10);
      digitalWrite(trigPin, LOW);
      float duration = pulseIn(echoPin, HIGH); 
      float distance = ((float)(340 * duration) / 10000) / 2;

      Serial.println(distance);
      
      //초음파 센서와 컵과의 거리에 따라 출력값을 결정한다. 
      
      if(distance <= 6.5){
      
      lcd.setCursor(3,0);
      lcd.println("Put again");
      lcd.setCursor(3,1);
      lcd.println("too close");
      delay(1000);
      lcd.clear();
       }
       
      else if(distance >= 12){
      lcd.setCursor(3,0);
      //Serial.print("Try again");
      lcd.print("Put again ");
      lcd.setCursor(3,1);
      lcd.println("too far");
      delay(1000);
      lcd.clear();
      }
      
      else{
      
      lcd.setCursor(3,0);
      lcd.print("Thank you!");
      lcd.setCursor(3,1);
      lcd.println("Collected");
      delay(3000);
      
      
      // 1번문이 닫힌다. 
      servoDoor1_1.write(90);
      servoDoor1_2.write(90); 
      lcd.clear();
      
      break;
      
        }
      }
    
      lcd.setCursor(3,0);
      lcd.print("Checking");
      lcd.setCursor(3,1);
      lcd.println("cup status");

    // 파이어베이스에 숫자 5를 보낸다.
    if (Firebase.setString(CG24,"/barcode/barcode", "5")) {
      Serial.println("ok");

    } else {
      Serial.println("Error: " + CG24.errorReason());
    }
    

     // 파이어베이스로부터 컵 오염도 검사 결과값을 받아온다. 깨끗하면 1000, 오염되었으면 100을 받아온다. 
     
     String cup;
     if (Firebase.getString(CG24,"/barcode/barcode")){
   
      while(true){
        if(Firebase.getString(CG24,"/barcode/barcode")){
        cup = CG24.stringData();
        Serial.println(cup);
        
        if(cup == "100" || cup == "1000" ){
          Serial.println("checking");
          lcd.clear();
          break; 
          }
        else{
         }
      }
      else{
        Serial.println(CG24.errorReason());
        }
      }
      
      if (cup == "1000"){ // 오염도 없음으로 판단하면 2차문이 왼쪽으로 회전한다. 
        servoDoor2_1.write(0);
        servoDoor2_2.write(180);
        delay(3000);
        servoDoor2_1.write(90);
        servoDoor2_2.write(90);
        delay(1000); 
       }

       else if (cup == "100"){ //오염도 있음으로 판단하면 2차문이 오른쪽으로 회전한다.
      
        servoDoor2_1.write(180);
        servoDoor2_2.write(0);
        delay(3000);
        servoDoor2_1.write(90);
        servoDoor2_2.write(90);
        delay(1000); 
        }

      else{
        Serial.println("undefined number");
      }
  }
  
  else{
    Serial.println(CG24.errorReason());
  }
      
      // 수거 후 수거 완료 안내문이 출력된다.
      lcd.setCursor(3,0);
      lcd.print("Collected");
      lcd.setCursor(1,1);
      lcd.println("Have a nice day");

      //파이어베이스에 0을 보내서 초기화시킨다. 
      if (Firebase.setString(CG24,"/barcode/barcode", "0")){
        }
      else{
          Serial.println(CG24.errorReason());
      }
      
      delay(2000);
      lcd.clear();
  }
   }
