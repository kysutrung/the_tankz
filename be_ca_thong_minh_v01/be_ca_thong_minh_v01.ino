#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>

//Khai báo chân cắm các linh kiện
const int RELAY1 = 14; //Relay điều khiển lọc nước 220V
const int HEATING_PIN = 27; //Sưởi nước
const int SERVO_PIN = 18; //Cho cá ăn
const int BUTTON_01 = 13; //Nút chuyển chế độ
const int BUTTON_02 = 12; //Nút điều chỉnh thông số trong chế độ
const int ONE_WIRE_BUS = 26; //Chân tín hiệu của cảm biến nhiệt độ nước
OneWire oneWire(ONE_WIRE_BUS);
Servo myServo;
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2); //Nếu không chạy thì thử 0x27 hoặc 0x3F

//Các biến toàn cục
int SET_WATER_TEMP = 0;
int LAST_SET_WATER_TEMP = 999;
int WATER_TEMP = 0;
int LAST_WATER_TEMP = 999;
int FEED_TIME = 12;
int LAST_FEED_TIME = 999;
int FIL_TIME = 1;
int LAST_FIL_TIME = 999;
int FIL_IN_TIME = 30;
bool IS_FILTER_RUN = 0;
bool IS_HEATING_RUN = 0;
int MENU_MODE = 0;
int HAVE_ACCESS_THIS_MODE = 0;
bool STILL_PRESS_BUT_1 = 0;
bool STILL_PRESS_BUT_2 = 0;
long int RUN_TIME = 0;
long int FIL_RUNTIME = 0;
long int FEED_RUN_TIME = 0;

//Khai báo các hàm

void menuInteraction(){
  int but1State = digitalRead(BUTTON_01);
  int but2State = digitalRead(BUTTON_02);

  if (but1State == 0 && STILL_PRESS_BUT_1 == 0){
    if(HAVE_ACCESS_THIS_MODE == 0){
      MENU_MODE++;
      resetRunTime();
    }

    if(MENU_MODE == 1 && HAVE_ACCESS_THIS_MODE == 1){
      SET_WATER_TEMP++;
      if(SET_WATER_TEMP > 0 && SET_WATER_TEMP < 24){
        SET_WATER_TEMP = 24;
      }
      if(SET_WATER_TEMP > 28){
        SET_WATER_TEMP = 0;
      }
    }

    if(MENU_MODE == 2 && HAVE_ACCESS_THIS_MODE == 1){
      FIL_TIME++;
      if(FIL_TIME > 24){
        FIL_TIME = 0;
      }
    }

    if(MENU_MODE == 3 && HAVE_ACCESS_THIS_MODE == 1){
      FEED_TIME += 6;
      if(FEED_TIME > 48){
        FEED_TIME = 6;
      }
    }

    STILL_PRESS_BUT_1 = 1;
  }
  if (but1State == 1 && STILL_PRESS_BUT_1 == 1){
    STILL_PRESS_BUT_1 = 0;
  }
  if(MENU_MODE > 3){
    MENU_MODE = 0;
  }

  if (but2State == 0 && STILL_PRESS_BUT_2 == 0){
    if(MENU_MODE != 0){
      HAVE_ACCESS_THIS_MODE++;
    }
    STILL_PRESS_BUT_2 = 1;
  }
  if (but2State == 1 && STILL_PRESS_BUT_2 == 1){
    STILL_PRESS_BUT_2 = 0;
  }
  if(HAVE_ACCESS_THIS_MODE > 1){
    HAVE_ACCESS_THIS_MODE = 0;
  }


  if(MENU_MODE == 0){
    setMainScreen();
  }
  if(MENU_MODE == 1){
    setHeating();
  }
  if(MENU_MODE == 2){
    setFilterTime();
  }
  if(MENU_MODE == 3){
    setFeedingTime();
  }
}

void resetRunTime(){
  RUN_TIME = 0;
  FIL_RUNTIME = 0;
  FEED_RUN_TIME = 0;
}

void setMainScreen(){
  getWaterTemp();

  lcd.clear();
  lcd.setCursor(0, 0);  // Cột + dòng
  lcd.print("T   F   H   Feed");

  //Hiển thị nhiệt độ nước
  lcd.setCursor(0, 1);
  lcd.print(String(WATER_TEMP));

  //Hiển thị trạng thái lọc nước
  lcd.setCursor(4, 1);
  if(IS_FILTER_RUN){
    lcd.print("ON");
  }
  else{
    lcd.print("OFF");
  }

  //Hiển thị trạng thái sưởi nước
  lcd.setCursor(8, 1);
  if(IS_HEATING_RUN){
    lcd.print("ON");
  }
  else{
    lcd.print("OFF");
  }

  //Hiển thị khoảng thời gian giữa các lần cho ăn
  lcd.setCursor(12, 1);
  lcd.print(String(FEED_TIME) + "H");
  
  RUN_TIME ++;
  FEED_RUN_TIME ++;
  FIL_RUNTIME ++;
}

void setHeating(){
  if(LAST_SET_WATER_TEMP != SET_WATER_TEMP || RUN_TIME == 0){
    lcd.clear();
    lcd.setCursor(0, 0);  // Cột + dòng
    lcd.print("Heating Setup:");
    lcd.setCursor(0, 1);
    lcd.print("Always " + String(SET_WATER_TEMP) + "C");
    LAST_SET_WATER_TEMP = SET_WATER_TEMP;
    RUN_TIME++;
  }
}

void setFilterTime(){
  if(LAST_FIL_TIME != FIL_TIME || RUN_TIME == 0){
    lcd.clear();
    lcd.setCursor(0, 0);  // Cột + dòng
    lcd.print("Filter Setup:");
    lcd.setCursor(0, 1);
    lcd.print("Every " + String(FIL_TIME) + "H");
    LAST_FIL_TIME = FIL_TIME;
    RUN_TIME++;
  }
}

void setFeedingTime(){
  if(LAST_FEED_TIME != FEED_TIME || RUN_TIME == 0){
    lcd.clear();
    lcd.setCursor(0, 0);  // Cột + dòng
    lcd.print("Feeding Setup:");
    lcd.setCursor(0, 1);
    lcd.print("Every " + String(FEED_TIME) + "H");
    LAST_FEED_TIME = FEED_TIME;
    RUN_TIME++;
  }
}

void getWaterTemp(){
  sensors.requestTemperatures(); 
  WATER_TEMP = sensors.getTempCByIndex(0);
  delay(500);
}

void feedTheFishs(){
  myServo.write(180);
  delay(740);
  myServo.write(90);
}

void waterFilter(bool u){
  if(u == 0){
    digitalWrite(RELAY1, LOW);
    IS_FILTER_RUN = 0;
  }
  else{
    digitalWrite(RELAY1, HIGH);
    IS_FILTER_RUN = 1;
  }
}

void heatTheWater(bool y){
  if(y == 0){
    IS_HEATING_RUN = 0;
    digitalWrite(HEATING_PIN, LOW);
  }
  else{
    IS_HEATING_RUN = 1;
    digitalWrite(HEATING_PIN, HIGH);
  }
}

void wholeSystemLogic(){
  if(SET_WATER_TEMP > WATER_TEMP){
    heatTheWater(1);
  }
  else{
    heatTheWater(0);
  }

  if(FIL_TIME > 0){
    waterFilter(0);

    if(FIL_RUNTIME == FIL_TIME * 7200){
      waterFilter(1);
    }
    if(FIL_RUNTIME == FIL_TIME * 7200 + FIL_IN_TIME){
      waterFilter(0);
      FIL_RUNTIME = 0;
    }
  }
  else{
    waterFilter(1);
  }

  if(FEED_RUN_TIME == FEED_TIME * 7200){
    feedTheFishs();
    FEED_RUN_TIME = 0;
  }

  if(RUN_TIME > 1000000){
    RUN_TIME = 1;
  }
}

void setup(){
  pinMode(BUTTON_01, INPUT_PULLUP);
  pinMode(BUTTON_02, INPUT_PULLUP);
  pinMode(RELAY1, OUTPUT);
  pinMode(HEATING_PIN, OUTPUT);
  digitalWrite(RELAY1, HIGH);
  sensors.begin();
  myServo.attach(SERVO_PIN);
  Wire.begin(21, 22);  // Khởi tạo I2C trên ESP32 (GPIO21 -> SDA, GPIO22 -> SCL)
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  waterFilter(0);
  heatTheWater(0);
}

void loop(){
  menuInteraction();
  Serial.println(String(MENU_MODE) + " " + String(HAVE_ACCESS_THIS_MODE) + " RUNTIME " + String(RUN_TIME));
}
