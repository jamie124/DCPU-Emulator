#include "stdafx.h"
#include "LEM1820.h"


LEM1820::LEM1820() :
	_window(nullptr),
	_renderer(nullptr),
	_texture(nullptr),
	_cellsHeight(32),
	_cellsWidth(12)
{
	_name = "LEM1802 - Low Energy Monitor";
	_identifier = 0x7349f615;
	_version = 0x1802;
	_manufacturer = 0x1c6c8b36;
}


LEM1820::~LEM1820()
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();

}

bool LEM1820::init()
{
	std::cout << "Initialising LEM1820" << std::endl;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_INIT Error: " << SDL_GetError() << std::endl;
		return false;
	}

	_window = SDL_CreateWindow("LEM1820", 100, 100, 640, 480, SDL_WINDOW_SHOWN);

	if (_window == nullptr) {
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();

		return false;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (_renderer == nullptr) {
		SDL_DestroyWindow(_window);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();

		return false;

	}

	/*
	auto surface = SDL_LoadBMP("test.bmp");

	
	if (surface != nullptr) {
		_texture = SDL_CreateTextureFromSurface(_renderer, surface);
		SDL_FreeSurface(surface);
	}
	*/

	return true;
}

void LEM1820::interrupt()
{
	 auto aVal = _cpu->getRegister(0);
	 auto bVal = _cpu->getRegister(1);

	 switch (aVal) {
	 case 0:
		 if (bVal == 0) {
			 std::cout << "Screen disconnected";
		 }
		 else {
			 _memOffset = bVal;
		 }
		 break;
	 case 1:
		 // TODO - Map font
		 break;
	 case 2:
		 // TODO: Map palette
		 break;
	 case 3:
		 // TODO: Border colour
		 break;
	 case 4:
		 // TODO: Mem dump font
		 break;
	 case 5:
		 // TODO: Dump palette
		 break;
	 }
}

void LEM1820::update()
{
	if (_renderer != nullptr) {
		SDL_RenderClear(_renderer);

		SDL_Event event;
		SDL_PollEvent(&event);

		for (int h = 0; h < _cellsHeight; ++h) {
			for (int w = 0; w < _cellsWidth; ++w) {

			}
		}
		
		//_texture = SDL_CreateTextureFromSurface(_renderer, _surface);
		
		//if (_texture != nullptr) {
			//SDL_RenderCopy(_renderer, _texture, NULL, NULL);
		//}

		SDL_RenderPresent(_renderer);
		//SDL_Delay(100);
	}
}

void LEM1820::charBlit(const uint16_t *charmap, SDL_Surface *dst, SDL_Rect *dstrect, uint8_t chr, Uint32 color)
{

}
