
#include <Arduino.h>



#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 19, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C


/*user define variable begin*/
#define key0 0
#define key1 1
const uint8_t g_u8_Block[] PROGMEM = {
  0xf0, // B1111000
  0xb0, // B1011000
  0xd0, // B1101000
  0xf0, // B1111000
};
uint8_t g_u8_Snake_Len = 2;
uint8_t g_u8_FoodX;
uint8_t g_u8_FoodY;
uint8_t g_u8_ArrayX[100];
uint8_t g_u8_ArrayY[100];
bool g_b_food_eaten = true;



uint16_t g_u16_Score = 0;
uint16_t g_u16_level = 1;
uint16_t g_u16_SnakeSpeed =  150;
/*user define variable end*/

/*user define function begin*/

// 定义中断服务函数
void  handleInterrupt_key0() {
  // 在中断服务函数中进行需要执行的操作
  Serial.println("External interrupt key0!");
}

void  handleInterrupt_key1() {
  // 在中断服务函数中进行需要执行的操作
  Serial.println("External interrupt key1!");
}
void key_init()
{
  pinMode(key0,INPUT_PULLUP);
  pinMode(key1,INPUT_PULLUP);
}

void key_scan()
{
  if(digitalRead(key0)==0)
  {
    delay(10);
    if(digitalRead(key0)==0)
      Serial.println("key0");
  }
  else if(digitalRead(key1)==0)
  {
    delay(10);
    if(digitalRead(key1)==0)
      Serial.println("key1");
  }
}

void draw_snake(int x, int y)
{
  u8g2.drawBitmap(x, y, 1, 4, g_u8_Block);
}

void show_score(int x, int y, int data)
{
  u8g2.setCursor(x, y);// 设置显示位置
  u8g2.print(data);

}
void screen(void)
{
  u8g2.clearBuffer();// 清屏
  u8g2.setFont(u8g2_font_6x10_tf); // 设置字体大小
  u8g2.drawFrame(0, 0, 102, 64); // 绘制边框
  u8g2.setCursor(104, 12);// 设置显示位置
  u8g2.print("LEVE");
  u8g2.setCursor(104, 40);// 设置显示位置
  u8g2.print("SCOR");

  show_score(110, 25, g_u16_level);
  show_score(110, 53, g_u16_Score);

  for (int i = 0; i < g_u8_Snake_Len; i++)
  {
    draw_snake(g_u8_ArrayX[i], g_u8_ArrayY[i]);
  }

  draw_snake(g_u8_FoodX, g_u8_FoodY);

  u8g2.sendBuffer();

}

void draw_food(void)
{
  int food_out = 0; // 判断食物是否在蛇体内

  if (g_b_food_eaten)
  {
    while (food_out == 0)
    {
      food_out = 1;

      g_u8_FoodX = (uint8_t)(random(4, 100) / 4) * 4;
      g_u8_FoodY = (uint8_t)(random(4, 60) / 4) * 4;

      for (int i = g_u8_Snake_Len - 1; i > 0; i--) // 遍历整个蛇身方块，若食物在蛇身内则重新生成
      {
        if (g_u8_FoodX == g_u8_ArrayX[i] && g_u8_FoodX == g_u8_ArrayY[i])
        {
          food_out = 0;
        }
      }
    }
  }

  g_b_food_eaten = false;
}

void snake_move(void)
{

}
/*user define function end*/


void setup(void) {

  u8g2.begin();
  Serial.begin(115200);

  Serial.println("test\n\r");
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(1,54,"github.com/olikraus/u8g2");
  
  key_init();
  attachInterrupt(key0, handleInterrupt_key0, RISING);
  attachInterrupt(key1, handleInterrupt_key1, RISING);
  
}


void loop(void) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  draw_food();
  screen();
  //key_scan();
  u8g2.sendBuffer();
  delay(1000);
}
