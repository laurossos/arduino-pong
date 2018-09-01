#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define USED_PINS_COUNT 4

int player_height = 16;
int player_width = 2;

int player0_pos = 32;
int player0_score = 0;

int player1_pos = 32;
int player1_score = 0;

float ball_x = 64;
float ball_y = 32;
float ball_x_vel = 1;
float ball_y_vel = 2;
int ball_radius = 2;
int ball_threshold = 2; //specifies, how many pixels the ball will have to move off-screen for a goal to be counted

int pin_states[14];

//float friction = 2.3;
const int USED_PINS[USED_PINS_COUNT] = {2,3,8,9};

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// End of constructor list


void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void u8g2_str(int x,int y, String string)
{
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(x,y);
  u8g2.print(string);
}
void setup(void) {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  u8g2.begin();
}

void pongDisplayScore(int score0, int score1)
{
  u8g2_str(32, 5, String(score0));
  u8g2_str(96, 5, String(score1));
  //debug
  //u8g2_str(32, 14, String(pin_states[2]));
  //u8g2_str(96, 14, String(pin_states[3]));
  //u8g2_str(64, 25, String(player1_pos - player_height/2));
  //u8g2_str(64, 35, String(player1_pos + player_height /2));
}

int pongReadPin(int pin)
{
  if(digitalRead(pin) == HIGH)
  {
    Serial.println("true");
    return 1;
  }
  else if(digitalRead(pin) == LOW)
  {
    Serial.println("false");
    return 0;
  }
}
void pongNewRound()
{
  ball_x = 64;
  ball_y = 32;
  player0_pos = 32;
  player1_pos = 32;
}

void pongMainCycle(void)
{
  for(int i = 0; i < USED_PINS_COUNT; i++)
  {
    pin_states[USED_PINS[i]] = pongReadPin(USED_PINS[i]);
  }
  
  if (player0_pos > 0){player0_pos -= pin_states[2] * 3;}
  if (player0_pos < 63 - player_height){player0_pos += pin_states[3] * 3;}

  if (player1_pos > 0){player1_pos -= pin_states[8] * 3;}
  if (player1_pos < 63 - player_height){player1_pos += pin_states[9] * 3;}

  //ball scoring
  if (ball_x >= 128 + ball_threshold){player0_score++; pongNewRound();}
  if (ball_x <= 0 - ball_threshold){player1_score++; pongNewRound();}

  //ball_collision
  if (ball_y >= 63 - ball_radius){ball_y_vel = -1.0;}
  if (ball_y <= 0 + ball_radius){ball_y_vel = 1.0;}

  /*debug
  u8g2.drawHLine(0, ball_y, 128);
  u8g2.drawHLine(0, player1_pos, 128);
  u8g2.drawHLine(0, player1_pos +player_height, 128);*/
  
  if (ball_x >= (128 - ball_radius - player_width))
  {
    if(player1_pos <= ball_y and ball_y <= player1_pos + player_height)
    {
      //if(pin_states[3] == 1){ball_y_vel += 0.3;}
      //if(pin_states[2] == 1){ball_y_vel -= 0.4;}
      ball_x_vel = -2;
    }
  }

  if (ball_x <= (0 + ball_radius + player_width))
  {
    if(player0_pos <= ball_y and ball_y <= player0_pos + player_height)
    {
      //if(pin_states[9] == 1){ball_y_vel += 0.5;}
      //if(pin_states[8] == 1){ball_y_vel -= 0.6;}
      ball_x_vel = 2;
    }
  }
  ball_x += ball_x_vel;
  ball_y += ball_y_vel;
  
  //draw ball 
  u8g2.drawDisc(ball_x, ball_y, ball_radius, U8G2_DRAW_ALL);

  //draw player0
  u8g2.drawBox(0, player0_pos, player_width, player_height);

  //draw player0
  u8g2.drawBox(128-player_width, player1_pos, player_width, player_height);
  
  pongDisplayScore(player0_score, player1_score);
}

void loop(void) {
  u8g2_prepare();
  u8g2.clearBuffer();
  pongMainCycle();
  u8g2.sendBuffer();
  delay(1);
}
