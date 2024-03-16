
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
#define KEY0 0
#define KEY1 1

enum g_e_Dir {UP=1,DOWN,LEFT,RIGHT};
bool g_b_food_eaten = true;
bool g_b_key_statue = true;
const uint8_t g_u8_Block[] PROGMEM = {
  0xf0, // B1111000
  0xb0, // B1011000
  0xd0, // B1101000
  0xf0, // B1111000
};
uint8_t g_u8_Snake_Headx = 4;
uint8_t g_u8_Snake_Heady = 4;
uint8_t g_u8_Snake_Len = 2;
uint8_t g_u8_FoodX;
uint8_t g_u8_FoodY;
uint8_t g_u8_ArrayX[100];
uint8_t g_u8_ArrayY[100];
uint8_t g_u8_dir = 0;



uint16_t g_u16_Score = 0;
uint16_t g_u16_level = 1;
uint16_t g_u16_SnakeSpeed =  150;
/*user define variable end*/

/*user define function begin*/

// 定义中断服务函数
void  handleInterrupt_KEY0() {
  
  if(g_b_key_statue==true)
  {
      g_u8_dir = LEFT;
  }
  else
  {
      g_u8_dir = UP;
  }
  g_b_key_statue = !g_b_key_statue;
}

void  handleInterrupt_KEY1() 
{
  if(g_b_key_statue==true)
  {
      g_u8_dir = RIGHT;
  }
  else
  {
      g_u8_dir = DOWN;
  }
  g_b_key_statue = !g_b_key_statue;
}

void key_init()
{
  pinMode(KEY0,INPUT_PULLUP);
  pinMode(KEY1,INPUT_PULLUP);
}

void key_scan()
{
  if(digitalRead(KEY0)==0)
  {
    delay(10);
    if(digitalRead(KEY0)==0)
      Serial.println("KEY0");
  }
  else if(digitalRead(KEY1)==0)
  {
    delay(10);
    if(digitalRead(KEY1)==0)
      Serial.println("KEY1");
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

void snke_move(void)
{
  switch (g_u8_dir)
  {
  case UP:
    g_u8_Snake_Heady -= 4; 
    break;
  case DOWN:
    g_u8_Snake_Heady += 4; 
    break;
  case LEFT:
    g_u8_Snake_Headx -= 4; 
    break;
  case RIGHT:
    g_u8_Snake_Headx += 4; 
    break;  
  default:
    break;
  }
  if((g_u8_Snake_Headx == g_u8_FoodX)&&(g_u8_Snake_Heady == g_u8_FoodY))
  {
    g_b_food_eaten = true;
    g_u8_Snake_Len++;
    g_u16_Score++;
    g_u16_level = g_u16_Score/5+1;
  }
  for(int i= g_u8_Snake_Len-1;i>0;i--)
  {
    g_u8_ArrayX[i] = g_u8_ArrayX[i-1];
    g_u8_ArrayY[i] = g_u8_ArrayY[i-1];
  }
  g_u8_ArrayX[0] = g_u8_Snake_Headx;
  g_u8_ArrayY[0] = g_u8_Snake_Heady;
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


/*user define function end*/


void setup(void) {

  u8g2.begin();
  Serial.begin(115200);

  Serial.println("test\n\r");
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(1,54,"github.com/olikraus/u8g2");
  
  key_init();
  attachInterrupt(KEY0, handleInterrupt_KEY0, RISING);
  attachInterrupt(KEY1, handleInterrupt_KEY1, RISING);
  randomSeed(analogRead(3));
}


void loop(void) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  
  snke_move();
  draw_food();
  screen();
  
  //key_scan();
  u8g2.sendBuffer();
  delay(100);
}
