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

	void createBlankTexture(uint32_t width, uint32_t height);

	void drawGlyph(word_t x, word_t y, word_t word);

	std::vector<uint8_t> _pixelBuffer;

	std::vector<word_t> _characterFont;

	std::vector<word_t> _characterMap;

	word_t _memOffset;

	uint16_t _cellsHeight;
	uint16_t _cellsWidth;
	uint16_t _cellHeight;
	uint16_t _cellWidth;

	uint16_t _screenHeight;
	uint16_t _screenWidth;

	bool _imageDirty;


	GLFWwindow* _window;

	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;

	GLuint _texture;

	GLuint _vertexShader;
	GLuint _fragmentShader;
	GLuint _shaderProgram;
};

