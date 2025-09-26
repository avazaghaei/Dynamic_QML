# Qt6 QML/C++ Dynamic Component System (Qt/C++/QML)

This C# console application demonstrates the implementation of the Command design pattern to create a flexible and extensible command processor. It supports a variety of operations—increment, decrement, double, random addition—and includes an undo feature to revert the most recent command. The project emphasizes clean code architecture, efficient memory usage through design patterns (such as the Singleton), and object-oriented best practices.

## 📋 Project Overview
A sophisticated Qt6 application demonstrating real-time integration between C++ backend and QML frontend. The system dynamically creates UI components based on JSON configuration files and implements intelligent position-based data flow control.

## ⚠️ Critical Configuration Requirement

**🚨 IMPORTANT: Config files must be located in the executable directory!**

For the application to work correctly, you **MUST** place the `configs/` folder containing all JSON files in the same directory as your executable file.

### Required File Structure:

	[Your Build Directory]/
	├── Project.exe (or Project executable)
	└── configs/
	├── backend_1.json
	├── backend_3.json
	├── backend_5.json
	├── frontend_1.json
	├── frontend_3.json
	└── frontend_5.json
	
	
### 🔧 If MovableBoxes Don't Show Up:
If the MovableBox components are not visible on the UI, **the most common cause is missing or misplaced config files**. Please verify:

1. ✅ The `configs/` folder exists in the same directory as your executable
2. ✅ All JSON files are present and properly formatted
3. ✅ No error messages about missing config files in the console output

## 🛠️ Build Instructions

### Prerequisites:
- Qt6 (6.8.x recommended)
- CMake (latest version)
- C++17 compatible compiler


## 🎯 Running the Application:

1. Build the project using the steps above

2. Copy the configs/ folder to your build output directory

3. Run the executable:

## 📁 Project Structure
Project/
├── CMakeLists.txt
├── main.cpp
├── Source/
│   ├── manager.h
│   ├── manager.cpp
│   ├── datagenerator.h
│   └── datagenerator.cpp
├── qml/
│   ├── main.qml
│   ├── MovableBox.qml
│   └── qml.qrc
└── configs/          # ⚠️ MUST be placed near executable
    ├── backend_1.json
    ├── backend_3.json
    ├── backend_5.json
    ├── frontend_1.json
    ├── frontend_3.json
    └── frontend_5.json
	
## 🚀 Features Implemented
✅ Core Functionality:

Dynamic QML Component Creation: UI components created at runtime from JSON configuration

C++ Backend Data Generators: Multiple data sources generating values at configurable intervals

Real-time C++/QML Communication: Seamless data flow between backend and frontend

Position-Aware Control: Backend pauses updates when components are in the right screen half

JSON-Driven Architecture: Complete configuration through external JSON files

## ✅ Technical Highlights:

Modern C++17 with Qt6 best practices

CMake build system integration

Memory-safe resource management

Comprehensive error handling

Real-time console logging

## Debugging Tips:

Check the console output for error messages

Verify all JSON files are valid using a JSON validator

Ensure the configs/ folder path is correct relative to the executable

Look for QML component creation errors in the console

## 📊 Configuration Files

The application uses two types of configuration files:

### Backend Config (e.g., backend_3.json):
[
    {
        "id": "generator1",
        "msec": 500,
        "min": 10,
        "max": 100
    }
]

### Frontend Config (e.g., frontend_3.json):
[
    {
        "id": "box1",
        "x": 50,
        "y": 100,
        "color-hex": "#FF5733",
        "dataSource": "generator1"
    }
]

## 📞 Support

If you encounter any issues:

1. First, verify the configs/ folder location as described above

2. Check the console output for specific error messages

3. Ensure all prerequisites are installed correctly

🎯 Success Criteria

## When the application runs correctly, you should see:

✅ Colored rectangles moving horizontally across the screen

✅ Real-time number updates inside each rectangle

✅ Console messages showing data generator activity

✅ Generators pausing/resuming based on rectangle position

✅ Smooth animation and responsive UI

## ⭐ Remember: The configs/ folder location is critical for proper functionality!