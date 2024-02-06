#include <iostream>

#include <cmath>
#define RAY_STEP 0.01
namespace almoast3dRendering{
	class coloredChar {
		public:
			char character;
			unsigned short int color;
			coloredChar(char crt = ' ', unsigned short int clr = 0) : character(crt), color(clr) {};
	};
	class twoDTexture {
		public:
			coloredChar* texture;
			unsigned short int height;
			twoDTexture(coloredChar* t = NULL, unsigned short int h = 0) : texture(t), height(h) {};
	};
	class mapElement {
		public:
			twoDTexture texture;
			bool walkable;
			bool seeThroughable;
			mapElement(twoDTexture ttr, bool w, bool s) : texture(ttr), walkable(w), seeThroughable(s) {};
			static twoDTexture defaultTexture;
			mapElement() : texture(mapElement::defaultTexture), walkable(false), seeThroughable(true) {};
	};
	twoDTexture mapElement::defaultTexture = {texture:NULL,height:0};
	class map {
			mapElement*** elements;
		public:
			typedef unsigned int wh_t;
			typedef long double wh_subi_t;
			wh_t width, height;
			map() : width(0), height(0) {};
			map(wh_t w, wh_t h);
			~map();
			class renderer;
	};
	
	map::map(map::wh_t w, map::wh_t h) : width(w), height(h) {
		coloredChar wallTextureElement1 = coloredChar('X', 128);
		coloredChar wallTextureElement2 = coloredChar('X', 8);
		coloredChar wallTexture[4] = {	wallTextureElement1,
										wallTextureElement2,
										wallTextureElement1,
										wallTextureElement2};
		twoDTexture wall2dTexture(wallTexture, 4);
		coloredChar* floorTexture = NULL;
		twoDTexture floor2dTexture(floorTexture, 4);
				
		this->elements = new mapElement**[this->height];
		for(wh_t i = 0; i < this->height; ++i) {
			this->elements[i] = new mapElement*[this->width];
			for(wh_t j = 0; j < this->width; ++j) {
				mapElement* thisElement = new mapElement;
				if(i == 0 || i == this->height-1 || j == 0 || j == this->width-1) {
					thisElement->texture = wall2dTexture;
					thisElement->seeThroughable = false;
					thisElement->walkable = false;
				} else {
					thisElement->texture = floor2dTexture;
					thisElement->seeThroughable = true;
					thisElement->walkable = true;
				}
				this->elements[i][j] = thisElement;
			}
		}
	}
	map::~map() {
		for(wh_t i = 0; i < this->height; ++i) {
			for(wh_t j = 0; j < this->width; ++j) {
				delete this->elements[i][j];
			}
			delete this->elements[i];
		}
		delete this->elements;
	}
	class map::renderer {
			map* thisMap;
			double degToRad(unsigned int deg) {return (double)deg * 3.14159265359 / 180;};
		protected:
			class ray {
					map::wh_subi_t x, y, deltaX, deltaY;
					void setAngle(double angleRad) {
						this->deltaX = cos(angleRad);
						this->deltaY = sin(angleRad);
					};
				public:
					ray(map::wh_subi_t sx, map::wh_subi_t sy, double angleRad, double step) : x(sx), y(sy) {
						this->setAngle(angleRad);
						this->deltaX *= step;
						this->deltaY *= step;
					}
					void step() {
						this->x += this->deltaX;
						this->y += this->deltaY;
					}
					
					friend map::renderer;
			};
			mapElement* checkOverlap(ray r) {
				if(this->thisMap->elements[(wh_t)r.y][(wh_t)r.y]->seeThroughable) return this->thisMap->elements[(wh_t)r.y][(wh_t)r.y];
				return NULL;
			}
		public:
			renderer(map* ThisMap) : thisMap(ThisMap) {};
			coloredChar** render(wh_subi_t const x, wh_subi_t const y, wh_t const width, wh_t const height, unsigned int rotationDeg, unsigned short int FOVDeg);
	};
	coloredChar** map::renderer::render(wh_subi_t const x, wh_subi_t const y, wh_t const width, wh_t const height, unsigned int rotationDeg, unsigned short int FOVDeg) {
		coloredChar** screen = new coloredChar*[width];
		unsigned int FOVStartDeg = (rotationDeg - FOVDeg/2 + 360)%360, FOVEndDeg = (rotationDeg + FOVDeg/2) % 360;
		double delta = (FOVStartDeg - FOVEndDeg) / width;
		
		double currentFOV;
		mapElement* hitTarget;
		unsigned short int startY, endY;
		for(unsigned int i = 0; i < width; ++i) {
			screen[i] = new coloredChar[height];
			
			currentFOV = this->degToRad((double)FOVStartDeg + delta*i);\
			
			// do tha Ray Tracing Pro 2D Gamer stuff here
			// and save the first encountered mapElement with seeThroughable set to false in hitTarget
			ray r = ray(x, y, currentFOV, RAY_STEP);
			hitTarget = NULL;
			while(hitTarget == NULL) {
				hitTarget = this->checkOverlap(r);
				r.step();
			}
			
			startY = height/2 - hitTarget->texture.height / 2;
			endY = height/2 + hitTarget->texture.height / 2 + (hitTarget->texture.height%2);
			for(unsigned short int j = 0; j < height; ++j) {
				if(j < startY || j > endY) screen[j][i] = coloredChar();
				else screen[j][i] = hitTarget->texture.texture[j - startY];
			}
		}
		
		return screen;
	}
}

using namespace almoast3dRendering;

#include <windows.h>
int main(int argc, char** argv) {
	  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  // you can loop k higher to see more color choices
  for(int k = 1; k < 255; k++)
  {
    // pick the colorattribute k you want
    SetConsoleTextAttribute(hConsole, k);
    std::cout << k << " I want to be nice today!\n";
  }
	
	return 0;
}
