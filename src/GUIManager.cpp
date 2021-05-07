#include "GUIManager.h"

GUIManager::GUIManager()
	:	multi_node_selected_(false),
		point_count_(0),
		points_collected_(0),
		max_point_count_(0),   // buffer between all panels and each other + screen edges
		delete_all_(false),
		gui_visible_(false),
		panel_pixel_buffer_(5),
		draw_particle_gui_(false),
		draw_audio_gui_(false)
{
	const string error_message = "Error: Updating failed"; // error message will show for all parameters that require but haven't received an update
	const int error_int = 404;

	const ofVec2f k_bounds = { 0.1f, 20.0f };
	const ofVec2f damping_bounds = { 0.1f, 8.0f };
	ofVec2f springmass_bounds = { 0.1f, 50.0f };

	gui_world_delete_all.addListener(this, &GUIManager::toggle_delete_all);

	// Global
	panel_world.setup("World", "", panel_pixel_buffer_, panel_pixel_buffer_);
	panel_world.add(gui_world_delete_all.setup("clear all"));
	panel_world.add(gui_world_gravity.setup("global gravity", false));
	panel_world.add(gui_world_hard_collisions.setup("hard collisions", false));

	// Player
	panel_player.setup("Player", "", panel_world.getPosition().x + panel_world.getWidth() + panel_pixel_buffer_, panel_pixel_buffer_);
	panel_player.add(gui_player_pos.setup("pos", error_message));
	panel_player.add(gui_player_vel.setup("vel", error_message));
	panel_player.add(gui_player_accel.setup("acceleration", error_message));
	panel_player.add(gui_player_mass.setup("mass", error_int, MINIMUM_MASS, MAXIMUM_MASS));
	panel_player.add(gui_player_infinite_mass.setup("infinite mass", false));
	panel_player.add(gui_player_radius.setup("radius", error_int, RADIUS_MINIMUM, RADIUS_MAXIMUM));
	panel_player.add(gui_player_affected_by_gravity.setup("gravity", false));

	// Mass
	panel_node.setup("Selected Object", "", ofGetWidth() - panel_node.getWidth() - panel_pixel_buffer_, panel_pixel_buffer_);
	panel_node.add(gui_node_pos.setup("pos", error_message));
	panel_node.add(gui_node_vel.setup("vel", error_message));
	panel_node.add(gui_node_accel.setup("acceleration", error_message));
	panel_node.add(gui_node_mass.setup("mass", error_int, MINIMUM_MASS, MAXIMUM_MASS));
	panel_node.add(gui_node_infinite_mass.setup("infinite mass", false));
	panel_node.add(gui_node_radius.setup("radius", error_int, RADIUS_MINIMUM, RADIUS_MAXIMUM));
	panel_node.add(gui_node_affected_by_gravity.setup("gravity", false));
	panel_node.add(gui_node_emission_frequency.setup("Emission Frequency", error_int, 15, 150));

	// Spring
	panel_spring_settings.setup("Spring Settings", "", ofGetWidth() - panel_spring_settings.getWidth() - panel_pixel_buffer_, panel_pixel_buffer_);
	panel_spring_settings.add(gui_spring_anchor_pos.setup("anchor pos", error_message));
	panel_spring_settings.add(gui_spring_k.setup("springiness", error_int, k_bounds.x, k_bounds.y));
	panel_spring_settings.add(gui_spring_damping.setup("damping", error_int, damping_bounds.x, damping_bounds.y));
	panel_spring_settings.add(gui_spring_springmass.setup("springmass", error_int, MINIMUM_MASS, MAXIMUM_MASS));
	panel_spring_settings.add(gui_spring_affected_by_gravity.setup("gravity", false));
	// Spring Node
	panel_spring_node.setup("Selected Object", "", ofGetWidth() - panel_spring_settings.getWidth() - panel_pixel_buffer_, panel_spring_settings.getPosition().y + panel_spring_settings.getHeight() + panel_pixel_buffer_);
	panel_spring_node.add(gui_spring_node_pos.setup("pos", error_message));
	panel_spring_node.add(gui_spring_node_vel.setup("vel", error_message));
	panel_spring_node.add(gui_spring_node_accel.setup("acceleration", error_message));
	panel_spring_node.add(gui_spring_node_mass.setup("mass", error_int, MINIMUM_MASS, MAXIMUM_MASS / 10));
	panel_spring_node.add(gui_spring_node_radius.setup("radius", error_int, RADIUS_MINIMUM, RADIUS_MAXIMUM));
}

void GUIManager::init(Controller* controller, FluidManager* fluid_manager, AudioManager* audio_manager, Camera* cam)
{
	game_controller_ = controller;
	fluid_manager_ = fluid_manager;
	audio_manager_ = audio_manager;
	cam_ = cam;
}

void GUIManager::update()
{	
	update_world();
}

void GUIManager::update_world()
{
	game_controller_->set_gravity(gui_world_gravity);
	game_controller_->set_use_hard_collisions(gui_world_hard_collisions);
}

void GUIManager::inc_points_collected()
{
	points_collected_++;
}

void GUIManager::inc_max_point_count()
{
	max_point_count_++;
}

void GUIManager::update_point_count(const int count)
{
	point_count_ = count;
}

void GUIManager::reset_point_counters()
{
	max_point_count_ = 0;
	points_collected_ = 0;
}

int GUIManager::get_delete_all() const
{
	return delete_all_;
}

void GUIManager::toggle_delete_all()
{
	(delete_all_ == 0) ? delete_all_ = true : delete_all_ = false;
}

void GUIManager::set_gui_visible(const bool value)
{
	(value == 1) ? gui_visible_ = true : gui_visible_ = false;
}

bool GUIManager::get_gui_visible() const
{
	return gui_visible_;
}

void GUIManager::update_values(const string entity_type, const ofVec2f node_position, const ofVec2f node_velocity, const ofVec2f node_acceleration, const float _node_mass, const bool infmass, const float _node_radius, const bool is_affected_by_gravity, const int emission_frequency)
{
	if (entity_type == "Player")
	{
		gui_player_pos = ofToString(roundf(node_position.x)) + ", " + ofToString(roundf(node_position.y));
		gui_player_vel = ofToString(roundf(node_velocity.x * 100) / 100) + ", " + ofToString(roundf(node_velocity.y * 100) / 100);
		gui_player_accel = ofToString(roundf(node_acceleration.x * 10000) / 10000) + ", " + ofToString(roundf(node_acceleration.y * 10000) / 10000);
		if (infmass)
		{
			gui_player_mass.setTextColor(0);
			gui_player_infinite_mass = true;
		}
		else
		{
			gui_player_mass.setTextColor(255);
			gui_player_mass = _node_mass;
			gui_player_infinite_mass = false;
		}
		gui_player_radius = _node_radius;
		gui_player_affected_by_gravity = is_affected_by_gravity;
	}
	else if (entity_type == "Mass")
	{
		panel_node.setName(entity_type);
		panel_node.setPosition(cam_->world_to_screen(ofVec2f(HALF_WORLD_WIDTH + node_position.x + 8, HALF_WORLD_HEIGHT + node_position.y + 8)));
		
		gui_node_pos = ofToString(roundf(node_position.x)) + ", " + ofToString(roundf(node_position.y));
		gui_node_vel = ofToString(roundf(node_velocity.x * 100) / 100) + ", " + ofToString(roundf(node_velocity.y * 100) / 100);
		gui_node_accel = ofToString(roundf(node_acceleration.x * 10000) / 10000) + ", " + ofToString(roundf(node_acceleration.y * 10000) / 10000);
		if (infmass)
		{
			gui_node_mass.setTextColor(0);
			gui_node_infinite_mass = true;
		}
		else
		{
			gui_node_mass.setTextColor(255);
			gui_node_mass = _node_mass;
			gui_node_infinite_mass = false;
		}
		gui_node_radius = _node_radius;
		gui_node_affected_by_gravity = is_affected_by_gravity;
	}
	else if (entity_type == "Collectable")
	{
		panel_node.setName(entity_type);
		panel_node.setPosition(cam_->world_to_screen(ofVec2f(HALF_WORLD_WIDTH + node_position.x + 8, HALF_WORLD_HEIGHT + node_position.y + 8)));

		gui_node_pos = ofToString(roundf(node_position.x)) + ", " + ofToString(roundf(node_position.y));
		gui_node_vel = ofToString(roundf(node_velocity.x * 100) / 100) + ", " + ofToString(roundf(node_velocity.y * 100) / 100);
		gui_node_accel = ofToString(roundf(node_acceleration.x * 10000) / 10000) + ", " + ofToString(roundf(node_acceleration.y * 10000) / 10000);
		if (infmass)
		{
			gui_node_mass.setTextColor(0);
			gui_node_infinite_mass = true;
		}
		else
		{
			gui_node_mass.setTextColor(255);
			gui_node_mass = _node_mass;
			gui_node_infinite_mass = false;
		}
		gui_node_radius = _node_radius;
		gui_node_affected_by_gravity = is_affected_by_gravity;
		gui_node_emission_frequency = emission_frequency;
	}
}

void GUIManager::update_multiple_values(const ofVec2f spring_anchor_position, const float spring_k, const float spring_damping, const float spring_mass, const bool is_affected_by_gravity, const ofVec2f selected_node_pos, const ofVec2f selected_node_vel, const ofVec2f selected_node_accel, const float selected_node_mass, const float selected_node_radius)
{
	panel_spring_settings.setPosition(cam_->world_to_screen(ofVec2f(HALF_WORLD_WIDTH + spring_anchor_position.x + 8, HALF_WORLD_HEIGHT + spring_anchor_position.y + 8)));
	panel_spring_node.setPosition(cam_->world_to_screen(ofVec2f(HALF_WORLD_WIDTH + selected_node_pos.x + 8, HALF_WORLD_HEIGHT + selected_node_pos.y + 8)));
	
	gui_spring_anchor_pos = ofToString(roundf(spring_anchor_position.x)) + ", " + ofToString(roundf(spring_anchor_position.y));

	gui_spring_k = spring_k;
	gui_spring_damping = spring_damping;
	gui_spring_springmass = spring_mass;
	gui_spring_affected_by_gravity = is_affected_by_gravity;

	if (selected_node_pos == ofVec2f(-1, -1) && selected_node_vel == ofVec2f(-1, -1) && selected_node_accel == ofVec2f(-1, -1) && selected_node_mass == -1 && selected_node_radius == -1)
	{
		// can't draw
		multi_node_selected_ = false;
	}
	else
	{
		multi_node_selected_ = true;
		gui_spring_node_pos = ofToString(roundf(selected_node_pos.x)) + ", " + ofToString(roundf(selected_node_pos.y));
		gui_spring_node_vel = ofToString(roundf(selected_node_vel.x * 100) / 100) + ", " + ofToString(roundf(selected_node_vel.y * 100) / 100);
		gui_spring_node_accel = ofToString(roundf(selected_node_accel.x * 10000) / 10000) + ", " + ofToString(roundf(selected_node_accel.y * 10000) / 10000);
		gui_spring_node_mass = selected_node_mass;
		gui_spring_node_radius = selected_node_radius;
	}
}

void GUIManager::draw_required_gui(GameObject* selected_object, const bool is_spring, const int new_node_id, const string current_gamemode)
{
	if (get_gui_visible() /*|| Event_Manager->playerGUIVisible*/)
	{
		panel_player.draw();

		if (selected_object != nullptr)
		{
			if (is_spring)
			{
				// if an object is a spring then it has multiple gui windows to draw
				panel_spring_settings.draw();
				if (multi_node_selected_ == true)
				{
					panel_spring_node.draw();
				}
			}
			else
			{
				panel_node.draw();
			}
		}
	}
	if (get_gui_visible())
	{
		panel_world.draw();
	}

	FluidManager::draw_gui(draw_particle_gui_);
	audio_manager_->drawGUI(draw_audio_gui_);

	draw_text(new_node_id, current_gamemode);
	if (current_gamemode == "Sandbox") draw_border();
}

void GUIManager::draw_text(const int new_node_id, const string current_gamemode) const
{
	string entity_type;
	switch (new_node_id)
	{
	case 0:
		entity_type = "Mass";
		break;
	case 1:
		entity_type = "Spring";
		break;
	case 2:
		entity_type = "Collectable";
		break;
	default:
		cout << "Error -> GUIManager.cpp::update_create_node_id -> New Node Type Not Specified" << endl;
		break;
	}
	
	ofDrawBitmapString("Entity Type: " + entity_type, glm::vec2((ofGetWidth() / 2) - 100, ofGetHeight() - 150));
	ofDrawBitmapString("GameMode: " + current_gamemode, glm::vec2((ofGetWidth() / 2) - 100, ofGetHeight() - 100));
	ofDrawBitmapString("Collectables Found: " + to_string(points_collected_) + " / " + to_string(max_point_count_), glm::vec2((ofGetWidth() / 2) - 100, ofGetHeight() - 50));
}

void GUIManager::draw_border() const
{
	ofPushStyle();
	ofPushMatrix();

	ofSetColor(255, 0, 0);
	ofNoFill();
	ofSetLineWidth(10);
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

	ofPopMatrix();
	ofPopStyle();
}

void GUIManager::key_pressed(const int key)
{
	if (key == 57344) // f1
	{		
		if (get_gui_visible())
		{
			set_gui_visible(false);
		}
		else
		{
			set_gui_visible(true);
			draw_particle_gui_ = false;
			draw_audio_gui_ = false;
		}
	}
	else if (key == 57345) // f2
	{		
		if (draw_particle_gui_)
		{
			draw_particle_gui_ = false;
		}
		else
		{
			set_gui_visible(false);
			draw_particle_gui_ = true;
			draw_audio_gui_ = false;
		}
	}
	else if (key == 57346) // f3
	{		
		if (draw_audio_gui_)
		{
			draw_audio_gui_ = false;
		}
		else
		{
			set_gui_visible(false);
			draw_particle_gui_ = false;
			draw_audio_gui_ = true;
		}
	}
}
