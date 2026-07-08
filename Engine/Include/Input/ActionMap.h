#pragma once
#include <Core/Common.h>

// Maps key presses to actions as GLFW_KEY_<label>, "ActionString"
// This uses a vector of pairs implementation to avoid overhead from unordered_map lookups
namespace Engine {
	struct ActionMap {
		std::vector<std::pair<i32, std::string>> bindings = {
			{GLFW_KEY_A, "A"},
			{GLFW_KEY_B, "B"},
			{GLFW_KEY_C, "C"},
			{GLFW_KEY_D, "D"},
			{GLFW_KEY_E, "E"},
			{GLFW_KEY_F, "F"},
			{GLFW_KEY_G, "G"},
			{GLFW_KEY_H, "H"},
			{GLFW_KEY_I, "I"},
			{GLFW_KEY_J, "J"},
			{GLFW_KEY_K, "K"},
			{GLFW_KEY_L, "L"},
			{GLFW_KEY_M, "M"},
			{GLFW_KEY_N, "N"},
			{GLFW_KEY_O, "O"},
			{GLFW_KEY_P, "P"},
			{GLFW_KEY_Q, "Q"},
			{GLFW_KEY_R, "R"},
			{GLFW_KEY_S, "S"},
			{GLFW_KEY_T, "T"},
			{GLFW_KEY_U, "U"},
			{GLFW_KEY_V, "V"},
			{GLFW_KEY_W, "W"},
			{GLFW_KEY_X, "X"},
			{GLFW_KEY_Y, "Y"},
			{GLFW_KEY_Z, "Z"},			
			{GLFW_KEY_SPACE, " "},
			{GLFW_KEY_APOSTROPHE, "'"},
			{GLFW_KEY_COMMA, ","},
			{GLFW_KEY_MINUS, "-"},
			{GLFW_KEY_PERIOD, "."},
			{GLFW_KEY_SLASH, "/"},
			{GLFW_KEY_0, "0"},
			{GLFW_KEY_1, "1"},
			{GLFW_KEY_2, "2"},
			{GLFW_KEY_3, "3"},
			{GLFW_KEY_4, "4"},
			{GLFW_KEY_5, "5"},
			{GLFW_KEY_6, "6"},
			{GLFW_KEY_7, "7"},
			{GLFW_KEY_8, "8"},
			{GLFW_KEY_9, "9"},
			{GLFW_KEY_SEMICOLON, ";"},
			{GLFW_KEY_BACKSLASH, "\\"},
			{GLFW_KEY_LEFT_BRACKET, "["},
			{GLFW_KEY_RIGHT_BRACKET, "]"},
			{GLFW_KEY_GRAVE_ACCENT, "`"},
			{GLFW_KEY_WORLD_1, "WORLD_1"}, // Extra key or used on non-EN keyboards
			{GLFW_KEY_WORLD_2, "WORLD_2"}, // Extra key or used on non-EN keyboards
			{GLFW_KEY_ESCAPE, "ESCAPE"},
			{GLFW_KEY_ENTER, "ENTER"},
			{GLFW_KEY_TAB, "TAB"},
			{GLFW_KEY_BACKSPACE, "BACKSPACE"},
			{GLFW_KEY_INSERT, "INSERT"},
			{GLFW_KEY_DELETE, "DELETE"},
			{GLFW_KEY_RIGHT, "R_ARROW"},
			{GLFW_KEY_LEFT, "L_ARROW"},
			{GLFW_KEY_UP, "U_ARROW"},
			{GLFW_KEY_DOWN, "D_ARROW"},
			{GLFW_KEY_PAGE_UP, "PG_UP"},
			{GLFW_KEY_PAGE_DOWN, "PG_DOWN"},
			{GLFW_KEY_HOME, "HOME"},
			{GLFW_KEY_END, "END"},
			{GLFW_KEY_CAPS_LOCK, "CAPS_LOCK"},
			{GLFW_KEY_SCROLL_LOCK, "SCROLL_LOCK"},
			{GLFW_KEY_NUM_LOCK, "NUM_LOCK"},
			{GLFW_KEY_PRINT_SCREEN, "PRINT_SCREEN"},
			{GLFW_KEY_PAUSE, "PAUSE"},
			{GLFW_KEY_F1, "F1"},
			{GLFW_KEY_F2, "F2"},
			{GLFW_KEY_F3, "F3"},
			{GLFW_KEY_F4, "F4"},
			{GLFW_KEY_F5, "F5"},
			{GLFW_KEY_F6, "F6"},
			{GLFW_KEY_F7, "F7"},
			{GLFW_KEY_F8, "F8"},
			{GLFW_KEY_F9, "F9"},
			{GLFW_KEY_F10, "F10"},
			{GLFW_KEY_F11, "F11"},
			{GLFW_KEY_F12, "F12"}, // F13 - F25 NOT MAPPED
			{GLFW_KEY_KP_0, "NUM0"}, // Numpad 0			
			{GLFW_KEY_KP_1, "NUM1"}, // Numpad 1			
			{GLFW_KEY_KP_2, "NUM2"}, // Numpad 2			
			{GLFW_KEY_KP_3, "NUM3"}, // Numpad 3			
			{GLFW_KEY_KP_4, "NUM4"}, // Numpad 4			
			{GLFW_KEY_KP_5, "NUM5"}, // Numpad 5			
			{GLFW_KEY_KP_6, "NUM6"}, // Numpad 6			
			{GLFW_KEY_KP_7, "NUM7"}, // Numpad 7			
			{GLFW_KEY_KP_8, "NUM8"}, // Numpad 8			
			{GLFW_KEY_KP_9, "NUM9"}, // Numpad 9			
			{GLFW_KEY_KP_DECIMAL, "NUM."}, // Numpad .		
			{GLFW_KEY_KP_DIVIDE, "NUM/"}, // Numpad /		
			{GLFW_KEY_KP_MULTIPLY, "NUM*"}, // Numpad *		
			{GLFW_KEY_KP_SUBTRACT, "NUM-"}, // Numpad -		
			{GLFW_KEY_KP_SUBTRACT, "NUM+"}, // Numpad +	
			{GLFW_KEY_KP_ENTER, "NUM_ENTER"}, // Numpad ENTER	
			{GLFW_KEY_KP_EQUAL, "NUM_="}, // Numpad =	
			{GLFW_KEY_LEFT_SHIFT, "L_SHIFT"},
			{GLFW_KEY_RIGHT_SHIFT, "R_SHIFT"},
			{GLFW_KEY_LEFT_CONTROL, "L_CTRL"},
			{GLFW_KEY_RIGHT_CONTROL, "R_CTRL"},
			{GLFW_KEY_LEFT_ALT, "L_ALT"},
			{GLFW_KEY_RIGHT_ALT, "R_ALT"},
			{GLFW_KEY_LEFT_SUPER, "L_SUPER"},
			{GLFW_KEY_RIGHT_SUPER, "R_SUPER"}
		};
	};
}