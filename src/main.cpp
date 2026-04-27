#include "Globals.h"

#include "Texture.h"
#include "MapLayer.h"
#include "View.h"

#include <SDL3/SDL_main.h>
#include <cstdlib>

// background
Texture background;

// map
tmx::Map map;
std::vector<Texture*> textures;
std::vector<MapLayer*> renderLayers;
std::vector<tmx::ObjectGroup*> objectLayers;

// player
struct Player {
	Texture texture;
	SDL_FRect onScreenRect{ };
	double speed = 40.0;
	bool movingLeft = false;
	bool movingRight = false;
	bool movingUp = false;
	bool movingDown = false;
};
Player player;

// View
View view(0, 0, screenWidth, screenHeight);

bool init()
{
	// SDL
	if (SDL_Init(SDL_INIT_VIDEO) == false) {
		SDL_Log("Error initializing SDL - SDL error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_CreateWindowAndRenderer("SDL3 tmx Loader", screenWidth, screenHeight, 0, &Window, &Renderer) == false) {
		SDL_Log("Window could not be created - SDL error: %s\n", SDL_GetError());
		return false;
	}
	if (background.loadFromFile(Renderer, "images/background.png") == false) {
		SDL_Log("Unable to load background image\n");
		return false;
	}

	// load map
	if (!map.load("maps/demo.tmx")) {
		SDL_Log("Unable to load map\n");
		return false;
	}

	// load all tilesets
	const auto& tileSets = map.getTilesets();
	assert(!tileSets.empty());
	for (const auto& ts : tileSets)
	{
		Texture* t = new Texture();
		if (!t->loadFromFile(Renderer, ts.getImagePath()))
			std::cerr << "Failed opening " << ts.getImagePath() << "\n";
		else textures.push_back(t);
	}

	// load render and object layers
	const auto& mapLayers = map.getLayers();
	for (auto i = 0u; i < mapLayers.size(); ++i)
	{
		if (mapLayers[i]->getType() == tmx::Layer::Type::Tile)
		{
			MapLayer* mp = new MapLayer(map, i, textures);
			renderLayers.push_back(mp);
		}
		else if (mapLayers[i]->getType() == tmx::Layer::Type::Object)
			objectLayers.push_back(&mapLayers[i].get()->getLayerAs<tmx::ObjectGroup>());
	}

	// process objects
	for (tmx::ObjectGroup* ol : objectLayers) {
		for (tmx::Object o : ol->getObjects()) {
			std::string s = o.getName();
			SDL_Point pos{ o.getPosition().x, o.getPosition().y };
			SDL_Point center{ pos.x + o.getAABB().width / 2.f, pos.y + o.getAABB().height / 2.f };

			// do something with the objects
			std::cout << "object " << s << " found." << std::endl;
		}
	}

	// load player
	if (player.texture.loadFromFile(Renderer, "images/man.png") == false) {
		SDL_Log("Unable to load player image\n");
		return false;
	}

	player.onScreenRect.x = view.getRect().w / 2.f;
	player.onScreenRect.y = view.getRect().h / 2.f;
	player.onScreenRect.w = player.texture.getWidth();
	player.onScreenRect.h = player.texture.getHeight();

	return true;
}

void destroy()
{
	player.texture.destroy();
	background.destroy();

	SDL_DestroyRenderer(Renderer);
	Renderer = nullptr;
	SDL_DestroyWindow(Window);
	Window = nullptr;
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	if (!init()) return EXIT_FAILURE;

	bool running = true;

	// main loop
	while (running) {
		SDL_Event e;
		SDL_zero(e);

		// time
		now = SDL_GetPerformanceCounter();
		deltaTime = (double)(now - last) / (double)freq;
		last = now;
		elapsed += deltaTime;

		// input
		while (SDL_PollEvent(&e) == true)
		{
			if (e.type == SDL_EVENT_QUIT)
				running = false;

			if (e.type == SDL_EVENT_KEY_DOWN) {
				if (e.key.key == SDLK_ESCAPE) running = false;

				if (e.key.key == SDLK_LEFT) player.movingLeft = true;
				if (e.key.key == SDLK_RIGHT) player.movingRight = true;
				if (e.key.key == SDLK_UP) player.movingUp = true;
				if (e.key.key == SDLK_DOWN) player.movingDown = true;

			}
			if (e.type == SDL_EVENT_KEY_UP) {
				if (e.key.key == SDLK_LEFT) player.movingLeft = false;
				if (e.key.key == SDLK_RIGHT) player.movingRight = false;
				if (e.key.key == SDLK_UP) player.movingUp = false;
				if (e.key.key == SDLK_DOWN) player.movingDown = false;
			}
		}

		// update movement 60 times per seconds
		if (elapsed >= timeStep) {
			float x = 0;
			float y = 0;

			if (player.movingLeft) x -= player.speed * timeStep;
			if (player.movingRight) x += player.speed * timeStep;
			if (player.movingUp) y -= player.speed * timeStep;
			if (player.movingDown) y += player.speed * timeStep;

			view.update(x, y);

			for (const auto& l : renderLayers)
				l->update(view.getRect());

			elapsed = 0.0;
		}

		if (running) {
			SDL_RenderClear(Renderer);

			// draw background
			background.renderAsBackground(Renderer, screenWidth, screenHeight);

			// update and draw layers
			for (const auto& l : renderLayers)
				l->draw(Renderer);

			// draw player
			player.texture.render(Renderer, player.texture.getRect(), player.onScreenRect, 0.f);

			SDL_RenderPresent(Renderer);
		}
	}	

	destroy();

	return EXIT_SUCCESS;
}
