#include <Servo.h>
Servo myservo;

const int buttonPin = 4;

int buttonState = 1;
signed long cas_drzania_button = 0;
int cas_set = 20; // ms pre otvorenie
int cas_mario_set = 1000; // ms pre otvorenie na 1 min
int posc, pos = 0; 
int pos_a = 30; // zatvorenie
int pos_b = 165; // otvorenie    
int rychlost[2] = {0, 15}; 
signed long cas_button = millis();
signed long cas_zavretia = millis();
signed long cas_zavretia_long = millis();
signed long cas_delay = millis(); // odpoji po 1000ms servo
signed long timedelay = millis(); // ak je mm < 30 tak po 2000s necha otvoreny kos
signed long timeinit = millis(); // init senzora ak zamrzne, na skusku
bool stav = false;
bool stav_old = stav;

void setup() 
{ 
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP); // button
  pinMode(3, OUTPUT); //buzzer 
  myservo.attach(6); // servo
  delay(800); 
  pos = pos_a;
  senzor();
  cas_zavretia_long = millis();
} 
 
 
void loop() 
{ 
  if(ActiveSensor()){
    myservo.attach(6);
    senzor();
    cas_delay = millis();
  }
  else{
    if((signed long)(millis() - cas_delay) > 1000) // && millis() > cas_zavretia_long)
       myservo.attach(8);
  }
}

void senzor()
{
  if(pos < (pos_b-30))
    pos = pos_b-30;
  else
    pos = pos;
  myservo.write(pos);
  while(pos<pos_b)     
  {                
    pos = constrain(pos+2, pos_a, pos_b);  
    myservo.write(pos);           
    delay(rychlost[1]+0);  
  }
  
  cas_zavretia = millis();

  while(((signed long)(millis()-cas_zavretia)) < 5000 ||  millis() < cas_zavretia_long)
  {
    if(ActiveSensor())
    {
      delay(1);
      cas_zavretia = millis();
    }
  }
  pos = pos_b;
  while(pos>pos_a)     
  {                
    pos = constrain(pos-2, pos_a, pos_b);  
    myservo.write(pos); 
    if(pos > pos_a + 105){ 
      delay(100);
      }
    else{
      delay(70);
      }
    if(ActiveSensor()){
      senzor();
    } 
  } 
}


bool LongOpen(void)
{
    if(stav == stav_old)
    {
      if(((signed long)(millis() - timedelay)) > 800) // ak viac ako 1s
      {
        stav = !stav; // negacia hodnoty, z true na false a opacne... prepnutie z ulohy A na ulohu B
        if(stav == true)
        {
          sing(2);
          cas_zavretia_long = millis()+120000;
          return true;
        }
        else
        {
          sing(1);
          cas_zavretia_long = millis();
        }
      }
    }
    else
    {
      timedelay = millis();
    }
    return false;
}

bool ActiveSensor()
{
  buttonState = digitalRead(buttonPin);
  cas_drzania_button = 0;
  cas_button = millis();
  //Serial.println(buttonState);
  while(buttonState == 0 && cas_drzania_button <= cas_mario_set)     
  {
    delay(1);  
    cas_drzania_button = (signed long)(millis() - cas_button);
    buttonState = digitalRead(buttonPin);
    //Serial.println(buttonState);
  }  
   
  if(cas_drzania_button >= cas_set)
  {
      if(cas_drzania_button >= cas_mario_set )
      {
        return LongOpen();
      }
      //Serial.println("TRUE");
      return true;
  }
  else 
  {
    if(millis() > cas_zavretia_long)
    {
      stav = false;
    }
    stav_old = stav; 
    timedelay = millis();
    //Serial.println("FALSE");
    return false;
  }
}
