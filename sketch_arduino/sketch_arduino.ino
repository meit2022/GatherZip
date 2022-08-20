#include <Firebase_Arduino_WiFiNINA.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include<Wire.h>

// CG24 연결 정보 
#define FIREBASE_HOST "cg24-8875b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AxRJusOXkR0cUOgJH9PVqjf6dibJeYpnPETD1Pjv"
#define WIFI_SSID "Soso"
#define WIFI_PASSWORD "32700000"

FirebaseData CG24; // CG24 연결

//서보모터 연결
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

LiquidCrystal_I2C lcd(0x27, 16, 2); // lcd 판 연결

int trigPin = 12; //초음파 출력핀
int echoPin = 13; //초음파 입력핀

void setup() {
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
  
 // 초음파 초기 설정
  pinMode(trigPin, OUTPUT); // 트리거 핀을 출력으로 설정
  pinMode(echoPin, INPUT); // 에코핀을 입력으로 설정

}

void loop() {
  
  Firebase.getString(CG24,"/barcode/barcode");
  String check_result = CG24.stringData();// 사용자 확인 숫자 

  Serial.println(check_result);

  if(check_result == "0"){
    
       lcd.setCursor(3,0);
       lcd.print("CG 24");
       delay(2000);
       lcd.clear();
  }
  
  else if(check_result == "1"){

    // 사용자에게 안내문이 나온다.
       lcd.setCursor(3,0);
       lcd.print("User check");
       lcd.setCursor(3,1);
       lcd.print("Completed");
       delay(3000);
       lcd.clear();
       
    // 사용자에게 컵을 놓으라는 말 
       lcd.setCursor(2,0);
       lcd.print("Place the cup");
       lcd.setCursor(2,1);
       lcd.print("in a circle.");
       delay(3000);
       
       
    // 서보모터 문 1번이 열린다.
        servoDoor1_1.write(180);
        servoDoor1_2.write(0);
        lcd.clear();

        
      while(true){
      digitalWrite(trigPin, HIGH);
      delay(10);
      digitalWrite(trigPin, LOW);
      float duration = pulseIn(echoPin, HIGH); 
      float distance = ((float)(340 * duration) / 10000) / 2;

      Serial.println(distance);
      
      //if문으로 컵과의 거리에 따라 출력값 설정하기
      if(distance <= 6.5){
      //Serial.print("try again");
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
      //Serial.print("Safe");
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
    // 카메라 사진 찍을 타이밍
      lcd.setCursor(3,0);
      lcd.print("Checking");
      lcd.setCursor(3,1);
      lcd.println("cup status");

      
    if (Firebase.setString(CG24,"/barcode/barcode", "5")) {
      Serial.println("ok");

    } else {
      Serial.println("Error: " + CG24.errorReason());
    }
    
    
     // 2차문 
     String cup;

     if (Firebase.getString(CG24,"/barcode/barcode")){
   
     //Serial.println(val);
      
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
      
      if (cup == "1000"){ // 오염도 없음 왼쪽
        servoDoor2_1.write(0);
        servoDoor2_2.write(180);
        delay(3000);
        servoDoor2_1.write(90);
        servoDoor2_2.write(90);
        delay(1000); 
       }

       else if (cup == "100"){ //오염도 있음 오른쪽
      
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

      lcd.setCursor(3,0);
      lcd.print("Collected");
      lcd.setCursor(1,1);
      lcd.println("Have a nice day");
      if (Firebase.setString(CG24,"/barcode/barcode", "0")){
        }
      else{
          Serial.println(CG24.errorReason());
      }
      
      delay(2000);
      lcd.clear();
  }

   
    
    // 등록되지 않은 사용자 
    else{ 
       lcd.setCursor(0,0);
       lcd.print("undefined barcode");
       delay(2000);
      
       lcd.clear();
        if (Firebase.setString(CG24,"/barcode/barcode", "0")){
        }
      else{
          Serial.println(CG24.errorReason());
      }
  }
  
   
   }
