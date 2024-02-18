# **MUI** (*Murky User Interface*)
**MUI** is a custom made GUI framework i made in c++ which is built on top of the win32 windows api

(*the code is probably shit but at least it works :), if you have any code improvments i'll be happy*)
## Features
* Support new style
  * If you want to disable it change `#define NEW_STYLE 1` to `#define NEW_STYLE 0` 
* Window
  * You can change its minimal size and maximal size
  * You can change its title and icon
  * You can select whether or not hide on close (inside `MUI.h` change `#define HIDE_ON_CLOSE 0` to `#define HIDE_ON_CLOSE 1`)
* Kind of components based system with callbacks
* Components list:
   * Button
   * CheckBox
   * Grid (you have to explicitly set the grid by using `window->SetGrid(&grid)` as in the demo project)
   * Image
   * ListView
   * RadioGroup
   * RadioButton
   * TextBlock
   * TextBox
* Custom coloring of each component type(works best with `#define NEW_STYLE 0` )
## Demo project
*You can find a demo project inside the `Demo` folder, this project is where i test new features and additions*

![demo-image](example.png)

## Building instructions
1. Download Visual Studio 2022 with Windows 11 SDK (10.0.22000.0)
2. Open the `MUI.sln` solution file
3. Build and compile!
