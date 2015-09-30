#include "stdafx.h"
#include "LEM1820.h"

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec2 position;"
"in vec3 color;"
"in vec2 texcoord;"
"out vec3 Color;"
"out vec2 Texcoord;"
"void main() {"
"   Color = color;"
"   Texcoord = texcoord;"
"   gl_Position = vec4(position, 0.0, 1.0);"
"}";
const GLchar* fragmentSource =
"#version 150 core\n"
"in vec3 Color;"
"in vec2 Texcoord;"
"out vec4 outColor;"
"uniform sampler2D textureLEM;"
"void main() {"
//" outColor = vec4(1, 0, 0, 1);"
"   outColor = texture(textureLEM, Texcoord);"
"}";


LEM1820::LEM1820() :

	_cellsHeight(12),
	_cellsWidth(32),
	_cellHeight(8),
	_cellWidth(4),

	_imageDirty(true)
{
	_name = "LEM1802 - Low Energy Monitor";
	_identifier = 0x7349f615;
	_version = 0x1802;
	_manufacturer = 0x1c6c8b36;

	_screenHeight = _cellsHeight * _cellHeight;
	_screenWidth = _cellsWidth * _cellWidth;
}


LEM1820::~LEM1820()
{

	glDeleteTextures(1, &_texture);

	glDeleteProgram(_shaderProgram);
	glDeleteShader(_fragmentShader);
	glDeleteShader(_vertexShader);


	glDeleteBuffers(1, &_ebo);
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);

	glfwTerminate();
	/*
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
	*/
}

bool LEM1820::init()
{
	std::cout << "Initialising LEM1820" << std::endl;

	_pixelBuffer.resize(_screenWidth * _screenHeight * 4);

	_characterFont = {

			0x000f, 0x0808, 0x080f, 0x0808, 0x08f8, 0x0808, 0x00ff, 0x0808,
				0x0808, 0x0808, 0x08ff, 0x0808, 0x00ff, 0x1414, 0xff00, 0xff08,
				0x1f10, 0x1714, 0xfc04, 0xf414, 0x1710, 0x1714, 0xf404, 0xf414,
				0xff00, 0xf714, 0x1414, 0x1414, 0xf700, 0xf714, 0x1417, 0x1414,
				0x0f08, 0x0f08, 0x14f4, 0x1414, 0xf808, 0xf808, 0x0f08, 0x0f08,
				0x001f, 0x1414, 0x00fc, 0x1414, 0xf808, 0xf808, 0xff08, 0xff08,
				0x14ff, 0x1414, 0x080f, 0x0000, 0x00f8, 0x0808, 0xffff, 0xffff,
				0xf0f0, 0xf0f0, 0xffff, 0x0000, 0x0000, 0xffff, 0x0f0f, 0x0f0f,
				0x0000, 0x0000, 0x005f, 0x0000, 0x0300, 0x0300, 0x3e14, 0x3e00,
				0x266b, 0x3200, 0x611c, 0x4300, 0x3629, 0x7650, 0x0002, 0x0100,
				0x1c22, 0x4100, 0x4122, 0x1c00, 0x2a1c, 0x2a00, 0x083e, 0x0800,
				0x4020, 0x0000, 0x0808, 0x0800, 0x0040, 0x0000, 0x601c, 0x0300,
				0x3e41, 0x3e00, 0x427f, 0x4000, 0x6259, 0x4600, 0x2249, 0x3600,
				0x0f08, 0x7f00, 0x2745, 0x3900, 0x3e49, 0x3200, 0x6119, 0x0700,
				0x3649, 0x3600, 0x2649, 0x3e00, 0x0024, 0x0000, 0x4024, 0x0000,
				0x0814, 0x2241, 0x1414, 0x1400, 0x4122, 0x1408, 0x0259, 0x0600,
				0x3e59, 0x5e00, 0x7e09, 0x7e00, 0x7f49, 0x3600, 0x3e41, 0x2200,
				0x7f41, 0x3e00, 0x7f49, 0x4100, 0x7f09, 0x0100, 0x3e49, 0x3a00,
				0x7f08, 0x7f00, 0x417f, 0x4100, 0x2040, 0x3f00, 0x7f0c, 0x7300,
				0x7f40, 0x4000, 0x7f06, 0x7f00, 0x7f01, 0x7e00, 0x3e41, 0x3e00,
				0x7f09, 0x0600, 0x3e41, 0xbe00, 0x7f09, 0x7600, 0x2649, 0x3200,
				0x017f, 0x0100, 0x7f40, 0x7f00, 0x1f60, 0x1f00, 0x7f30, 0x7f00,
				0x7708, 0x7700, 0x0778, 0x0700, 0x7149, 0x4700, 0x007f, 0x4100,
				0x031c, 0x6000, 0x0041, 0x7f00, 0x0201, 0x0200, 0x8080, 0x8000,
				0x0001, 0x0200, 0x2454, 0x7800, 0x7f44, 0x3800, 0x3844, 0x2800,
				0x3844, 0x7f00, 0x3854, 0x5800, 0x087e, 0x0900, 0x4854, 0x3c00,
				0x7f04, 0x7800, 0x447d, 0x4000, 0x2040, 0x3d00, 0x7f10, 0x6c00,
				0x417f, 0x4000, 0x7c18, 0x7c00, 0x7c04, 0x7800, 0x3844, 0x3800,
				0x7c14, 0x0800, 0x0814, 0x7c00, 0x7c04, 0x0800, 0x4854, 0x2400,
				0x043e, 0x4400, 0x3c40, 0x7c00, 0x1c60, 0x1c00, 0x7c30, 0x7c00,
				0x6c10, 0x6c00, 0x4c50, 0x3c00, 0x6454, 0x4c00, 0x0836, 0x4100,
				0x0077, 0x0000, 0x4136, 0x0800, 0x0201, 0x0201, 0x704c, 0x7000
	};


	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_SRGB_CAPABLE, TRUE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	/* Create a windowed mode window and its OpenGL context */
	_window = glfwCreateWindow(_screenWidth * 4, _screenHeight * 4, "LEM1820", NULL, NULL);
	if (!_window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(_window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();


	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);


	glGenBuffers(1, &_vbo);

	GLfloat vertices[] = {
		//  Position   Color             Texcoords
		-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
		1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
		1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
		-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
	};

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create an element array

	glGenBuffers(1, &_ebo);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	// Load textures

	glGenTextures(1, &_texture);

	glBindTexture(GL_TEXTURE_2D, _texture);

	glUniform1i(glGetUniformLocation(shaderProgram, "textureLEM"), 0);


	createBlankTexture(_screenWidth, _screenHeight);

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	//std::vector<uint8_t> pixelBuffer;

	//pixelBuffer.resize(_screenWidth * _screenHeight * 4);

	for (auto i = 0; i < _pixelBuffer.size(); ++i) {
		_pixelBuffer[i] = 0;
	}

	for (auto row = 0; row < _cellsHeight; ++row) {
		for (auto col = 0; col < _cellsWidth; ++col) {
			drawGlyph(col, row, (row * _cellsWidth + col));
		}
	}
	/*
	word_t glyph = 19;

	glyph *= 2;

	auto index = 0;
	std::vector<word_t> cols;
	cols.resize(4);

	for (auto h = 0; h < (_screenHeight * 4); h += 4) {
		for (auto w = 0; w < (_screenWidth * 4); w += 4) {
			index = h * _screenWidth + w;

			auto hIndex = (h / 4) / _cellHeight;
			auto wIndex = (w / 4) / _cellWidth;

			if (((w / 4) % 4) == 0)  {
				cols[0] = _characterFont[glyph] >> 8;
				cols[1] = _characterFont[glyph] & 0xff;
				cols[2] = _characterFont[glyph + 1] >> 8;
				cols[3] = _characterFont[glyph + 1] & 0xff;
			}

			if (hIndex == 0 && wIndex == 0) {
				auto bit = (cols[w / 4] >> (h / 4)) & 0x01;

				if (bit == 1) {
					pixelBuffer[index + 0] = 255;
					pixelBuffer[index + 1] = 255;
					pixelBuffer[index + 2] = 0;
					pixelBuffer[index + 3] = 255;
				}

				else {
					pixelBuffer[index + 0] = 0;
					pixelBuffer[index + 1] = 0;
					pixelBuffer[index + 2] = 0;
					pixelBuffer[index + 3] = 255;
				}
			}

		}

	}
	*/

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
		_screenWidth, _screenHeight,
		GL_RGBA, GL_UNSIGNED_BYTE, &_pixelBuffer[0]);


	// Draw a rectangle from the 2 triangles using 6 indices
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(_window);

	glfwPollEvents();

}

void LEM1820::createBlankTexture(uint32_t width, uint32_t height)
{
	std::vector<uint8_t> pixelBuffer;

	pixelBuffer.resize(width * height * 4);

	// Fill with colour
	uint8_t* ptr = &pixelBuffer[0];
	uint8_t* end = ptr + pixelBuffer.size();

	while (ptr < end) {
		*ptr++ = 255;
		*ptr++ = 255;
		*ptr++ = 0;
		*ptr++ = 255;
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		&pixelBuffer[0]);

}

void LEM1820::drawGlyph(word_t x, word_t y, word_t word)
{
	std::vector<word_t> cols;
	cols.resize(4);

	auto glyph = word * 2;

	if (glyph >= _characterFont.size()) {
		return;
	}

	cols[0] = _characterFont[glyph] >> 8;
	cols[1] = _characterFont[glyph] & 0xff;
	cols[2] = _characterFont[glyph + 1] >> 8;
	cols[3] = _characterFont[glyph + 1] & 0xff;

	x = x * 4;
	y = y * 4;

	for (auto row = 0; row < (_cellHeight * 4); row += 4) {
		for (auto col = 0; col < (_cellWidth * 4); col += 4) {

			auto index = ((y * _cellHeight) + row) * _screenWidth + ((x * _cellWidth) + col);

		//	std::cout << index << ", ";

			auto bit = (cols[(col / 4)] >> (row / 4)) & 0x01;

			if (bit == 1) {
				
				_pixelBuffer[index + 0] = 255;
				_pixelBuffer[index + 1] = 255;
				_pixelBuffer[index + 2] = 0;
				_pixelBuffer[index + 3] = 255;
			}
			else {
				_pixelBuffer[index + 0] = 0;
				_pixelBuffer[index + 1] = 0;
				_pixelBuffer[index + 2] = 0;
				_pixelBuffer[index + 3] = 255;
			}
		}

	}

//	std::cout << "";
}

/*
std::unique_ptr<SDL_Texture> LEM1820::loadTexture(const std::string& path)
{
	//The final texture
	std::unique_ptr<SDL_Texture> newTexture = std::make_unique<SDL_Texture>();

	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(_renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}
*/

/*
void LEM1820::charBlit(const uint16_t *charmap, SDL_Surface *dst, SDL_Rect *dstrect, uint8_t chr, Uint32 color)
{

}
*/