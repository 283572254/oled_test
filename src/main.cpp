
#include <Arduino.h>

#include <U8g2lib.h>
#include <Wire.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 18, /* data=*/ 19, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 4, /* data=*/ 5);   // ESP32 Thing, HW I2C with pin remapping
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0); // 使用硬件 I2C
/*user define variable begin*/
#define KEY0 0
#define KEY1 1
#define KEY2 2
#define KEY3 6
enum g_e_Dir {UP=1,DOWN,LEFT,RIGHT};
bool g_b_food_eaten = true;
bool g_b_snake_statue = true;
bool g_b_game_over = false;
const uint8_t g_u8_Block[] PROGMEM = {
  0xf0, // B1111000
  0xb0, // B1011000
  0xd0, // B1101000
  0xf0, // B1111000
};
uint8_t g_u8_Snake_Headx = 4;
uint8_t g_u8_Snake_Heady = 4;
uint8_t g_u8_Snake_Len = 1;
uint8_t g_u8_FoodX;
uint8_t g_u8_FoodY;
uint8_t g_u8_ArrayX[100] = {0};
uint8_t g_u8_ArrayY[100] = {0};
uint8_t g_u8_dir = DOWN;



uint16_t g_u16_Score = 0;
uint16_t g_u16_level = 1;
uint16_t g_u16_SnakeSpeed =  150;

int i;
/*user define variable end*/

/*user define function begin*/
void check_snake_die(void);
// 定义中断服务函数
void  handleInterrupt_KEY0() {
    if(g_u8_dir != UP)
      g_u8_dir = DOWN;
    
}
void  handleInterrupt_KEY1() 
{
  if(g_u8_dir != LEFT)
    g_u8_dir = RIGHT;
  
}
void  handleInterrupt_KEY2() {
  if(g_u8_dir != DOWN)
    g_u8_dir = UP;
  
}
void  handleInterrupt_KEY3() {
  if(g_u8_dir != RIGHT)
    g_u8_dir = LEFT;
  
}
void key_init()
{
  pinMode(KEY0,INPUT_PULLUP);
  pinMode(KEY1,INPUT_PULLUP);
  pinMode(KEY2,INPUT_PULLUP);
  pinMode(KEY3,INPUT_PULLUP);
}



void draw_snake(int x, int y)
{
  u8g2.drawBox(x, y, 4, 4);
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
#if 0
  if(g_b_snake_statue==true)
  {
    for (i = 0; i < g_u8_Snake_Len; i++)
    {
      draw_snake(g_u8_ArrayX[i], g_u8_ArrayY[0]);
    }
    g_b_snake_statue = false;
  }
#endif
  for (i = 0; i < g_u8_Snake_Len; i++)
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
#if 1
  if((g_u8_Snake_Headx == g_u8_FoodX)&&(g_u8_Snake_Heady == g_u8_FoodY))
  {
    g_b_food_eaten = true;
    g_u8_Snake_Len++;
    g_u16_Score++;
    g_u16_level = g_u16_Score/5+1;
    g_u16_SnakeSpeed -= g_u16_level;
  }
#endif
  for(i= g_u8_Snake_Len-1;i>0;i--)
  {
    g_u8_ArrayX[i] = g_u8_ArrayX[i-1];
    g_u8_ArrayY[i] = g_u8_ArrayY[i-1];
  }
  g_u8_ArrayX[0] = g_u8_Snake_Headx;
  g_u8_ArrayY[0] = g_u8_Snake_Heady;
  check_snake_die();

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

      for (i = g_u8_Snake_Len - 1; i > 0; i--) // 遍历整个蛇身方块，若食物在蛇身内则重新生成
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
void check_snake_die(void)
{
  if(g_u8_Snake_Headx<4||g_u8_Snake_Headx>92||g_u8_Snake_Heady<1||g_u8_Snake_Heady>56)
  {
    g_b_game_over = true;
  }
  if(g_u8_Snake_Len>4)
  {
    for(int i=1;i<g_u8_Snake_Len;i++)
    {
      if(g_u8_Snake_Headx==g_u8_ArrayX[i]&&g_u8_Snake_Heady==g_u8_ArrayY[i])
      {
        g_b_game_over = true;
      }
    }
  }
}
void draw_game_over()
{
  u8g2.clearBuffer();// 清屏
  u8g2.setFont(u8g2_font_6x10_tf); // 设置字体大小
  u8g2.setCursor(10, 10);// 设置显示位置

  u8g2.print("GAME OVER");
  u8g2.setCursor(30, 35);// 设置显示位置
  u8g2.print("LEVE:");
  u8g2.setCursor(30, 55);// 设置显示位置
  u8g2.print("SCOR:");

  show_score(80, 35, g_u16_level);
  show_score(80, 55, g_u16_Score);

  u8g2.sendBuffer();
}
/*user define function end*/


void setup(void) {

  u8g2.begin();
  Serial.begin(115200);

  Serial.println("test\n\r");
  
  key_init();
  attachInterrupt(KEY0, handleInterrupt_KEY0, RISING);
  attachInterrupt(KEY1, handleInterrupt_KEY1, RISING);
  attachInterrupt(KEY2, handleInterrupt_KEY2, RISING);
  attachInterrupt(KEY3, handleInterrupt_KEY3, RISING);
  randomSeed(analogRead(3));
  g_u8_ArrayX[0]  = g_u8_Snake_Headx;
  g_u8_ArrayY[0]  = g_u8_Snake_Heady;
}


void loop(void) {
  u8g2.clearBuffer();
  if(g_b_game_over)
  {
    draw_game_over();
  }
  else
  {
    snke_move();
    draw_food();
    screen();
  }


  u8g2.sendBuffer();
  delay(g_u16_SnakeSpeed);
}
