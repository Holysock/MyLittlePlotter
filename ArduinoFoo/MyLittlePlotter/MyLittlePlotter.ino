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
int stepZ = 8;
int dirX = 4;
int dirY = 5;
int dirZ = 12;
int enable =6;
boolean hS_X = 1; //1 if there is a limit switch for respective axis
boolean hS_Y = 1;
boolean hS_Z = 0;
boolean endS_X = 0;
boolean endS_Y = 0;
boolean endS_Z = 0;
int homeX = 10; 
int homeY = 11; 
int homeZ;
int endX; 
int endY; 
int endZ;
boolean stepArry[6] = {0,0,0,0,0,0}; //X-step,X-dir,Y-step,Y-dir,Z-step,Z-dir
int t = 10; //time between steps. Change with T[int]
float RposX; //Your current Xposition in steps
float RposY; //             Yposition
float RposZ; //             Zposition
float mmX = 1600; // step/mm ratio
float mmY = 1600;
float mmZ = 1600;
long endPosX = 528000; // Area in steps X
long endPosY = 960000; //               Y
long CM_PER_SEGMENT = 1000; //Factor for arc interpolation
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
  pinMode(enable,OUTPUT);
  if(hS_X) pinMode(homeX,INPUT_PULLUP);
  if(hS_Y) pinMode(homeY,INPUT_PULLUP);
  if(hS_Z) pinMode(homeZ,INPUT_PULLUP);
  if(endS_X) pinMode(endX,INPUT_PULLUP);
  if(endS_Y) pinMode(endY,INPUT_PULLUP);
  if(endS_Z) pinMode(endZ,INPUT_PULLUP);
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
      else if(g==4){
        delay(Serial.parseInt());
      }
      else if(g==28){
        home();
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
    stepArry[4]=1; 
    Serial.println(F("Moving -Y")); 
  }
  else if(a=="d"){
    stepArry[1]=1;
    stepArry[4]=0;
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
  else if(a=="q" || a=="x"){
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
  stepArry[1]=0;
  stepArry[2]=0;
  stepArry[0]=1; //Home X
  stepArry[3]=1;
  if(hS_X){
    while(!limit(1,0)){
      step();
    }
    stepArry[3]=0;
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
    stepArry[1]=1; //Home Y
    stepArry[4]=1;
    while(!limit(2,0)){
      step();
    }
    stepArry[4]=0;
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
    stepArry[2]=1;
    stepArry[5]=0;
    while(!limit(3,0)){
      step();
    }
    stepArry[5]=1;
    for(int i=0;i<mmZ*5;i++){
      step();
    }
    stepArry[5]=0;
    while(!limit(3,0)){
      step();
    }
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
