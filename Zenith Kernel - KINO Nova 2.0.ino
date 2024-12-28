#define MAX_MENU_SIZE 10 //This can be change in order of the device memory.

struct menu {
  uint8_t length;
  const char* menu_data_array[MAX_MENU_SIZE];
};
/**
   @brief Handles Printing Menu.
   @param const menu& menu, const uint8_t& index, uint8_t size_to_display, void (printing_function)(char[])
   @return success state (0, successful | 1, unsuccessful)
   Takes the struct menu& object as menu and print what is inside the specified &index.
   Printing from external functions is accepted, as default, Serial BUS will be use.
   IMPORTANT: The UART Comunication systme must be alredy defined and created, using Serial.begin(); or similar.
*/
int print(const menu& menu, const uint8_t& index, uint8_t size_to_display = 2, void (printing_function)(char[], int) = nullptr) {
  for (int i = 0; i < size_to_display; i += 1) {
    if (printing_function != nullptr) {
      if (index + i > menu.length - 1) return 1;
      printing_function(menu.menu_data_array[index + i], i);
    }
    else {
      Serial.println(menu.menu_data_array[index + i]);
    }
  }
  return 0;
}
/**
  @brief Handles Keyboard Input for UP & DOWN.
  @param char key, uint8_t& index, const menu& menu, char keymap_up, char keymap_down, bool auto_print, uint8_t size_to_display, void (printing_function)(char[])
  @return success state (0, success | -1, unsuccessful. Invalid | 1, Unaccepted character '\n' | '\r'
  Take key and check if match UP | DOWN key. If so, takes &index and substract or add 1 to it, to check if inside limits, it takes struct menu& object as menu, and compare the length of the menu to the &index, if inside, it will add or substract.
*/
int handle_keyboard_input(char key, uint8_t& index, const menu& menu, char keymap_up = 'A', char keymap_down = 'B', bool auto_print = false, uint8_t size_to_display = 2, void (printing_function)(char[], int) = nullptr) {
  if (key == keymap_up) {
    if (index < menu.length - size_to_display + 1) {
      index += 1;
    }
    else {
      index = 0;
    }
    if (auto_print == true) {
      print(menu, index, size_to_display, printing_function);
    }
    return 0;
  }
  else if (key == keymap_down) {
    if (index > 0) {
      index -= 1;
    }
    else {
      index = menu.length - size_to_display;
    }
    if (auto_print == true) {
      print(menu, index, size_to_display, printing_function);
    }
    return 0;
  }
  else if (key != '\n' && key != '\r') {
    return -1; //In case to find any char not '\n' | '\r', will return -1.
  }
  else {
    return 1; //In case to find '\n' | '\r' will return an error.
  }
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  -------------------------------test zone-------------*/

#define ENTER '#' //Represent the ENTER Key.
#define BACK '*' //Represent the GO BACK Key.
#define FORWARD 'W' //Represent the GO FORWARD Key.
#define BACKWARD 'S' //Represent the GO BACKWARD Key.

const uint8_t _max_items_per_menu = 4; //Hold the max number of items that each menu can hold.
const uint8_t _menu_item_count[_max_items_per_menu] = {4, 4, 4, 4}; //Hold the number of iteams that each item independently hold.
const uint8_t _max_menu_per_menu = 4; //Hold the max number of submenus that can exist in one single menu.
const uint8_t _max_menu_depth = 2; //Hold the max depth that a submenu can hold.
const uint8_t _menu_depth[_max_items_per_menu] = {2, 2, 2, 2}; //Hold the depth of each submenu.

//MAX MENU DEPTH is HIGHER_ROWS | MENU DATA LENGTH is LOWER_ROWS | _max_items_per_menu is COLUMNS

const char* _menu_data[_max_menu_depth][_max_menu_per_menu][_max_items_per_menu] = { //always set the menu depth of the biggest menu
  {{"ITEM:1DEPTH1MENU:1", "ITEM:2DEPTH1MENU:1", "ITEM:3DEPTH1MENU:1", "ITEM:4DEPTH1MENU:1"}},

  { {"ITEM:1DEPTH2MENU:1", "ITEM:2DEPTH2MENU:1", "ITEM:3DEPTH2MENU:1", "ITEM:4DEPTH2MENU:1"},
    {"ITEM:1DEPTH2MENU:2", "ITEM:2DEPTH2MENU:2", "ITEM:3DEPTH2MENU:2", "ITEM:4DEPTH2MENU:2"},
    {"ITEM:1DEPTH2MENU:3", "ITEM:2DEPTH2MENU:3", "ITEM:3DEPTH2MENU:3", "ITEM:4DEPTH2MENU:3"},
    {"ITEM:1DEPTH2MENU:4", "ITEM:2DEPTH2MENU:4", "ITEM:3DEPTH2MENU:4", "ITEM:4DEPTH2MENU:4"}
  }
};

menu _menu; //Create the MENU Object.
char _input; //Hold the Keyboard Input.
uint8_t _index = 0; //Hold the index of the menu.
uint8_t _display_size = 2; //Hold the displaying size.
uint8_t _current_depth = 0; //Hold the current depth the system is displaying.

bool keyboard_interrupt(char& data) {
  if (Serial.available()) {
    data = Serial.read(); //Read Serial Data
    if (data == '\n' || data == '\r') {
      return false;  // Don't process newline characters
    } else {
      return true; //Return true once a correct key is press
    }
  }
  return false; //If nothing to read, return false.
}

void print_(char msg[], int pos) {
  Serial.print("MENU: ");
  Serial.println(msg); //Print the data into Serial PORT, needs the Serial UART to be precreated, using Serial.begin() or similar.
}

void update_menu() {
  _menu.length = _menu_item_count[_index];
  if (_menu.length == 0)return;
  for (int i = 0; i < _menu_item_count[_index]; i += 1) {
    _menu.menu_data_array[i] = _menu_data[_current_depth][_index][i];
  }
  _index = 0;
  return;
}

void setup() {
  Serial.begin(9600);
  update_menu();
  print(_menu, _index, _display_size, print_);
}

void loop() {
  if (keyboard_interrupt(_input)) {
    if (handle_keyboard_input(_input, _index, _menu, FORWARD, BACKWARD, true, _display_size, print_) == -1 && _input == ENTER && _current_depth < _menu_depth[_index] - 1) {
      _current_depth += 1;
    } else if (_input == BACK && _current_depth >= 1) {
      _current_depth -= 1;
      _index = 0; //Always return to the first item of the menu.
    } else if (_input == ENTER && _current_depth == _menu_depth[_index] - 1) { // To access a menu.
    }
    else {
      return; //This is important, if the code does not end here when any other non defined key is press, the menu will update ramdonly.
    }
    update_menu();
    print(_menu, _index, _display_size, print_);
  }
}
/**
  Usage:
  Send trough SERIAL PORT the character specified in the handle_keyboard_input invoke.
  For the default example, use 'W' and 'S' to move trough the menu.

  If '#' system will move inside.
  If want to go back to the last menu, use '*'.
*/
