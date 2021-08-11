#include <TouchScreen.h> //touch library
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include "UNO-HIDKeyboard-Library/HIDKeyboard.h"
#include "UNO-HIDKeyboard-Library/HIDKeyboard.cpp"

//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV tft(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//param calibration from kbv
#define TS_MINX 906
#define TS_MAXX 116

#define TS_MINY 92 
#define TS_MAXY 952

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define BTN_TXT_CLEARANCE 5
#define TXT_LINE_CLEARANCE 20
#define FONT_HIGHT 14
#define FONT_WIDTH 11

#define BTNS_SIZE 4

using actionFN = void(*)();

void nop(){};

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

HIDKeyboard Keyboard; 

void f20(){
  Keyboard.pressSpecialKey(F13);
  Keyboard.releaseKey();
}

void f21(){
  Keyboard.pressSpecialKey(F14);
  Keyboard.releaseKey();
}

void f22(){
  Keyboard.pressSpecialKey(F15);
  Keyboard.releaseKey();
}

void f23(){
  Keyboard.pressSpecialKey(F16);
  Keyboard.releaseKey();
}

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    tft.Set_Text_Mode(mode);
    tft.Set_Text_Size(csize);
    tft.Set_Text_colour(fc);
    tft.Set_Text_Back_colour(bc);
    tft.Print_String(str,x,y);
}

void write_string(uint8_t *str,int16_t x,int16_t y,uint8_t size,uint16_t fgcolour, uint16_t bgcolour, boolean mode){
    show_string(str, x, y,size, fgcolour, bgcolour, mode);
}

class Button {
  public:
    Button();
    Button(int id, int16_t x, int16_t y, int16_t colour);
    Button(int16_t x, int16_t y, int16_t size, int16_t colour, int id);
    Button(int16_t x, int16_t y, int16_t size, int16_t colour, int id, String txt);
    Button(int16_t x, int16_t y, int16_t size, int16_t colour, int id, String txt, actionFN fn);
    int ID();
    void Draw(LCDWIKI_KBV screen);
    bool Clicked(int16_t x, int16_t y);
    void Action();
  private:
    int16_t _x, _y, _size, _colour;
    int _id;
    String _txt;
    actionFN _fn;    
};

Button::Button(int16_t x, int16_t y, int16_t size, int16_t colour, int id, String txt, actionFN fn){
  _x = x;
  _y = y;
  _size = size;
  _colour = colour;
  _id = id;
  _txt = txt;
  _fn = fn;
}

Button::Button(){
  _x = 0;
  _y = 0;
  _size = 100;
  _colour = BLUE;
  _txt = "Tiago";
  _fn = nop;
}

Button::Button(int16_t x, int16_t y, int16_t size, int16_t colour, int id){
  _x = x;
  _y = y;
  _size = size;
  _colour = colour;
  _id = id;
  _txt = "Btn" + String(id);
  _fn = nop;
}

Button::Button(int16_t x, int16_t y, int16_t size, int16_t colour, int id, String txt){
  _x = x;
  _y = y;
  _size = size;
  _colour = colour;
  _id = id;
  _txt = txt;
  _fn = nop;
}

int Button::ID(){
  return _id;
}

Button::Button(int id, int16_t x, int16_t y, int16_t colour){
  _x = x;
  _y = y;
  _size = 95;
  _colour = colour;
  _id = 13+id;
  _fn = nop;
  // Serial.print("btns[");
  // Serial.print(id);
  // Serial.print("] = Button(");
  // Serial.print(_x);
  // Serial.print(",");
  // Serial.print(_y);
  // Serial.print(",");
  // Serial.print(_size);
  // Serial.print(",");
  // Serial.print("BLUE");
  // Serial.print(",");
  // Serial.print(_id);
  // Serial.println(");");
}

void Button::Action(){
  _fn();
}

void Button::Draw(LCDWIKI_KBV screen){
  tft.Set_Draw_color(_colour);
  tft.Draw_Round_Rectangle(_x, _y, _x+_size, _y+_size, 5);
  int16_t yPos = _y + (_size/2) - FONT_HIGHT/2 ;

  write_string(_txt.c_str(), _x+5, yPos, 2, RED, RED, 1);
}

bool Button::Clicked(int16_t px, int16_t py){
  if (((px >= _x) && (px <= (_x+_size))) && ((py >= _y) && (py <= (_y+_size)))){
    return true;
  }

  return false;
}

Button btns[BTNS_SIZE];

void setup(void) 
{
  // Serial.begin(115200);
  Serial.begin(9600);

  // Offset constants
  int16_t kx = 20, ky = 90;

  btns[0] = Button(  0 + kx, 0 + ky, 130, 31, 13, "   EMACS", f20);
  btns[1] = Button(150 + kx, 0 + ky, 130, 31, 14, "    WEB", f21);
  
  btns[2] = Button(  0 + kx, 150 + ky, 130, 31, 15, "   MUTE", f22);
  btns[3] = Button(150 + kx, 150 + ky, 130, 31, 16, "  PRIVACY", f23);

  tft.Init_LCD();
  // Serial.print("LCD ID:");
  // Serial.println(tft.Read_ID(), HEX);

  tft.Fill_Screen(BLACK);

  // Serial.print("Screen size: (");
  // Serial.print(tft.Get_Display_Width());
  // Serial.print(",");
  // Serial.print(tft.Get_Display_Height());
  // Serial.println(")");

  for (int i = 0; i < BTNS_SIZE; i++){
    btns[i].Draw(tft);
  }

  pinMode(13, OUTPUT);
}

void loop(){
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int16_t x = map(p.x, TS_MINX, TS_MAXX, tft.Get_Display_Width(), 0);
    int16_t y = map(p.y, TS_MINY, TS_MAXY, tft.Get_Display_Height(),0);

    for (int idx = 0; idx < BTNS_SIZE; idx ++){
      if (btns[idx].Clicked(x,y)){
        // Serial.print("clicked btn: ");
        // Serial.println(btns[idx].ID());
        btns[idx].Action();
      }
    }

    delay(100);
  }
}

