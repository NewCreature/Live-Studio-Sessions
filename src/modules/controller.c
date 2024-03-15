#include "t3f/t3f.h"
#include "t3f/input.h"

#include "controller.h"

void lss_map_keyboard_instrument_controls(LSS_CONTROLLER * cp)
{
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_A, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_F1);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_B, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_F2);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_Y, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_F3);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_X, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_F4);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_L, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_F5);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_DPAD_UP, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_RSHIFT);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_DPAD_DOWN, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_ENTER);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_START, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_ESCAPE);
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_GREEN] = T3F_GAMEPAD_A;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_RED] = T3F_GAMEPAD_B;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_YELLOW] = T3F_GAMEPAD_Y;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_BLUE] = T3F_GAMEPAD_X;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_ORANGE] = T3F_GAMEPAD_L;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN] = T3F_GAMEPAD_DPAD_DOWN;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP] = T3F_GAMEPAD_DPAD_UP;
}

void lss_map_keyboard_gamepad_controls(LSS_CONTROLLER * cp)
{
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_A, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_A);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_B, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_D);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_Y, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_J);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_X, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_K);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_L, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_L);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_START, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_ESCAPE);
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_GREEN] = T3F_GAMEPAD_A;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_RED] = T3F_GAMEPAD_B;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_YELLOW] = T3F_GAMEPAD_Y;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_BLUE] = T3F_GAMEPAD_X;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_ORANGE] = T3F_GAMEPAD_L;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN] = T3F_GAMEPAD_DPAD_DOWN;
	cp->map[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP] = T3F_GAMEPAD_DPAD_UP;
}

void lss_map_keyboard_menu_controls(LSS_CONTROLLER * cp)
{
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_DPAD_UP, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_UP);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_DPAD_DOWN, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_DOWN);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_DPAD_LEFT, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_LEFT);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_DPAD_RIGHT, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_RIGHT);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_A, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_ENTER);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_B, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_ESCAPE);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_Y, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_Z);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_X, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_X);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_L, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_A);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_R, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_S);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_START, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_Q);
	t3f_bind_input_handler_element(cp->input, T3F_GAMEPAD_SELECT, T3F_INPUT_HANDLER_DEVICE_TYPE_KEYBOARD, 0, ALLEGRO_KEY_W);
}

void lss_read_controller(LSS_CONTROLLER * cp)
{
	int i, x, y;

	switch(cp->source)
	{
		case LSS_CONTROLLER_SOURCE_CONTROLLER:
		{
			t3f_update_input_handler_state(cp->input);
			break;
		}
	}
}
