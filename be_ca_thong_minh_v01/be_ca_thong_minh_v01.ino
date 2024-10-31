#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Màn này chạy với điện 5V, 3v3 bị mờ
// Địa chỉ I2C của LCD, thường là 0x27 hoặc 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Các biến toàn cục
int SET_WATER_TEMP = 25;
int FEED_TIME = 12;
int FIL_TIME = 6;

//Khai báo các hàm
void setMainScreen(){
  lcd.clear();
  lcd.setCursor(0, 0);  // Cột + dòng
  lcd.print("T   F   H   Feed");
  lcd.setCursor(0, 1);
  lcd.print("26  OFF OFF 6H");
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
  lcd.print("Every" + String(FIL_TIME) + "H");
}

void setFeedingTime(){
  lcd.clear();
  lcd.setCursor(0, 0);  // Cột + dòng
  lcd.print("Feeding Setup:");
  lcd.setCursor(0, 1);
  lcd.print("Every 24H");
}

void setup() {
  Wire.begin(21, 22);  // Khởi tạo I2C trên ESP32 (GPIO21 -> SDA, GPIO22 -> SCL)
  lcd.init();          // Khởi động LCD
  lcd.backlight();     // Bật đèn nền LCD

  setHeating();
}

void loop() {

}
