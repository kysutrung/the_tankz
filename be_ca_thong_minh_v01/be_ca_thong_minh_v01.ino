#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>

//Cần làm gì tiếp
//1 Code đọc nhiệt độ từ cảm biến nước 
//2 Code điều bật tắt relay lọc nước
//4 Code cho servo sg90
//5 code cho sưởi

//Khai báo chân cắm các linh kiện

const int SERVO_PIN = 18;
const int BUTTON_01 = 13; //Nút chuyển chế độ
const int BUTTON_02 = 12; //Nút điều chỉnh thông số trong chế độ
const int ONE_WIRE_BUS = 2; //Chân tín hiệu của cảm biến nhiệt độ nước
OneWire oneWire(ONE_WIRE_BUS);
Servo myServo;
DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2); //Nếu không chạy thì thử 0x27 hoặc 0x3F

//Các biến toàn cục
int SET_WATER_TEMP = 25;
int WATER_TEMP = 0;
int FEED_TIME = 12;
int FIL_TIME = 6;
bool IS_FILTER_RUN = 0;
bool IS_HEATING_RUN = 0;
int MENU_MODE = 0;
int HAVE_ACCESS_THIS_MODE = 0;
bool STILL_PRESS_BUT_1 = 0;
bool STILL_PRESS_BUT_2 = 0;


//Khai báo các hàm
void getButton(){
  int but1State = digitalRead(BUTTON_01);
  int but2State = digitalRead(BUTTON_02);

  if (but1State == 0 && STILL_PRESS_BUT_1 == 0){
    MENU_MODE++;
    HAVE_ACCESS_THIS_MODE = 0;
    STILL_PRESS_BUT_1 = 1;
  }
  if (but1State == 1 && STILL_PRESS_BUT_1 == 1){
    STILL_PRESS_BUT_1 = 0;
  }
  if(MENU_MODE > 3){
    MENU_MODE = 0;
  }

  if (but2State == 0 && STILL_PRESS_BUT_2 == 0){
    HAVE_ACCESS_THIS_MODE++;
    STILL_PRESS_BUT_2 = 1;
  }
  if (but2State == 1 && STILL_PRESS_BUT_2 == 1){
    STILL_PRESS_BUT_2 = 0;
  }
  if(HAVE_ACCESS_THIS_MODE > 1){
    HAVE_ACCESS_THIS_MODE = 0;
  }


}

void getWaterTemp(){
  sensors.requestTemperatures(); 
  WATER_TEMP = sensors.getTempCByIndex(0);
}

void feedTheFishs(){
  myServo.write(180);
  delay(740);
  myServo.write(90);
}

void menuInteraction(){
  if(MENU_MODE == 0 && HAVE_ACCESS_THIS_MODE == 1){
    setMainScreen();
  }
  if(MENU_MODE == 1 && HAVE_ACCESS_THIS_MODE == 1){
    setHeating();
  }
  if(MENU_MODE == 2 && HAVE_ACCESS_THIS_MODE == 1){
    setFilterTime();
  }
  if(MENU_MODE == 3 && HAVE_ACCESS_THIS_MODE == 1){
    setFeedingTime();
  }
}

void setMainScreen(){
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
  
}

void setHeating(){
  lcd.clear();
  lcd.setCursor(0, 0);  // Cột + dòng
  lcd.print("Heating Setup:");
  lcd.setCursor(0, 1);
  lcd.print("Always " + String(SET_WATER_TEMP) + "C");
}

void setFilterTime(){
  lcd.clear();
  lcd.setCursor(0, 0);  // Cột + dòng
  lcd.print("Filter Setup:");
  lcd.setCursor(0, 1);
  lcd.print("Every " + String(FIL_TIME) + "H");
}

void setFeedingTime(){
  lcd.clear();
  lcd.setCursor(0, 0);  // Cột + dòng
  lcd.print("Feeding Setup:");
  lcd.setCursor(0, 1);
  lcd.print("Every " + String(FEED_TIME) + "H");
}

void setup(){
  pinMode(BUTTON_01, INPUT_PULLUP);
  pinMode(BUTTON_02, INPUT_PULLUP);
  sensors.begin();
  myServo.attach(SERVO_PIN);
  Wire.begin(21, 22);  // Khởi tạo I2C trên ESP32 (GPIO21 -> SDA, GPIO22 -> SCL)
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}

void loop(){
  getButton();
  Serial.println(String(MENU_MODE) + " " + String(HAVE_ACCESS_THIS_MODE));
}
