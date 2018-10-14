#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "math.h"

struct MapLayer {
	p2SString name = "";
	uint width = 0u;
	uint height = 0u;
	uint* data = nullptr;

	~MapLayer(){
		if (data != nullptr) {
			delete[] data;
		}
	}
	inline uint Get(int x, int y) const {
		return data[x + (width*y)];
	}
};


struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	uint				firstgid;
	uint				margin;
	uint				spacing;
	uint				tile_width;
	uint				tile_height;
	SDL_Texture*		texture;
	uint				tex_width;
	uint				tex_height;
	uint				num_tiles_width;
	uint				num_tiles_height;
	uint				offset_x;
	uint				offset_y;

	~TileSet();
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

struct BackgroundData
{
	SDL_Texture*		background_img;
	SDL_Rect			background_rect;
	float				background_offset;
	float				background_speed;
};

struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	layers;
	p2List<Collider*>	colliders;
	BackgroundData      background_1;
	BackgroundData      background_2;

};

class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);
	
	//Translates x,y coordinates from map positions to world positions
	iPoint MapToWorld(int x, int y) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadCollisionLayer(pugi::xml_node & node);
	bool LoadUtilsLayer(pugi::xml_node & node);
	TileSet* GetTileset(uint id) const;

public:

	MapData data;

private:
	
	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__