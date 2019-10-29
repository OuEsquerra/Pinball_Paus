
#include "Module.h"

struct SDL_Texture;

struct Font
{
	char table[MAX_FONT_CHARS];
	SDL_Texture* graphic = nullptr;
	uint rows, len, char_w, char_h, row_chars;
};

class ModuleFonts : public Module
{
public:

	ModuleFonts(Application* app, bool start_enabled = true);
	~ModuleFonts();

	int Load(const char* texture_path, const char* characters, uint rows = 1);
	bool UnLoad(int font_id);

	// Create a surface from text
	bool BlitText(int x, int y, int bmp_font_id, const char* text) const;

private:

	Font	 fonts[MAX_FONTS];
};