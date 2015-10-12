/*
=============================================================================
=============================================================================
=                                                                           =
=             MyLittlePlotter V0.95 APLHA 11.Oct 2015                       =
=                   by Nick S. Lemberger aka Holysock                       =
=                      nick-lemberger@live.de                               =
=                                                                           =
=============================================================================
=============================================================================
*/
int stepX = 2; //pinout
int stepY = 3;
int stepZ = 10;
int dirX = 4;
int dirY = 5;
int dirZ = 12;
int enable =6;
int Xmode0 = A0;
int Xmode1 = A1;
int Xmode2 = A2;
int Ymode0 = 7;
int Ymode1 = 8;
int Ymode2 = 9;
boolean hS_X = 1; //1 if there is a limit switch for respective axis
boolean hS_Y = 1;
boolean hS_Z = 0;
boolean endS_X = 0;
boolean endS_Y = 0;
boolean endS_Z = 0;
int homeX = 12; 
int homeY = 11; 
int homeZ;
int endX; 
int endY; 
int endZ;
boolean stepArry[6] = {0,0,0,0,0,0}; //X-step,X-dir,Y-step,Y-dir,Z-step,Z-dir
int t = 250; //time between steps. Change with T[int]
float RposX; //Your current Xposition in steps
float RposY; //             Yposition
float RposZ; //             Zposition
float mmX = 6400; // step/mm ratio
float mmY = 6400;
float mmZ = 3200;
long endPosX = 528000; // Area in steps X
long endPosY = 960000; //               Y
int XmodeInverse; // =16;
int YmodeInverse; // =16;
int ZmodeInverse; // =16;
long CM_PER_SEGMENT = 1000; //do not change!
String a;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Nyello! :3"));
  Serial.println(F(" MyLittlePlotter V0.95 APLHA 11.Oct 2015 by Nick S. Lemberger aka Holysock."));
  pinMode(stepX,OUTPUT);
  pinMode(stepY,OUTPUT);
  pinMode(stepZ,OUTPUT);
  pinMode(dirX,OUTPUT);
  pinMode(dirY,OUTPUT);
  pinMode(dirZ,OUTPUT);
  pinMode(Xmode0,OUTPUT);
  pinMode(Xmode1,OUTPUT);
  pinMode(Xmode2,OUTPUT);
  pinMode(Ymode0,OUTPUT);
  pinMode(Ymode1,OUTPUT);
  pinMode(Ymode2,OUTPUT);
  pinMode(enable,OUTPUT);
  if(hS_X) pinMode(homeX,INPUT);
  if(hS_Y) pinMode(homeY,INPUT);
  if(hS_Z) pinMode(homeZ,INPUT);
  if(endS_X) pinMode(endX,INPUT);
  if(endS_Y) pinMode(endY,INPUT);
  if(endS_Z) pinMode(endZ,INPUT);
  power(0);
  Serial.println(F("Motors are on."));
}
void loop() {
  getSerial(); 
  step();
} 
//#########################################################################################
void power(boolean on){
  digitalWrite(enable,on);
}
void step (){
  if(hS_X&&digitalRead(homeX)&&stepArry[3]) stepArry[0]=0;
  if(hS_Y&&digitalRead(homeY)&&stepArry[4]) stepArry[1]=0;
  if(hS_Z&&digitalRead(homeZ)&&stepArry[5]) stepArry[2]=0;
  if(endS_X&&digitalRead(endX)&&!stepArry[3]) stepArry[0]=0;
  if(endS_Y&&digitalRead(endY)&&!stepArry[4]) stepArry[1]=0;
  if(endS_Z&&digitalRead(endZ)&&!stepArry[5]) stepArry[2]=0;
  digitalWrite(stepX,stepArry[0]);
  digitalWrite(stepY,stepArry[1]);
  digitalWrite(stepZ,stepArry[2]);
  digitalWrite(dirX,stepArry[3]);
  digitalWrite(dirY,stepArry[4]);
  digitalWrite(dirZ,stepArry[5]);
  if(stepArry[0]&&stepArry[3]) RposX=RposX-1*XmodeInverse;
  if(stepArry[0]&&!stepArry[3]) RposX=RposX+1*XmodeInverse;
  if(stepArry[1]&&stepArry[4]) RposY=RposY-1*YmodeInverse;
  if(stepArry[1]&&!stepArry[4]) RposY=RposY+1*YmodeInverse;
  if(stepArry[2]&&stepArry[5]) RposZ=RposZ-1*ZmodeInverse;
  if(stepArry[2]&&!stepArry[5]) RposZ=RposZ+1*ZmodeInverse;
  delayMicroseconds(t);
  digitalWrite(stepX,LOW);
  digitalWrite(stepY,LOW);
  digitalWrite(stepZ,LOW);
  delayMicroseconds(t);
}
void getSerial(){ 
  if(Serial.available()<=0) return;
  if(Serial.peek()==71){
      int g = Serial.parseInt();
      if(g==1||g==0){
      float eX = Serial.parseFloat()*mmX;
      float eY = Serial.parseFloat()*mmY;
      float eZ = Serial.parseFloat()*mmZ;
      line(eX,eY);
      }
      else if(g==2||g==3){
      float eX = Serial.parseFloat()*mmX;
      float eY = Serial.parseFloat()*mmY;
      float i = Serial.parseFloat()*mmX;
      float j = Serial.parseFloat()*mmY;
      boolean dir;
      if(g==3) dir = 0;
      else dir = 1;
      arc(RposX+i,RposY+j,eX,eY,dir);
      }
      else if(g=4){
        delay(Serial.parseInt());
      }
      else if(g==28){
        homeZero();
      }
    Serial.println(F("Done."));
    Serial.readString();
    return;
  }
  if(Serial.peek()==84){
    t=Serial.parseInt();
    Serial.print("Time set to: ");
    Serial.println(t);
  }
  a = Serial.readString();
  if(a=="w"){
    stepArry[0]=1;
    stepArry[3]=0;
    Serial.println(F("Moving +X"));   
  }
  else if(a=="s"){
    stepArry[0]=1;
    stepArry[3]=1;
    Serial.println(F("Moving -X"));  
  }
  else if(a=="a"){
    stepArry[1]=1;
    stepArry[4]=0; 
    Serial.println(F("Moving -Y")); 
  }
  else if(a=="d"){
    stepArry[1]=1;
    stepArry[4]=1;
    Serial.println(F("Moving +Y"));  
  }
  else if(a=="r"){
    stepArry[2]=1;
    stepArry[5]=0;
    Serial.println(F("Moving +Z"));   
  }
  else if(a=="f"){
    stepArry[2]=1;
    stepArry[5]=1;
    Serial.println(F("Moving -Z"));  
  }
  else if(a=="x"){
    stepArry[0]=0;
    stepArry[1]=0;
    Serial.println(F("Stopped. Type 'w' 's' 'a' 'd' to move")); 
  }
  else if(a=="where"){
    Serial.print(F("PosX = "));
    Serial.println(RposX/mmX);
    Serial.print(F("PosY = "));
    Serial.println(RposY/mmY);
    Serial.print(F("PosZ = "));
    Serial.println(RposZ/mmZ);
  }
  else if(a=="thisIsHome"){  //note: in g-Befehl ändern
    RposX=0;
    RposY=0;
    Serial.println(F("Home Sweet Home"));
  }
  else if(a=="echo"){
    Serial.println(F("echo1"));
  }
  else if(a=="on"){
    power(0);
    Serial.println(F("Motors are on."));
  }
  else if(a=="off"){
    power(1);
    Serial.println(F("Motors are off."));
  }
  else Serial.println(F("Unkown command."));
  a="none";
} 
//################################################################################################
void homeZero(){
    boolean done=0;
    boolean aX,aY,aZ,bX,bY,bZ,cX,cY,cZ;
    long i = 0;
    long j = 0;
    long k = 0;
    while(!done){
      if(hS_X){
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
      } else cX=1;
      if(hS_Y){
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
      } else cY=1;
      if(hS_Z){
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
      } else cZ=1;
      if(cX&&cY&&cZ){
        done=1;
      }
      step();
    }
    RposX=0;
    RposY=0;
    RposZ=0;
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
  int modeInverse;
  if (dX>dY){
    pdx=incx;
    pdy=0;
    ddx=incx;
    ddy=incy;
    es=dY;
    el=dX;
    modeInverse = XmodeInverse;
  }
  else{
    pdx=0;
    pdy=incy;
    ddx=incx;
    ddy=incy;
    es=dX;
    el=dY;
    modeInverse = YmodeInverse;
  }
  err=el*0.5;
  for(long i=0;i<el;i=i+modeInverse){
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
      else if(pdy<0) stepArry[3]=1;
      stepArry[0]=abs(pdx);
      stepArry[1]=abs(pdy);
    }
    step();
  }
  stepArry[0]=0;
  stepArry[1]=0;
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
  int num_segments = abs(floor( len / CM_PER_SEGMENT ));
  float nx, ny,angle3, fraction;
  for(i=0;i<num_segments;i++){
    fraction = ((float)i)/((float)num_segments);
    angle3 = (sweep * fraction)+angle1;
    nx = cx + sin(angle3)*radius;
    ny = cy + cos(angle3)*radius;
    line(nx,ny);
    }
  line(x,y);
}
void setModeX(int m, boolean s){
  if(m == 1){
    digitalWrite(Xmode0,LOW);
    digitalWrite(Xmode1,LOW);
    digitalWrite(Xmode2,LOW);
    XmodeInverse = 32;
    if(s) Serial.println(F("X Microsteps set to: 1"));
  }  
  if(m == 2){
    digitalWrite(Xmode0,HIGH);
    digitalWrite(Xmode1,LOW);
    digitalWrite(Xmode2,LOW);
    XmodeInverse = 16;
    if(s) Serial.println(F("X Microsteps set to: 2"));
  }
  if(m == 4){
    digitalWrite(Xmode0,LOW);
    digitalWrite(Xmode1,HIGH);
    digitalWrite(Xmode2,LOW);
    XmodeInverse = 8;
    if(s) Serial.println(F("X Microsteps set to: 4"));
  }
  if(m == 8){
    digitalWrite(Xmode0,HIGH);
    digitalWrite(Xmode1,HIGH);
    digitalWrite(Xmode2,LOW);
    XmodeInverse = 4;
    if(s) Serial.println(F("X Microsteps set to: 8"));
  }
  if(m == 16){
    digitalWrite(Xmode0,LOW);
    digitalWrite(Xmode1,LOW);
    digitalWrite(Xmode2,HIGH);
    XmodeInverse = 2;
    if(s) Serial.println(F("X Microsteps set to: 16"));
  }
  if(m == 32){
    digitalWrite(Xmode0,HIGH);
    digitalWrite(Xmode1,HIGH);
    digitalWrite(Xmode2,HIGH);
    XmodeInverse = 1;
    if(s) Serial.println(F("X Microsteps set to: 32"));
  }
}
void setModeY(int m, boolean s){
  if(m == 1){
    digitalWrite(Ymode0,LOW);
    digitalWrite(Ymode1,LOW);
    digitalWrite(Ymode2,LOW);
    YmodeInverse = 32;
    if(s) Serial.println(F("Y Microsteps set to: 1"));
  }
  if(m == 2){
    digitalWrite(Ymode0,HIGH);
    digitalWrite(Ymode1,LOW);
    digitalWrite(Ymode2,LOW);
    YmodeInverse = 16;
    if(s) Serial.println(F("Y Microsteps set to: 2"));
  }
  if(m == 4){
    digitalWrite(Ymode0,LOW);
    digitalWrite(Ymode1,HIGH);
    digitalWrite(Ymode2,LOW);
    YmodeInverse = 8;
    if(s) Serial.println(F("Y Microsteps set to: 4"));
  }
  if(m == 8){
    digitalWrite(Ymode0,HIGH);
    digitalWrite(Ymode1,HIGH);
    digitalWrite(Ymode2,LOW);
    YmodeInverse = 4;
    if(s) Serial.println(F("Y Microsteps set to: 8"));
  }
  if(m == 16){
    digitalWrite(Ymode0,LOW);
    digitalWrite(Ymode1,LOW);
    digitalWrite(Ymode2,HIGH);
    YmodeInverse = 2;
    if(s) Serial.println(F("Y Microsteps set to: 16"));
  }
  if(m == 32){
    digitalWrite(Ymode0,HIGH);
    digitalWrite(Ymode1,HIGH);
    digitalWrite(Ymode2,HIGH);
    YmodeInverse = 1;
    if(s) Serial.println(F("Y Microsteps set to: 32"));
  }
}
void setModeZ(int m, boolean s){
  if(m == 1){
    ZmodeInverse = 32;
    if(s) Serial.println(F("Z Microsteps set to: 1"));
  }  
  if(m == 2){
    ZmodeInverse = 16;
    if(s) Serial.println(F("Z Microsteps set to: 2"));
  }
  if(m == 4){
    ZmodeInverse = 8;
    if(s) Serial.println(F("Z Microsteps set to: 4"));
  }
  if(m == 8){
    ZmodeInverse = 4;
    if(s) Serial.println(F("Z Microsteps set to: 8"));
  }
  if(m == 16){
    ZmodeInverse = 2;
    if(s) Serial.println(F("Z Microsteps set to: 16"));
  }
  if(m == 32){
    ZmodeInverse = 1;
    if(s) Serial.println(F("Z Microsteps set to: 32"));
  }
}
