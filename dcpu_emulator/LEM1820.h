#pragma once
#include "Device.h"

class LEM1820 :
	public Device
{
public:
	LEM1820();
	virtual ~LEM1820();

	bool init() override;
	void interrupt() override;
	void update() override;


private:
	void charBlit(const uint16_t *charmap, SDL_Surface *dst,
		SDL_Rect *dstrect, uint8_t chr, Uint32 color);

	SDL_Window* _window;
	SDL_Renderer* _renderer;

	SDL_Surface* _surface;
	SDL_Texture* _texture;

	word_t _memOffset;

	word_t _cellsHeight;
	word_t _cellsWidth;


};

