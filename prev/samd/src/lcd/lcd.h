#include <LiquidCrystal.h>
namespace LCD{
    const int rs = 21, en = 20, d4 = 19, d5 = 18, d6 = 17, d7 = 16;
    LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
    void begin(){
        lcd.begin(16, 2);
    }
    void clear(){
        lcd.clear();
    }
    void write(const char* str){
        lcd.setCursor(0, 0);
        lcd.print(str);
    }
    void write2(const char* str){
        lcd.setCursor(0, 1);
        lcd.print(str);
    }
}
