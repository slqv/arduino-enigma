/* YourDuino.com Example Software Sketch
    LCD_I2C_4x20_TEST.ino
     20 character 4 line I2C Display
     Uses Bill Perry's HD44780 Library, which can be installed from the Arduino Library Manager
     See Bill's documentation: https://github.com/duinoWitchery/hd44780
     See the "API Summary" section.  Use "lcd" in front like lcd.begin(cols, rows)

     LCD Display Blue or Yellow: I2C/TWI "Backpack" Interface
  questions??  terry@yourduino.com

*/
// ----------------------------------------------------------------------------
/*LiquidCrystal compability:
  Since this hd44780 library is LiquidCrystal API compatible, most existing LiquidCrystal
  sketches should work with hd44780 hd44780_I2Cexp i/o class once the
  includes are changed to use hd44780 and the lcd object constructor is
  changed to use the hd44780_I2Cexp i/o class.
*/
/*-----( Import needed libraries )-----*/
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header


/*-----( Declare Constants )-----*/
// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;


/*-----( Declare objects )-----*/
hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip
/*-----( Declare Variables )-----*/
int selectNum = 0;
int pointer = 0;
int encrypting = 0;
int reflector = 0;
int rings[] = {0 , 0 , 0};
int rotors[] = {0 , 0 , 0};
int rotorPosition[] = {0 , 0 , 0};
int scene = 0;

bool refresh = true;
int modulo(int a , int b) {
  return (((a % b) + b) % b);
}
void changeSelect(int amount , int mod) {
  selectNum = modulo(selectNum + amount , mod);
}
char selectChar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char newChar[26][4] = {"A01", "B02", "C03", "D04", "E05", "F06", "G07", "H08", "I09", "J10", "K11", "L12", "M13", "N14", "O15", "P16", "Q17", "R18", "S19", "T20", "U21", "V22", "W23", "X24", "Y25", "Z26"};
int selectPlugboard = 0;
int plugboard[26] = {0 , 1 , 2 , 3 , 4 , 5 , 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
int rotorI[26] = {4, 10, 12, 5, 11, 6, 3, 16, 21, 25, 13, 19, 14, 22, 24, 7, 23, 20, 18, 15, 0, 8, 1, 17, 2, 9};
int inverseRotorI[26] = {20, 22, 24, 6, 0, 3, 5, 15, 21, 25, 1, 4, 2, 10, 12, 19, 7, 23, 18, 11, 17, 8, 13, 16, 14, 9};
int rotorII[26] = {0, 9, 3, 10, 18, 8, 17, 20, 23, 1, 11, 7, 22, 19, 12, 2, 16, 6, 25, 13, 15, 24, 5, 21, 14, 4};
int inverseRotorII[26] = {0, 9, 15, 2, 25, 22, 17, 11, 5, 1, 3, 10, 14, 19, 24, 20, 16, 6, 4, 13, 7, 23, 12, 8, 21, 18};
int rotorIII[26] = {1, 3, 5, 7, 9, 11, 2, 15, 17, 19, 23, 21, 25, 13, 24, 4, 8, 22, 6, 0, 10, 12, 20, 18, 16, 14};
int inverseRotorIII[26] = {19, 0, 6, 1, 15, 2, 18, 3, 16, 4, 20, 5, 21, 13, 25, 7, 24, 8, 23, 9, 22, 11, 17, 10, 14, 12};
int rotorIV[26] = {4, 18, 14, 21, 15, 25, 9, 0, 24, 16, 20, 8, 17, 7, 23, 11, 13, 5, 19, 6, 10, 3, 2, 12, 22, 1};
int inverseRotorIV[26] = {7, 25, 22, 21, 0, 17, 19, 13, 11, 6, 20, 15, 23, 16, 2, 4, 9, 12, 1, 18, 10, 3, 24, 14, 8, 5};
int rotorV[26] = {21, 25, 1, 17, 6, 8, 19, 24, 20, 15, 18, 3, 13, 7, 11, 23, 0, 22, 12, 9, 16, 14, 5, 4, 2, 10};
int inverseRotorV[26] = {16, 2, 24, 11, 23, 22, 4, 13, 5, 19, 25, 14, 18, 12, 21, 9, 20, 3, 10, 6, 8, 0, 17, 15, 7, 1};
int reflectorB[26] = {24, 17, 20, 7, 16, 18, 11, 3, 15, 23, 13, 6, 14, 10, 12, 8, 4, 1, 5, 25, 2, 22, 21, 9, 0, 19};
int reflectorC[26] = {5, 21, 15, 9, 8, 0, 14, 24, 4, 3, 17, 25, 23, 22, 6, 2, 19, 10, 20, 16, 18, 1, 13, 12, 7, 11};
char selectReflector[] = "BC";
int rotorNotch[5] = {16, 4, 21, 9, 25};
char selectRotor[5][4] = {"I  ", "II ", "III", "IV ", "V  "};
bool refreshScene = false;
bool plugboardComplete = false;
int plugboardCount = 0;
char text[21];
char encrypted[21];
int lBtn = 3;
int mBtn = 4;
int rBtn = 5;
bool lStatus = true;
bool mStatus = true;
bool rStatus = true;

bool checkPlugboard(int check) {
  if (check == 0) {
    if (selectPlugboard == plugboard[selectPlugboard]) {
      return true;
    } else {
      return false;
    }
  }
  if (check == 1) {
    if (selectNum == plugboard[selectNum]) {
      return true;
    } else {
      return false;
    }
  }
}
bool checkBtn(char btn) {
  if (btn == 'l') {
    if (digitalRead(lBtn) == HIGH) {
      if (lStatus == true) {
        lStatus = false;
        return true;
      } else {
        return false;
      }
    } else {
      delay(50);
      if (digitalRead(lBtn) == HIGH) {
        if (lStatus == true) {
          lStatus = false;
          return true;
        } else {
          return false;
        }
      } else {
        lStatus = true;
        return false;
      }
    }
  }
  if (btn == 'm') {
    if (digitalRead(mBtn) == HIGH) {
      if (mStatus == true) {
        mStatus = false;
        return true;
      } else {
        return false;
      }
    } else {
      delay(50);
      if (digitalRead(mBtn) == HIGH) {
        if (mStatus == true) {
          mStatus = false;
          return true;
        } else {
          return false;
        }
      } else {
        mStatus = true;
        return false;
      }
    }
  }
  if (btn == 'r') {
    if (digitalRead(rBtn) == HIGH) {
      if (rStatus == true) {
        rStatus = false;
        return true;
      } else {
        return false;
      }
    } else {
      delay(50);
      if (digitalRead(rBtn) == HIGH) {
        if (rStatus == true) {
          rStatus = false;
          return true;
        } else {
          return false;
        }
      } else {
        rStatus = true;
        return false;
      }
    }
  }
}
void stepRotors() {
  rotorPosition[2] = modulo(rotorPosition[2] + 1, 26);
  if (rotorPosition[2] == rotorNotch[rotors[2]]) {
    rotorPosition[1] = modulo(rotorPosition[1] + 1, 26);
    if (rotorPosition[1] == rotorNotch[rotors[1]]) {
      rotorPosition[0] = modulo(rotorPosition[0] + 1, 26);
    }
  }
}
void doPlugboard(int encrypt) {
  encrypting = plugboard[encrypt];
}
void doReflector(int encrypt) {
  if (reflector == 0) {
    encrypting = reflectorB[encrypt];
  }
  if (reflector == 1) {
    encrypting = reflectorC[encrypt];
  }
}
void doRotor(int encrypt , int rotor , int inverse) {
  encrypting = encrypt;
  encrypting = modulo(encrypt + rotorPosition[rotor] - rings[rotor] , 26);
  if (inverse == 0) {
    if (rotors[rotor] == 0) {
      encrypting = rotorI[encrypting];
    }
    if (rotors[rotor] == 1) {
      encrypting = rotorII[encrypting];
    }
    if (rotors[rotor] == 2) {
      encrypting = rotorIII[encrypting];
    }
    if (rotors[rotor] == 3) {
      encrypting = rotorIV[encrypting];
    }
    if (rotors[rotor] == 4) {
      encrypting = rotorV[encrypting];
    }
  }
  if (inverse == 1) {
    if (rotors[rotor] == 0) {
      encrypting = inverseRotorI[encrypting];
    }
    if (rotors[rotor] == 1) {
      encrypting = inverseRotorII[encrypting];
    }
    if (rotors[rotor] == 2) {
      encrypting = inverseRotorIII[encrypting];
    }
    if (rotors[rotor] == 3) {
      encrypting = inverseRotorIV[encrypting];
    }
    if (rotors[rotor] == 4) {
      encrypting = inverseRotorV[encrypting];
    }
  }
  encrypting = modulo(encrypting - rotorPosition[rotor] + rings[rotor] , 26);
}

void setup() {
  Serial.begin(9600);
  pinMode(lBtn, INPUT);
  pinMode(mBtn, INPUT);
  pinMode(rBtn, INPUT);
  lcd.begin(20,4);
  lcd.clear();
  lcd.backlight();      // Make sure backlight is on

  // Print a message on both lines of the LCD.
  lcd.setCursor(3 , 1);
  lcd.print("ENIGMA MACHINE");
  lcd.setCursor(6 , 2);
  lcd.print("By slqv");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (checkBtn('l') == true) {
    if (scene == 0) {
      changeSelect(-1 , 2);
      reflector = selectNum;
      refresh = true;
    }
    if (scene == 1 || scene == 2 || scene == 3) {
      changeSelect(-1 , 5);
      rotors[scene - 1] = selectNum;
      refresh = true;
    }
    if (scene == 4 || scene == 5 || scene == 6) {
      changeSelect(-1 , 26);
      rings[scene - 4] = selectNum;
      refresh = true;
    }
    if (scene == 7) {
      changeSelect(-1 , 26);
      while (checkPlugboard(1) == false) {
        changeSelect(-1 , 26);
      }
      refresh = true;
    }
    if (scene == 8 || scene == 9 || scene == 10) {
      changeSelect(-1 , 26);
      rotorPosition[scene - 8] = selectNum;
      refresh = true;
    }
    if (scene == 11) {
      changeSelect(-1 , 26);
      refresh = true;
    }
  }
  if (checkBtn('r') == true) {
    if (scene == 0) {
      changeSelect(1 , 2);
      refresh = true;
    }
    if (scene == 1 || scene == 2 || scene == 3) {
      changeSelect(1 , 5);
      rotors[scene - 1] = selectNum;
      refresh = true;
    }
    if (scene == 4 || scene == 5 || scene == 6) {
      changeSelect(1 , 26);
      rings[scene - 4] = selectNum;
      refresh = true;
    }
    if (scene == 7) {
      changeSelect(1 , 26);
      while (checkPlugboard(1) == false) {
        changeSelect(1 , 26);
      }
      refresh = true;
    }
    if (scene == 8 || scene == 9 || scene == 10) {
      changeSelect(1 , 26);
      rotorPosition[scene - 8] = selectNum;
      refresh = true;
    }
    if (scene == 11) {
      changeSelect(1 , 26);
      refresh = true;
    }
  }
  if (checkBtn('m') == true) {
    if (scene == 7) {
      if (plugboard[selectNum] != selectPlugboard){
        plugboardCount ++;
      }
      plugboard[selectNum] = selectPlugboard;
      plugboard[selectPlugboard] = selectNum;
      selectPlugboard ++;
      while (checkPlugboard(0) == false && selectPlugboard != 26) {
        selectPlugboard ++;
      }
      if (selectPlugboard == 26 || plugboardCount == 10) {
        plugboardComplete = true;
        selectNum = 0;
      } else {
        selectNum = selectPlugboard;
      }
      refresh = true;
    }
    if (scene == 11) {
      stepRotors();
      encrypting = selectNum;
      Serial.println();
      Serial.println(selectChar[encrypting]);
      doPlugboard(selectNum);
      Serial.println(selectChar[encrypting]);
      doRotor(encrypting , 2 , 0);
      Serial.println(selectChar[encrypting]);
      doRotor(encrypting , 1 , 0);
      Serial.println(selectChar[encrypting]);
      doRotor(encrypting , 0 , 0);
      Serial.println(selectChar[encrypting]);
      doReflector(encrypting);
      Serial.println(selectChar[encrypting]);
      doRotor(encrypting , 0 , 1);
      Serial.println(selectChar[encrypting]);
      doRotor(encrypting , 1 , 1);
      Serial.println(selectChar[encrypting]);
      doRotor(encrypting , 2 , 1);
      Serial.println(selectChar[encrypting]);
      doPlugboard(encrypting);
      Serial.println(selectChar[encrypting]);
      if (strlen(text) < 20) {
        text[strlen(text)] = selectChar[selectNum];
      } else {
        for (int i = 0 ; i < 19 ; i++) {
          text[i] = text[i + 1];
        }
        text[19] = selectChar[selectNum];
      }
      if (strlen(encrypted) < 20) {
        encrypted[strlen(encrypted)] = selectChar[encrypting];
      } else {
        for (int i = 0 ; i < 19 ; i++) {
          encrypted[i] = encrypted[i + 1];
        }
        encrypted[19] = selectChar[encrypting];
      }
      refresh = true;
    }
    if (scene != 7) {
      selectNum = 0;
    }
    refreshScene = true;
    refresh = true;
    if (scene != 7 || plugboardComplete == true) {      
      if (scene != 11){
        scene ++;
        plugboardComplete = false;
      }
    }
  }
  if (refresh == true) {
    lcd.clear();
    lcd.setCursor(0 , 0);
    pointer = 0;
    if (scene == 0){
      lcd.print(selectReflector[selectNum]);
      lcd.setCursor(pointer , 1);
      lcd.print("^");
    }
    if (scene == 1 || scene == 2 || scene == 3){
      lcd.print(selectRotor[rotors[0]]);
      lcd.print(" ");
      lcd.print(selectRotor[rotors[1]]);
      lcd.print(" ");
      lcd.print(selectRotor[rotors[2]]);
      pointer = scene * 4 - 4;
      lcd.setCursor(pointer , 1);
      lcd.print("^");
    }
    if (scene == 4 || scene == 5 || scene == 6){
      lcd.print(newChar[rings[0]]);
      lcd.print(" ");
      lcd.print(newChar[rings[1]]);
      lcd.print(" ");
      lcd.print(newChar[rings[2]]);
      pointer = scene - 3;
      pointer = pointer * 4 - 4;
      lcd.setCursor(pointer , 1);
      lcd.print("^");
    }
    if (scene == 7){
      lcd.print(selectChar[selectPlugboard]);
      lcd.print(selectChar[selectNum]);
      lcd.setCursor(1 , 1);
      lcd.print("^");
    }
    if (scene == 8 || scene == 9 || scene == 10){
      lcd.print(newChar[rotorPosition[0]]);
      lcd.print(" ");
      lcd.print(newChar[rotorPosition[1]]);
      lcd.print(" ");
      lcd.print(newChar[rotorPosition[2]]);
      pointer = scene - 7;
      pointer = pointer * 4 - 4;
      lcd.setCursor(pointer , 1);
      lcd.print("^");
    }
    if (scene == 11) {
      lcd.print(newChar[rotorPosition[0]]);
      lcd.print(" ");
      lcd.print(newChar[rotorPosition[1]]);
      lcd.print(" ");
      lcd.print(newChar[rotorPosition[2]]);
      lcd.setCursor(0 , 1);
      if (strlen(text) > 0) {
        lcd.print(text);
      }
      lcd.setCursor(0 , 2);
      if (strlen(encrypted) > 0) {
        lcd.print(encrypted);
      }
    }
    lcd.setCursor(0 , 2);
    if (scene == 0){
      lcd.print("Reflector ");
    }
    if (scene == 1 || scene == 2 || scene == 3){
      lcd.print("Rotors ");
    }
    if (scene == 4 || scene == 5 || scene == 6){
      lcd.print("Rings ");
    }
    if (scene == 7){
      lcd.print("Plugboard ");
    }
    if (scene == 8 || scene == 9 || scene == 10) {
      lcd.print("Position ");
    }
    if (scene == 11){
      lcd.setCursor(0 , 3);
      lcd.print("Keyboard ");
    }
    lcd.print("< ");
    if (scene == 0) {
      lcd.print(selectReflector[selectNum]);
    }
    if (scene == 1 || scene == 2 || scene == 3) {
      lcd.print(selectRotor[selectNum]);
    }
    if (scene == 4 || scene == 5 || scene == 6) {
      lcd.print(newChar[selectNum]);
    }
    if (scene == 7) {
      lcd.print(selectChar[selectNum]);
    }
    if (scene == 8 || scene == 9 || scene == 10) {
      lcd.print(newChar[selectNum]);
    }
    if (scene == 11){
      lcd.print(selectChar[selectNum]);
    }
    lcd.print(" >");
    refresh = false;
  }
}
