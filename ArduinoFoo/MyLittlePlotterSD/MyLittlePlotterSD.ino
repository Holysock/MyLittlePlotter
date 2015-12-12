#include <SD.h>
#include <SPI.h>


/*
=============================================================================
=============================================================================
=                                                                           =
=             MyLittlePlotter V1.2 APLHA 7.Dez 2015                         =
=                   by Nick S. Lemberger aka Holysock                       =
=                      nick-lemberger@live.de                               =
=                                                                           =
=============================================================================
=============================================================================
*/

int stepX = 5; //pinout
int stepY = 7;
int stepZ = 3;
int dirX = 6;
int dirY = 8;
int dirZ = 4;
int enable =2;
int laser = 9; //pewpew
boolean hS_X = 1; //1 if there is a limit switch for respective axis
boolean hS_Y = 1;
boolean hS_Z = 0;
boolean endS_X = 0;
boolean endS_Y = 0;
boolean endS_Z = 0;
int homeX = A1;
int homeY = A0;
int homeZ = A2;
int endX; 
int endY; 
int endZ;
int someSensor = A5;
boolean stepArry[6] = {0,0,0,0,0,0}; //X-step,X-dir,Y-step,Y-dir,Z-step,Z-dir
int t = 40; 
float RposX; //Your current Xposition in steps
float RposY; //             Yposition
float RposZ; //             Zposition
float mmX = 1600; // step/mm ratio
float mmY = 1600;
float mmZ = 4000;
//long endPosX = 528000; // Area in steps X
//long endPosY = 960000; //               Y
long CM_PER_SEGMENT = 1000; //Factor for arc interpolation
boolean Z_mode = 0; // 0 = use laser / 1 = use z-axis
int laserMax = 255;
int laserMin = 0;
byte nextByte;
File file;
String fileName;
boolean debugging=0;
boolean debugging2=0;
boolean debugging3=0;

int penPos=-1;
float penUp = mmZ;

void setup() {
  delay(1500);
  if(debugging3)Serial.begin(9600);
  while(!getConfig()){
    //can't start without config-file / SD-card
    if(debugging3)Serial.println(F("Missing SD-card or config.cfg file."));
  }
  if(debugging){
    Serial.begin(9600);
    Serial.println(F("#MyLittlePlotter V1.1 APLHA 5.Dez 2015#"));
    Serial.println(F("#--by Nick S. Lemberger aka Holysock--#"));
    Serial.println(F("#-------nick-lemberger@live.de--------#"));
  }
  else Serial.end();
  
  pinMode(stepX,OUTPUT);
  pinMode(stepY,OUTPUT);
  pinMode(stepZ,OUTPUT);
  pinMode(dirX,OUTPUT);
  pinMode(dirY,OUTPUT);
  pinMode(dirZ,OUTPUT);
  pinMode(enable,OUTPUT);
  pinMode(laser,OUTPUT);
  if(hS_X) pinMode(homeX,INPUT_PULLUP);
  if(hS_Y) pinMode(homeY,INPUT_PULLUP);
  if(hS_Z) pinMode(homeZ,INPUT_PULLUP);
  if(endS_X) pinMode(endX,INPUT_PULLUP);
  if(endS_Y) pinMode(endY,INPUT_PULLUP);
  if(endS_Z) pinMode(endZ,INPUT_PULLUP);
  power(0);
  if(debugging)Serial.println(F("Motors are on."));
  setPen(1);
  setLaser(laserMin);
}

void loop(){
  readFile();
  idle();
  //step();
}

void idle(){
  power(1);
  setLaser(0);
  if(debugging)Serial.println(F("idle..."));
}

void power(boolean on){
  digitalWrite(enable,on);
}

void setLaser(int pwm){
  analogWrite(9,pwm);
}

void setPen(float z){
  int tmp = t;
  t = 2*t;
  if(z>0&&penPos<0){
    stepArry[0]=0;
    stepArry[1]=0;
    stepArry[2]=1;
    stepArry[5]=0;
    while(RposZ<penUp){
      step();
    }
    penPos = 1;
  }
  else if(z<0&&penPos>0){
    stepArry[0]=0;
    stepArry[1]=0;
    stepArry[2]=1;
    stepArry[5]=1;
    while(RposZ>0){
      step();
    }
    penPos = -1;
  }
  t = tmp;
}

boolean getConfig(){
  if(debugging3)Serial.println(F("reading config file...")); 
  if(!SD.begin()) return 0;
  file=SD.open("config.cfg");
  if(!file) return 0;
  byte cfg=file.read();
  while(cfg!=36) cfg=file.read();
  while(cfg==36) cfg=file.read();
  if(cfg==91){
    if(debugging3)Serial.println(F("Reading fileName.."));
    cfg=file.read();
    while(cfg!=93){
      if(debugging3)Serial.println(fileName);
      fileName += char(cfg);
      cfg=file.read();
    }
    if(debugging3)Serial.println(fileName);
  }
  file.close();
  return 1;  
}

#define STATE_IDLE 0
#define STATE_G 1
#define STATE_M 2
#define STATE_F 3
#define STATE_G0 4
#define STATE_G1 5
#define STATE_G2 6
#define STATE_G3 7

int readFile(){
  /*if(!SD.begin()){
    if(debugging)Serial.println(F("Initialization of SD-card failed!"));
    return 1;
  }*/
  if(debugging){
    Serial.print(F("Reading file:")); 
    Serial.println(fileName);
  }  
  char __fileName[sizeof(fileName)];
  fileName.toCharArray(__fileName, sizeof(__fileName));
  
  file=SD.open("gcode.nc");
  if(!file) return 2;
  byte state = STATE_IDLE;
  
  while (file.available()){  // State machine Z0 = STATE_IDLE
    if(debugging2){
      Serial.print(state);
      Serial.print(F(" "));
      Serial.println(char(nextByte));
    }
   
    if(state==STATE_IDLE){
      nextByte=file.read();
      if(nextByte==71)      state=STATE_G; // found a G-command
      //else if(nextByte==77) state=STATE_M; // found a M-command
      //else if(nextByte==70) state=STATE_F; // found a F-command
      else{
        state=STATE_IDLE;
        continue; 
      }    
      if(debugging2){
        Serial.print(state);
        Serial.print(F(" "));
        Serial.println(char(nextByte));
      }
    }
    
    if(state==STATE_G){ // what G-command?
      nextByte=file.read();
      if(nextByte==48){ // found a G0-command
        nextByte=file.read();       
        if(nextByte==48)      state=STATE_G0; // really a G0-command? Y/N
        else if(nextByte==49) state=STATE_G1; // found a G01-command
        else if(nextByte==50) state=STATE_G2; // found a G02-command
        else if(nextByte==51) state=STATE_G3; // found a G03-command
      }
      else if(nextByte==49) state=STATE_G1; // found a G1-command
      else if(nextByte==50) state=STATE_G2; // found a G2-command
      else if(nextByte==51) state=STATE_G3; // found a G3-command
      else{
        state=STATE_IDLE;
        continue; 
      }
      if(debugging2){
        Serial.print(state);
        Serial.print(F(" "));
        Serial.println(char(nextByte));
      }
      
      float x = RposX;
      float y = RposY;
      float z = penPos;
      float i = 0;
      float j = 0;  
 
      nextByte=file.read(); 
      
      while(nextByte==32) nextByte=file.read(); // getting rid of space
        
      while(nextByte==120||nextByte==88||nextByte==121||nextByte==89||nextByte==122||nextByte==90||nextByte==105||nextByte==73|nextByte==106||nextByte==74){ // while there are parameters...
        //possible paremeter-prefixes are: x,y,z,i,j,X,Y,Z,I,J 
        if(nextByte==120||nextByte==88)       x=findNum(); 
        else if(nextByte==121||nextByte==89)  y=findNum();
        else if(nextByte==122||nextByte==90)  z=findNum();
        else if(nextByte==105||nextByte==73)  i=findNum();
        else if(nextByte==106||nextByte==74)  j=findNum();
        while(nextByte==32) nextByte=file.read(); // getting rid of space
        }
        if(debugging){
         Serial.println(F("Parameters (x,y,z,i,j):"));
         Serial.println(x);
         Serial.println(y);
         Serial.println(z);
         Serial.println(i);
         Serial.println(j);
         Serial.println(F("----------------------- "));
        }
        if(state==STATE_G0){
          if(debugging)Serial.println(F("G0"));
          
          setPen(z);
          line(x,y);
          setLaser(laserMin);
        }
        else if(state==STATE_G1){
          if(debugging)Serial.println(F("G1"));
          
          setPen(z);
          line(x,y);
          setLaser(laserMin);
        }
        else if(state==STATE_G2){
          if(debugging)Serial.println(F("G2"));
          
          setPen(z);
          arc(x,y,i,j,0);
          setLaser(laserMin);
        }
        else if(state==STATE_G3){
          if(debugging)Serial.println(F("G3")); 
          
          setPen(z);
          arc(x,y,i,j,1);
          setLaser(laserMin);
        }  
        state=STATE_IDLE;
        continue;
    }
    if(state==STATE_M){ // M-command
      //Serial.print(state);
      //Serial.print(" ");
      //Serial.println(char(nextByte));
    }
    if(state==STATE_F){ // F-command  
      //Serial.print(state);
      //Serial.print(" ");
      //Serial.println(char(nextByte));
    }
  }
  return 0;
}

float findNum(){
  boolean flag=0; // minus flag
  float tmp=0;
  
  nextByte=file.read(); 
  
  while(nextByte==32) nextByte=file.read(); // getting rid of spaces
  
  if(nextByte==45){ // negative number? 
    flag = 1;       // set minus-flag
    nextByte=file.read();
  }
  if(nextByte>=48 && nextByte<=57) tmp = nextByte-48;
  //Serial.println(F("Now numbers before point"));
  //Serial.println(tmp);
  nextByte = file.read();
  if(nextByte>=48 && nextByte<=57){
    while(nextByte!=44 && nextByte!=46 && nextByte!=32){ // numbers before point 
      float aNum = nextByte - 48;
      tmp = tmp*10 + aNum;
      //Serial.println(tmp);
      nextByte = file.read();
    }
  }
  if(nextByte!=44&&nextByte!=46){
      if(flag) tmp = -tmp;
      //Serial.println(F("Well, thats strange..."));
      return tmp;
   }
   else{
     nextByte = file.read();
     int i=1;
     //Serial.println(F("Now numbers after point"));
     while(nextByte>=48 && nextByte<=57){  // numbers after point
       float aNum = nextByte-48; 
       for(int j=0;j<i;j++){
         aNum = aNum*0.1;
       }
       i++;
       tmp = tmp + aNum;
       //Serial.println(aNum);
       nextByte = file.read();
     }
     if(flag) tmp = -tmp;
     return tmp;
   }
}

void step (){
  if(hS_X&&limit(1,0)&&stepArry[3]) stepArry[0]=0;
  if(hS_Y&&limit(2,0)&&stepArry[4]) stepArry[1]=0;
  if(hS_Z&&limit(3,0)&&stepArry[5]) stepArry[2]=0;
  if(endS_X&&limit(1,1)&&!stepArry[3]) stepArry[0]=0;
  if(endS_Y&&limit(2,1)&&!stepArry[4]) stepArry[1]=0;
  if(endS_Z&&limit(3,1)&&!stepArry[5]) stepArry[2]=0;
  digitalWrite(stepX,stepArry[0]);
  digitalWrite(stepY,stepArry[1]);
  digitalWrite(stepZ,stepArry[2]);
  digitalWrite(dirX,stepArry[3]);
  digitalWrite(dirY,stepArry[4]);
  digitalWrite(dirZ,stepArry[5]);
  if(stepArry[0]&&stepArry[3]) RposX=RposX-1;
  if(stepArry[0]&&!stepArry[3]) RposX=RposX+1;
  if(stepArry[1]&&stepArry[4]) RposY=RposY-1;
  if(stepArry[1]&&!stepArry[4]) RposY=RposY+1;
  if(stepArry[2]&&stepArry[5]) RposZ=RposZ-1;
  if(stepArry[2]&&!stepArry[5]) RposZ=RposZ+1;
  delayMicroseconds(t);
  digitalWrite(stepX,LOW);
  digitalWrite(stepY,LOW);
  digitalWrite(stepZ,LOW);
  delayMicroseconds(t);
}

boolean limit(int axis, boolean h_e){
  switch(axis){
    case 1: 
      if(h_e) return !digitalRead(endX);
      else return !digitalRead(homeX);
    case 2: 
      if(h_e) return !digitalRead(endY);
      else return !digitalRead(homeY);
    case 3: 
      if(h_e) return !digitalRead(endZ);
      else return !digitalRead(homeZ);
    default:
      return 0;
  }
}

void home(){
  int temp=t;
  stepArry[1]=0;
  stepArry[2]=0;
  stepArry[0]=1; //Home X
  stepArry[3]=1;
  if(hS_X){
    while(!limit(1,0)){
      step();
    }
    stepArry[3]=0;;
    t=t*2;
    for(int i=0;i<mmX*5;i++){
      step();
    }
    stepArry[3]=1;
    while(!limit(1,0)){
      step();
    }
    stepArry[0]=0;
  }
  if(hS_Y){
    t=temp;
    stepArry[1]=1; //Home Y
    stepArry[4]=1;
    while(!limit(2,0)){
      step();
    }
    stepArry[4]=0;
    t=t*2;
    for(int i=0;i<mmY*5;i++){
      step();
    }
    stepArry[4]=1;
    while(!limit(2,0)){
      step();
    }
    stepArry[1]=0;
  }
  if(hS_Z){
    t=temp;
    stepArry[2]=1;
    stepArry[5]=0;
    while(!limit(3,0)){
      step();
    }
    stepArry[5]=1;
    t=t*2;
    for(int i=0;i<mmZ*5;i++){
      step();
    }
    stepArry[5]=0;
    while(!limit(3,0)){
      step();
    }
    t=temp;
  }
  RposX=0;
  RposY=0;
  t=temp;
}

int sig(long var){
  if(var>0) return 1;
  else if(var<0) return -1;
  return 0;
}
int line(float movX, float movY){

  long dX = movX-RposX;
  long dY = movY-RposY;
  int incx = sig(dX);
  int incy = sig(dY);
  int pdx,pdy,ddx,ddy;
  float err,el,es;
  
  if (dX<0) dX=-dX;
  if (dY<0) dY=-dY;
  if (dX>dY){
    pdx=incx;
    pdy=0;
    ddx=incx;
    ddy=incy;
    es=dY;
    el=dX;
  }
  else{
    pdx=0;
    pdy=incy;
    ddx=incx;
    ddy=incy;
    es=dX;
    el=dY;
  }
  err=el*0.5;
  
  for(long i=0;i<el;i++){
    err=err-es;
    if(err<0){
      err=err+el;
      if(ddx>0) stepArry[3]=0;
      else if(ddx<0) stepArry[3]=1;
      if(ddy>0) stepArry[4]=0;
      else if(ddy<0) stepArry[4]=1;
      stepArry[0]=abs(ddx);
      stepArry[1]=abs(ddy);
    }
    else{
      if(pdx>0) stepArry[3]=0;
      else if(pdx<0) stepArry[3]=1;
      if(pdy>0) stepArry[4]=0;
      else if(pdy<0) stepArry[4]=1;
      stepArry[0]=abs(pdx);
      stepArry[1]=abs(pdy);
    }
    step();
  }
  stepArry[0]=0;
  stepArry[1]=0;
  //if(!Z_mode || Z_mode) setLaser(laserMin); // note: change later 
  return 0;
}

float atan3(float dx,float dy){
  float a=atan2(dy,dx);
  if(a<0) a=(PI*2.0)+a;
  return a;
}

void arc(float cx,float cy,float x,float y,float dir){
  float dx = RposX-cx;
  float dy = RposY-cy;
  
  float radius=sqrt(dx*dx+dy*dy);
  float angle1 = atan3(dy,dx);
  float angle2 = atan3(y-cy,x-cx);
  float sweep = angle2-angle1;
  
  if(dir==1 && sweep<0) angle2+=2*PI;
  else if(dir==0 && sweep>0) angle1+=2*PI;
  sweep=angle2-angle1;
  float len = abs(sweep) * radius;
  int i;
  int segments = abs(floor( len / CM_PER_SEGMENT ));
  float newX, newY,angle3, fraction;
  
  for(i=0;i<segments;i++){
    fraction = ((float)i)/((float)segments);
    angle3 = (sweep * fraction)+angle1;
    newX = cx + sin(angle3)*radius;
    newY = cy + cos(angle3)*radius;
    line(newX,newY);
    }
  line(x,y);
}

/*void homeZero(){
    boolean done=0;
    boolean aX,aY,aZ,bX,bY,bZ,cX,cY,cZ = 0;
    long i = 0;
    long j = 0;
    long k = 0;
    while(!done){
    //  if(hS_X){
        Serial.println("1");
        if(!digitalRead(homeX)&&!aX&&!bX){
          stepArry[0]=1;
          stepArry[3]=1;
        }
        if(digitalRead(homeX)&&!aX&&!bX){
          stepArry[0]=1;
          stepArry[3]=0;
          aX=1;
        }
        if(i<(mmX*5)&&aX&&!bX){
          i++;
        }
        if(i>=(mmX*5)&&aX&&!bX){
          stepArry[0]=1;
          stepArry[3]=1;
          bX=1;
        }
        if(bX==1&&digitalRead(homeX)){
          cX=1;
        }
     // } else cX=1;
     // if(hS_Y){
        Serial.println("2");
        if(!digitalRead(homeY)&&!aY&&!bY){
          stepArry[1]=1;
          stepArry[4]=1;
        }
        if(digitalRead(homeY)&&!aY&&!bY){
          stepArry[1]=1;
          stepArry[4]=0;
          aY=1;
        }
        if(j<850&&aY&&!bY){
          j++;
        }
        if(j>=850&&aY&&!bY){
          stepArry[1]=1;
          stepArry[4]=1;
          bY=1;
        }
        if(bY==1&&digitalRead(homeY)){
          cY=1;
        }
     // } else cY=1;
     // if(hS_Z){
        Serial.println("1");
        if(!digitalRead(homeZ)&&!aZ&&!bZ){
          stepArry[2]=1;
          stepArry[5]=1;
        }
        if(digitalRead(homeZ)&&!aZ&&!bZ){
          stepArry[2]=1;
          stepArry[5]=0;
          aZ=1;
        }
        if(k<(mmZ*5)&&aZ&&!bZ){
          k++;
        }
        if(k>=(mmZ*5)&&aZ&&!bZ){
          stepArry[2]=1;
          stepArry[5]=1;
          bZ=1;
        }
        if(bZ==1&&digitalRead(homeZ)){
          cZ=1;
        }
   //   } else cZ=1;
      Serial.println("3");
      if(cX&&cY){
        done=1;
      }
      step();
    }
    Serial.println("4");
    RposX=0;
    RposY=0;
    RposZ=0;
}*/

