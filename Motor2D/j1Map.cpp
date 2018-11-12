#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include <math.h>
#include "j1Map.h"
#include "j1Player.h"
#include "j1SwapScene.h"
#include "j1Window.h"

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
	
	InfiniteBackground();

	p2List_item<MapLayer*>* layer;
	layer = data.layers.start;
	p2List_item<TileSet*>* tileset;
	tileset = data.tilesets.start;

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

	//Remove all colliders
	p2List_item<Collider*>* collider;
	collider = data.colliders.start;
	while (collider != NULL)
	{
		collider->data->to_delete = true;
		collider = collider->next;
	}
	data.colliders.clear();

	//Remove background image
	App->tex->UnLoad(data.background_1.background_img);
	data.background_1.background_img = nullptr;
	data.background_1.default_x = 0;

	App->tex->UnLoad(data.background_2.background_img);
	data.background_2.background_img = nullptr;
	data.background_2.default_x = 0;

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString path = PATH(folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(path.GetString());

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
	data.background_1.background_img = data.background_2.background_img = App->tex->Load(PATH(folder.GetString(), background.child("image").attribute("source").as_string()));
	data.background_1.background_rect = data.background_2.background_rect = { data.background_1.default_x,0,background.child("image").attribute("width").as_int(), background.child("image").attribute("height").as_int() };
	data.background_1.background_offset = data.background_2.background_offset = background.attribute("offsety").as_float();
	data.background_1.background_speed = data.background_2.background_speed = map_file.child("map").child("imagelayer").child("properties").find_child_by_attribute("name","speed").attribute("value").as_float();
	data.background_2.background_rect.x = data.background_2.default_x == 0? data.background_1.background_rect.w:data.background_2.default_x;
	data.background_1.default_x = 0;
	data.background_2.default_x = 0;
	//Load Utils
	pugi::xml_node utils = map_file.child("map").find_child_by_attribute("name", "utils");
	LoadUtilsLayer(utils);

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

bool j1Map::Save(pugi::xml_node &node) const
{
	node.append_child("background_1").append_attribute("x") = data.background_1.background_rect.x;
	node.append_child("background_2").append_attribute("x") = data.background_2.background_rect.x;
	return true;
}

bool j1Map::Load(pugi::xml_node &node)
{
	data.background_1.default_x = node.child("background_1").attribute("x").as_int();
	data.background_2.default_x = node.child("background_2").attribute("x").as_int();
	data.background_1.background_rect.x = data.background_1.default_x;
	data.background_2.background_rect.x = data.background_2.default_x;

	return true;
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
	uint i = 0U;
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
	pugi::xml_node end_trigger = node.find_child_by_attribute("name", "end");
	SDL_Rect temp_trigger;
	temp_trigger.x = end_trigger.attribute("x").as_int();
	temp_trigger.y = end_trigger.attribute("y").as_int();
	temp_trigger.w = end_trigger.attribute("width").as_int();
	temp_trigger.h = end_trigger.attribute("height").as_int();
	data.colliders.add(App->collision->AddCollider(temp_trigger, COLLIDER_TRIGGER, (j1Module*)App->swap_scene->current_scene));

	App->render->camera.body.y = -(App->map->data.height * App->map->data.tile_height - App->render->camera.body.h);
	App->render->camera.body.x = 0;
	pugi::xml_node spawn = node.find_child_by_attribute("name", "spawn");
	App->player->SetPosition(spawn.attribute("x").as_float(), spawn.attribute("y").as_float());

	return true;
}

void j1Map::InfiniteBackground()
{
	if (data.background_2.background_rect.x + data.background_2.background_rect.w < -App->render->camera.body.x * data.background_2.background_speed && App->render->camera.velocity.x < 0)
	{
		data.background_2.background_rect.x = data.background_1.background_rect.x + data.background_2.background_rect.w;
	}
	else
	{
		if (data.background_2.background_rect.x >(-App->render->camera.body.x * data.background_2.background_speed) + App->render->camera.body.w  && App->render->camera.velocity.x > 0)
		{
			data.background_2.background_rect.x = data.background_1.background_rect.x - data.background_2.background_rect.w;
		}
	}
	if (data.background_1.background_rect.x + data.background_1.background_rect.w < -App->render->camera.body.x * data.background_1.background_speed && App->render->camera.velocity.x < 0)
	{
		data.background_1.background_rect.x = data.background_2.background_rect.x + data.background_1.background_rect.w;
	}
	else
	{
		if (data.background_1.background_rect.x >(-App->render->camera.body.x * data.background_1.background_speed) + App->render->camera.body.w  && App->render->camera.velocity.x > 0)
		{
			data.background_1.background_rect.x = data.background_2.background_rect.x - data.background_1.background_rect.w;
		}
	}

	App->render->Blit(data.background_1.background_img, data.background_1.background_rect.x, data.background_1.background_offset, NULL, data.background_1.background_speed);
	App->render->Blit(data.background_2.background_img, data.background_2.background_rect.x, data.background_2.background_offset, NULL, data.background_2.background_speed);
}

TileSet* j1Map::GetTileset(uint id) const
{
	p2List_item<TileSet*>* tileset = data.tilesets.start;
	while (tileset != NULL)
	{
		if (id < tileset->data->firstgid) return tileset->prev->data;
		tileset = tileset->next;
	}
	return data.tilesets.end->data;
}

TileSet::~TileSet() {
	App->tex->UnLoad(texture);
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	p2List_item<MapLayer*>* item;
	item = data.layers.start;

	for (item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->name != "Navigation")
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTileset(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = 0;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2) - 1;
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}