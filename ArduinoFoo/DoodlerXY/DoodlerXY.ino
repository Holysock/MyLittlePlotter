/*
=============================================================================
=============================================================================
=                                                                           =
=                    Doodler V0.95g 9.Oct 2015                              =
=                   by Nick S. Lemberger aka Kira                           =
=                      nick-lemberger@live.de                               =
=                                                                           =
=============================================================================
=============================================================================
*/
int stepX = 2; //pinout
int stepY = 3;
int dirX = 4;
int dirY = 5;
int enable =6;
int Xmode0 = A0;
int Xmode1 = A1;
int Xmode2 = A2;
int Ymode0 = 7;
int Ymode1 = 8;
int Ymode2 = 9;
int homeX = 12; 
int homeY = 11; 
boolean stepArry[4] = {0,0,0,0};
int t = 250; //time between steps. DO NOT CHANGE
String a;
float RposX; //Your current Xposition in steps
float RposY; //             Yposition
float mm = 6400; //step/mm ratio
long endPosX = 528000; // Area in steps X
long endPosY = 960000; //               Y
int XmodeInverse;
int YmodeInverse;
long CM_PER_SEGMENT = 1000;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Nyello! :3"));
  Serial.println(F("Doodler V9.95 8.Oct 2015 by Nick S. Lemberger, at your service."));
  Serial.println(F("endPosX = 300mm endPosY = 165mm."));
  pinMode(stepX,OUTPUT);
  pinMode(stepY,OUTPUT);
  pinMode(dirX,OUTPUT);
  pinMode(dirY,OUTPUT);
  pinMode(Xmode0,OUTPUT);
  pinMode(Xmode1,OUTPUT);
  pinMode(Xmode2,OUTPUT);
  pinMode(Ymode0,OUTPUT);
  pinMode(Ymode1,OUTPUT);
  pinMode(Ymode2,OUTPUT);
  pinMode(homeX,INPUT);
  pinMode(homeY,INPUT);
  power(0);
  Serial.println(F("Motors are on."));
  setModeX(2,1);
  setModeY(2,1);
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
  if(digitalRead(homeX)&&stepArry[2]) stepArry[0]=0;
  if(digitalRead(homeY)&&stepArry[3]) stepArry[1]=0;
  digitalWrite(stepX,stepArry[0]);
  digitalWrite(stepY,stepArry[1]);
  digitalWrite(dirX,stepArry[2]);
  digitalWrite(dirY,stepArry[3]);
  if(stepArry[0]&&stepArry[2]) RposX=RposX-1*XmodeInverse;
  if(stepArry[0]&&!stepArry[2]) RposX=RposX+1*XmodeInverse;
  if(stepArry[1]&&stepArry[3]) RposY=RposY-1*YmodeInverse;
  if(stepArry[1]&&!stepArry[3]) RposY=RposY+1*YmodeInverse;
  delayMicroseconds(t);
  digitalWrite(2,LOW);
  digitalWrite(3,LOW);
  delayMicroseconds(t);
}
void getSerial(){ 
  if(Serial.available()<=0) return;
  if(Serial.peek()==71){
      int g = Serial.parseInt();
      if(g==1||g==0){
      float eX = Serial.parseFloat()*mm;
      float eY = Serial.parseFloat()*mm;
      line(eX,eY);
      }
      else if(g==2||g==3){
      float eX = Serial.parseFloat()*mm;
      float eY = Serial.parseFloat()*mm;
      float i = Serial.parseFloat()*mm;
      float j = Serial.parseFloat()*mm;
      boolean dir;
      if(g==3) dir = 0;
      else dir = 1;
      arc(RposX+i,RposY+j,eX,eY,dir);
      }
    Serial.println(F("Done."));
    Serial.readString();
    return;
  }
  a = Serial.readString();
  if(a=="pos"){
    Serial.println("Please enter X,Y");
    while(Serial.available()<1){}
    float posX = Serial.parseFloat()*mm; 
    float posY = Serial.parseFloat()*mm;
    if(posX > endPosX || posX < 0 || posY > endPosY || posY < 0){
      Serial.print("Error: pos_out_of_field! Values must be > 0 and < " );
      Serial.print(endPosX/mm);
      Serial.print(" for X axis and ");
      Serial.print(endPosY/mm);
      Serial.println("for Y axis");
      return;
    }
    Serial.print("Moving to X: ");
    Serial.print(posX/mm);
    Serial.print(" Y: ");
    Serial.println(posY/mm);
    int err=line(posX,posY);
    if(err==0) Serial.println(F("Done."));
    else if(err==1) Serial.println(F("Interrupted."));
    else if(err==2) Serial.println(F("Error: outside_of_field! Type 'home' and 'homeEnd' to measure the field."));
    return;
  }
  else if(a=="time"){
    Serial.println(F("Time in Microseconds"));
    while(Serial.available()<1){}
    t = Serial.parseInt();
    Serial.print(F("Time seto to: "));
    Serial.println(t);  
  }
  else if(a=="w"){
    stepArry[0]=1;
    stepArry[2]=0;
    Serial.println(F("Moving +X"));   
  }
  else if(a=="s"){
    stepArry[0]=1;
    stepArry[2]=1;
    Serial.println(F("Moving -X"));  
  }
  else if(a=="a"){
    stepArry[1]=1;
    stepArry[3]=0; 
    Serial.println(F("Moving -Y")); 
  }
  else if(a=="d"){
    stepArry[1]=1;
    stepArry[3]=1;
    Serial.println(F("Moving +Y"));  
  }
  else if(a=="stop" || a=="x"){
    stepArry[0]=0;
    stepArry[1]=0;
    Serial.println(F("Stopped. Type 'w' 's' 'a' 'd' to move")); 
  }
  else if(a=="home"){
    homeZero(4);
    Serial.println(("PosX = 0, PosY = 0"));
  }
  else if(a=="where"){
    Serial.print(F("PosX = "));
    Serial.println(RposX/mm);
    Serial.print(F("PosY = "));
    Serial.println(RposY/mm);
  }
  else if(a=="thisIsHome"){
    RposX=0;
    RposY=0;
    Serial.println(F("Done."));
  }
  else if(a=="test"){
    Serial.println(F("test"));
  }
  else if(a=="on"){
    power(0);
    Serial.println(F("Motors are on."));
  }
  else if(a=="off"){
    power(1);
    Serial.println(F("Motors are off."));
  }
  else if(a=="circle") circle();
  else Serial.println(F("Unkown command."));
  a="none";
} 
//################################################################################################
void homeZero(int){
    boolean done=0;
    boolean aX=0;
    boolean aY=0;
    boolean bX=0;
    boolean bY=0;
    boolean cX=0;
    boolean cY=0;
    int i=0;
    int j=0;
    setModeX(2,1);
    setModeY(2,1);
    while(!done){
      if(!digitalRead(homeX)&&!aX&&!bX){
        stepArry[0]=1;
        stepArry[2]=1;
      }
      if(digitalRead(homeX)&&!aX&&!bX){
        stepArry[0]=1;
        stepArry[2]=0;
        aX=1;
      }
      if(i<850&&aX&&!bX){
        i++;
      }
      if(i>=850&&aX&&!bX){
        stepArry[0]=1;
        stepArry[2]=1;
        setModeX(4,1);
        bX=1;
      }
      if(bX==1&&digitalRead(homeX)){
        cX=1;
      }
      if(!digitalRead(homeY)&&!aY&&!bY){
        stepArry[1]=1;
        stepArry[3]=1;
      }
      if(digitalRead(homeY)&&!aY&&!bY){
        stepArry[1]=1;
        stepArry[3]=0;
        aY=1;
      }
      if(j<850&&aY&&!bY){
        j++;
      }
      if(j>=850&&aY&&!bY){
        stepArry[1]=1;
        stepArry[3]=1;
        setModeY(4,1);
        bY=1;
      }
      if(bY==1&&digitalRead(homeY)){
        cY=1;
      }
      if(cX&&cY){
        done=1;
      }
      step();
    }
    setModeX(2,1);
    setModeY(2,1);
    RposX=0;
    RposY=0;
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
      if(ddx>0) stepArry[2]=0;
      else if(ddx<0) stepArry[2]=1;
      if(ddy>0) stepArry[3]=0;
      else if(ddy<0) stepArry[3]=1;
      stepArry[0]=abs(ddx);
      stepArry[1]=abs(ddy);
    }
    else{
      if(pdx>0) stepArry[2]=0;
      else if(pdx<0) stepArry[2]=1;
      if(pdy>0) stepArry[3]=0;
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

/*int gcode(int g, float sX, float sY, float eX, float eY, float mX, float mY){ //most important part
  if(g==1||g==0){
    movePos(sX,sY);
    movePos(eX,eY);
  }
  if(g==2){
    float i=0;
    float pX;
    float pY;
    //movePos(sX,sY);
    /*while(i<1){
      pY = sY+(eY-sY)*i;
      if(g==2) pY = sqrt(pow(sY-mX,2)+pow(sX-mY,2)-pow(pX-mX,2))+mY; // Thanks to Rebecca :3
      else pY = -sqrt(pow(sY-mX,2)+pow(sX-mY,2)-pow(pX-mX,2))+mY;    
      movePos(pX,abs(pY)); 
      i=i+0.001;
    }
     //movePos(eX,eY);
     while(1){
     movePos(sX+sqrt(pow(mX,2)+pow(mY,2))*sin(i),sY+sqrt(pow(mX,2)+pow(mY,2))*cos(i));
     i=i+0.0025;
     }
     Serial.println("done");
     return 0;
  }
}*/
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
  Serial.println(angle1);
  Serial.println(angle2);
  //Serial.println(sweep);
  if(dir==1 && sweep<0) angle2+=2*PI;
  else if(dir==0 && sweep>0) angle1+=2*PI;
  sweep=angle2-angle1;
  Serial.println(sweep);
  float len = abs(sweep) * radius;
  Serial.println(len);
  int i;
  int num_segments = abs(floor( len / CM_PER_SEGMENT ));
  float nx, ny,angle3, fraction;
  Serial.println("Here");
  Serial.println(num_segments);
  for(i=0;i<num_segments;i++){
    fraction = ((float)i)/((float)num_segments);
    angle3 = (sweep * fraction)+angle1;
    nx = cx + sin(angle3)*radius;
    ny = cy + cos(angle3)*radius;
    //Serial.println(nx);
    //Serial.println(ny);
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
void circle(){
  for(float i; i<1000; i=i+0.05){
    line(50*mm+sin(i)*30*mm,50*mm+cos(i)*30*mm);
  }
  stepArry[0]=0;
  stepArry[1]=0;
}

/*
#################################################
        OLD STUFF
#################################################

/*
  else if(a.charAt(0)=='m' &&a.charAt(1)=='o' && a.charAt(2)=='d' &&a.charAt(3)=='e'){
    int currentNumber = 0;
    if(a.charAt(4)=='X'){
      int currentNumber = 0;
      if(isDigit(a.charAt(6))){
        currentNumber = a.charAt(6)-'0';
        if(isDigit(a.charAt(7))) currentNumber = currentNumber * 10 + a.charAt(7)-'0';
      }
      setModeX(currentNumber,1);
    } 
    else if(a.charAt(4)=='Y'){
      if(isDigit(a.charAt(6))){
        currentNumber = a.charAt(6)-'0';
        if(isDigit(a.charAt(7))) currentNumber = currentNumber * 10 + a.charAt(7)-'0';
      }
      setModeY(currentNumber,1);
    } 
  }*/
  
/*
int movePos(float movX, float movY){
  while(abs(RposX-movX)>=1||abs(RposY-movY)>=1){
    if(Serial.available()>0) {
      if(Serial.readString()=="x"){
        stepArry[0]=0;
        stepArry[1]=0;
        return 1;
      }
    }
    //Serial.println(RposX,RposY);
    if(RposX<movX){
      stepArry[0]=1;
      stepArry[2]=0;
    }
    else if(RposX>movX){
      stepArry[0]=1;
      stepArry[2]=1;
    }
    if(RposY<movY){
      stepArry[1]=1;
      stepArry[3]=0;
    }
    else if(RposY>movY){
      stepArry[1]=1;
      stepArry[3]=1;
    }
    if(abs(RposX-movX)<1){
      stepArry[0]=0;
    }
    else if(abs(RposX-movX)<32 && XmodeInverse>1){
      setModeX(32,0);
    }
    if(abs(RposY-movY)<1){
      stepArry[1]=0;
    }
    else if(abs(RposY-movY)<32 && YmodeInverse>1){
      setModeY(32,0);
    }
    if((digitalRead(homeX)&&stepArry[2])||(digitalRead(homeY)&&stepArry[3])) return 2;
    step();
  }
  stepArry[0]=0;
  stepArry[1]=0;
  setModeX(2,0);
  setModeY(2,0);
  return 0;
}*//*
int movePos(float movX, float movY){
  while(abs(RposX-movX)>=1||abs(RposY-movY)>=1){
    if(Serial.available()>0) {
      if(Serial.readString()=="x"){
        stepArry[0]=0;
        stepArry[1]=0;
        return 1;
      }
    }
    //Serial.println(RposX,RposY);
    if(RposX<movX){
      stepArry[0]=1;
      stepArry[2]=0;
    }
    else if(RposX>movX){
      stepArry[0]=1;
      stepArry[2]=1;
    }
    if(RposY<movY){
      stepArry[1]=1;
      stepArry[3]=0;
    }
    else if(RposY>movY){
      stepArry[1]=1;
      stepArry[3]=1;
    }
    if(abs(RposX-movX)<1){
      stepArry[0]=0;
    }
    else if(abs(RposX-movX)<32 && XmodeInverse>1){
      setModeX(32,0);
    }
    if(abs(RposY-movY)<1){
      stepArry[1]=0;
    }
    else if(abs(RposY-movY)<32 && YmodeInverse>1){
      setModeY(32,0);
    }
    if((digitalRead(homeX)&&stepArry[2])||(digitalRead(homeY)&&stepArry[3])) return 2;
    step();
  }
  stepArry[0]=0;
  stepArry[1]=0;
  setModeX(2,0);
  setModeY(2,0);
  return 0;
}

void circle(){
  for(float i; i<1000; i=i+0.05){
    line(endPosX*0.3+i*750+(i*500)*sin(i),endPosY*0.5+(i*500)*cos(i));
  }
  stepArry[0]=0;
  stepArry[1]=0;
}
*/
