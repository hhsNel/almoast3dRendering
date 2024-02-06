#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

ifstream input("input.txt");
unsigned int mapSize = 128, mapW = 128, mapH = 128;


string map[128];

const int rayN = 64, vertN = 14, displayW = 64, displayH = 16;
double viewAngle = 3.14*0.125, speed = 5, turningSpeed = 3.14*0.125, zoom = 0.1;
bool live = true;
string ascii = "@%8#+=;:,....................";

struct vec2{
  double x;
  double y;
  vec2(double tX = 0, double tY = 0):x(tX), y(tY){};
};

vec2 fromAngle(double angle){
  vec2 temp;
  temp.x = sin(angle);
  temp.y = cos(angle);
  return temp;
}

double dist2D(double x1, double y1, double x2, double y2){
  double x = abs(x1 - x2);
  double y = abs(y1 - y2);
  double dist = sqrt(x*x + y*y);
  return dist;
}

void mapLoad(string filePath){
  if(input){cout<<"FileLoaded:)\n";}else{cout<<"FileNotFound:(\n";}
  for(int l = 0; l < 128; l++){
    input>>map[l];
    cout<<l<<"\t|\t"<<map[l]<<"\n";
  }
  input.close();
}

struct Vert{
  double x1, y1, x2, y2, color;
  Vert(double tempX1 = 0, double tempY1 = 0, double tempX2 = 0, double tempY2 = 0, int tempColor = 0){
    x1 = tempX1;
    y1 = tempY1;
    x2 = tempX2;
    y2 = tempY2;
    color = tempColor;
  }
};

Vert verts[vertN] = {0};

struct Ray{
  float x, y, dirX, dirY, vertId;
  vec2 intersect;
  Ray(float tempX = 0, float tempY = 0, float tempDirX = 0, float tempDirY = 0){
    x = tempX;
    y = tempY;
    dirX = tempDirX;
    dirY = tempDirY;
  }
};

struct Camera{
  float x, y, angle;
  Ray rays[rayN];
  Camera(float tempX = 0, float tempY = 0, float tempAngle = 0){
    x = tempX;
    y = tempY;
    angle = -3.1413*0.75;
    
    vec2 temp;
    
    for(int r = 0; r < rayN; ++r){
      temp = fromAngle(2*3.14/rayN*r*viewAngle - 3.14*viewAngle + angle);
      rays[r] = Ray(x, y, temp.x, temp.y);
    }
  }
  void move(){
    x = x>1?x:1;  //max(1, x);
    x = x<mapW?x:mapW;  //min(mapW - 1, x);
    y = y>1?y:1;  //max(1, y);
    y = y<mapH?y:mapH;  //min(mapH - 1, y);
    
    vec2 temp;
    for(int r = 0; r < rayN; ++r){
      temp = fromAngle(2*3.14/rayN*r*viewAngle - 3.14*viewAngle + angle);
      rays[r].dirX = temp.x;
      rays[r].dirY = temp.y;
    }
  }
  void cast(){
    for(int r = 0; r < rayN; ++r){
      vec2 cVI = vec2(mapW*mapH, mapW*mapH); //closest vert intersect
      //find intersect:
      for(int v =  0; v < vertN; ++v){
        double x1 = verts[v].x1;
        double y1 = verts[v].y1;
        double x2 = verts[v].x2;
        double y2 = verts[v].y2;
        double x3 = x;
        double y3 = y;
        double x4 = x + rays[r].dirX;
        double y4 = y + rays[r].dirY;
        double t = (((x1 - x3)*(y3 - y4))-((y1 - y3)*(x3 - x4)))/(((x1 - x2)*(y3 -y4))-((y1 - y2)*(x3 - x4)));
        double u =(((x1 - x3)*(y1 - y2))-((y1 - y3)*(x1 - x2)))/(((x1 - x2)*(y3 -y4))-((y1 - y2)*(x3 - x4)));
        
        vec2 P = vec2(x1 + t*(x2 - x1), y1 + t*(y2 - y1));
        
        if(t > 0 && u > 0 && t < 1 && dist2D(x, y, P.x, P.y) < dist2D(x, y, cVI.x, cVI.y)){
          cVI = P;
          rays[r].vertId = v;
        }
      }
      rays[r].intersect = vec2(cVI);
    }
  }
};

Camera cam = Camera(50, 50, 3.14);

void randomizeVerts(){
  int c;
  
  srand(time(NULL));
  
  c = rand()%7 + 1;
  verts[0] = Vert(0, 0, 0, mapH, c);
  c = rand()%7 + 1;
  verts[1] = Vert(0, mapH, mapW, mapH, c);
  c = rand()%7 + 1;
  verts[2] = Vert(mapW, mapH, mapW, 0, c);
  c = rand()%7 + 1;
  verts[3] = Vert(mapW, 0, 0, 0, c);
  for(int v = 4; v < vertN; v++){
    double x1 = rand()%(mapW - 2) + 1;
    double y1 = rand()%(mapH - 2) + 1;
    double x2 = rand()%(mapW - 2) + 1;
    double y2 = rand()%(mapH - 2) + 1;
    c = rand()%7 + 1;
    verts[v] = Vert(x1, y1, x2, y2, c);
  }
}

void keyPressed(){
  char k = getch();
  switch(k){
    case 'w':
      cam.x += sin(cam.angle)*speed;
      cam.y += cos(cam.angle)*speed;
      break;
    case 'a':
      cam.angle -= turningSpeed;
      break;
    case 's':
      cam.x -= sin(cam.angle)*speed;
      cam.y -= cos(cam.angle)*speed;
      break;
    case 'd':
      cam.angle += turningSpeed;
      break;
    case 'e':
      live = false;
      break;
  }
}

void display3DAscii(){
  char asciiDisplay[displayW][displayH] = {0};
  int asciiDisplayColors[displayW][displayH] = {0};
  int color = 0;
  for(int r = 0; r < rayN; ++r){
    vec2 in = cam.rays[r].intersect;
    for(int y = int(dist2D(cam.x, cam.y, in.x, in.y)*zoom); y < displayH - int(dist2D(cam.x, cam.y, in.x, in.y)*zoom); ++y){
    //for(int y = 0; y < displayH; ++y){
      int c = int(dist2D(cam.x, cam.y, in.x, in.y)/255*ascii.length());
      asciiDisplay[displayW*r/rayN][y] = ascii[c < ascii.length() - 1?c:ascii.length() - 1];
      asciiDisplayColors[displayW*r/rayN][y] = verts[int(cam.rays[r].vertId)].color;
    }
  }
  for(int y = 0; y < displayH; ++y){
    for(int x = 0; x < displayW; ++x){
      if(asciiDisplay[x][y] == char(0)){
        SetConsoleTextAttribute(hConsole, 8);
        cout<<" ";
      }else{
        SetConsoleTextAttribute(hConsole, asciiDisplayColors[x][y]);
        cout<<asciiDisplay[x][y];
      }
    }
    cout<<"\n";
  }
}

/*int main(){
  //srand(time(NULL));
  
  randomizeVerts();
  
  for(int v = 0; v < vertN; v++){
    cout<<verts[v].x1<<":"<<verts[v].y1<<":"<<verts[v].x2<<":"<<verts[v].y2<<"\n";
  }
  
  //mapLoad("input.txt");
  //updateVerts();
  while(live){
    keyPressed();
    system("cls");
    cout<<"input\n";
    cout<<"system cls\n";
    cam.move();
    cout<<"cam move\n";
    cam.cast();
    cout<<"cam cast\n";
    display3DAscii();
    
    SetConsoleTextAttribute(hConsole, 7);
    cout<<"camera position: "<<cam.x<<", "<<cam.y<<"\n";
    cout<<"camera angle: "<<cam.angle<<"\n";
    cout<<""<<cam.rays[0].intersect.x<<", "<<cam.rays[0].intersect.x<<"\n";
  }
  
  getch();
}*/
