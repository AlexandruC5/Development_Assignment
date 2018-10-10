#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include <math.h>
#include "j1Map.h"
#include "j1Player.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	p2List_item<MapLayer*>* layer;
	layer = data.layers.start;
	p2List_item<TileSet*>* tileset;
	tileset = data.tilesets.start;

	App->render->Blit(data.background_img, 0, 0, &data.background_rect, 0.5f);
	App->render->Blit(data.background_img, data.background_rect.w, 0, &data.background_rect, 0.5f);
	//provisional
	App->render->Blit(data.background_img, 2*(data.background_rect.w), 0, &data.background_rect, 0.5f);

	while (layer != NULL)
	{
		for (int x = 0; x < data.width; x++) {
			for (int y = 0; y < data.height; y++) {
				uint gid = layer->data->Get(x, y);
				if (gid == 0) continue;
				TileSet* tileset = GetTileset(gid);
				SDL_Rect rect = tileset->GetTileRect(gid);
				iPoint position = MapToWorld(x, y);
				if (App->render->InCamera({ position.x, position.y, rect.w, rect.h }))
					App->render->Blit(tileset->texture, position.x, position.y, &rect);
			}
		}
		layer = layer->next;
	}

}


iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	ret.x = x * data.tile_width;
	ret.y = y * data.tile_height;

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* layer_item;
	layer_item = data.layers.start;
	while (layer_item != NULL)
	{
		RELEASE(layer_item->data);
		layer_item = layer_item->next;
	}
	data.layers.clear();
	App->tex->UnLoad(data.background_img);
	data.background_img = nullptr;

	//Remove all colliders
	p2List_item<Collider*>* collider;
	collider = data.colliders.start;
	while (collider != NULL)
	{
		collider->data->to_delete = true;
		collider = collider->next;
	}
	data.colliders.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}
	// Load all layers info
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* set = new MapLayer();

		if (ret == true)
		{
			ret = LoadLayer(layer, set);
		}

		data.layers.add(set);
	}

	//Load collision layer and create colliders
	pugi::xml_node objects = map_file.child("map").find_child_by_attribute("name", "colliders");
	ret = LoadCollisionLayer(objects);

	//Load image layer
	pugi::xml_node background = map_file.child("map").child("imagelayer");
	data.background_img = App->tex->Load(PATH(folder.GetString(), background.child("image").attribute("source").as_string()));
	data.background_rect = { 0,0,background.child("image").attribute("width").as_int(), background.child("image").attribute("height").as_int() };

	//Load Utils
	pugi::xml_node utils = map_file.child("map").find_child_by_attribute("name", "utils");
	LoadUtilsLayer(utils);

	App->player->position = data.spawn;

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node & node, MapLayer * layer)
{	
	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();

	uint size = layer->width*layer->height;
	layer->data = new uint[size];
	memset(layer->data, 0, sizeof(unsigned int) * size);

	pugi::xml_node tile;
	uint i = 0u;
	for (tile = node.child("data").child("tile"); tile; tile = tile.next_sibling("tile")) {
		layer->data[i] = tile.attribute("gid").as_uint();
		i++;
	}

	return true;
}

bool j1Map::LoadCollisionLayer(pugi::xml_node & node)
{
	pugi::xml_node collider;
	for (collider = node.child("object"); collider; collider = collider.next_sibling("object")) 
	{
		SDL_Rect rect = { collider.attribute("x").as_int(),collider.attribute("y").as_int(), collider.attribute("width").as_int(), collider.attribute("height").as_int() };
		if (collider.attribute("type").as_int() == COLLIDER_PLATFORM)
		{
			data.colliders.add(App->collision->AddCollider(rect, COLLIDER_PLATFORM));
		}
		else if(collider.attribute("type").as_int() == COLLIDER_FLOOR)
		{
			data.colliders.add(App->collision->AddCollider(rect, COLLIDER_FLOOR));
		}
	}
	return true;
}

bool j1Map::LoadUtilsLayer(pugi::xml_node & node)
{
	data.spawn.x = node.find_child_by_attribute("name", "spawn").attribute("x").as_float();
	data.spawn.y = node.find_child_by_attribute("name", "spawn").attribute("y").as_float();

	data.lvl_end.x = node.find_child_by_attribute("name", "end").attribute("x").as_float();
	data.lvl_end.y = node.find_child_by_attribute("name", "end").attribute("y").as_float();
	data.lvl_end.w = node.find_child_by_attribute("name", "end").attribute("width").as_float();
	data.lvl_end.h = node.find_child_by_attribute("name", "end").attribute("height").as_float();

	return true;
}

TileSet* j1Map::GetTileset(uint id) const
{
	p2List_item<TileSet*>* tileset = data.tilesets.start;
	while (tileset != NULL)
	{
		if (tileset->next && id < tileset->next->data->firstgid) return tileset->data;
		tileset = tileset->next;
	}
	return data.tilesets.end->data;
}

TileSet::~TileSet() {
	App->tex->UnLoad(texture);
}