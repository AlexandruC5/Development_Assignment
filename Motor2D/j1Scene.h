#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"


struct SDL_Texture;
struct j1UIElement;
struct j1UIScrollBar;
enum GUI_Event;

struct Level {
	p2SString map_path = "";
	p2SString sound_path = "";
	int next_level = 0;
	bool game_level = true;
};

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node &node);
	bool Save(pugi::xml_node &node) const;

	bool GUIEvent(j1UIElement* element, GUI_Event gui_event);

	int current_level = 0;


private:
	p2List<Level> levels;

	j1UIElement* menu_background;

	j1UIElement* main_menu_panel;
	j1UIElement* main_menu_button_play;
	j1UIElement* main_menu_button_continue;
	j1UIElement* main_menu_button_settings;
	j1UIElement* main_menu_button_credits;
	j1UIElement* main_menu_button_exit;

	j1UIElement* main_menu_button_play_text;
	j1UIElement* main_menu_button_continue_text;
	j1UIElement* main_menu_button_settings_text;
	j1UIElement* main_menu_button_credits_text;
	j1UIElement* main_menu_button_exit_text;


	j1UIElement* pause_menu_panel;
	j1UIElement* pause_menu_button_resume;
	j1UIElement* pause_menu_button_main_menu;
	
	j1UIElement* pause_menu_button_resume_text;
	j1UIElement* pause_menu_button_main_menu_text;

	j1UIElement* settings_menu_panel;
	j1UIElement* settings_menu_button_main_menu;
	j1UIElement* settings_menu_button_main_menu_text;
	j1UIScrollBar* settings_menu_music_slider;
	j1UIElement* settings_menu_music_text;
	j1UIScrollBar* settings_menu_sfx_slider;
	j1UIElement* settings_menu_sfx_text;



	j1UIElement* credits_menu_panel;
	j1UIElement* credits_menu_button_main_menu;
	j1UIElement* credits_menu_button_main_menu_text;
	j1UIElement* credits_menu_text_panel;
	j1UIElement* credits_menu_text;
	j1UIScrollBar* credits_menu_text_scroll;
};

#endif // __j1SCENE_H__