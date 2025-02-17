# Install script for directory: /home/obob/cs381/as2/raylib-cpp/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/obob/cs381/as2/raylib-cpp/include/AudioDevice.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/AudioStream.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/AutomationEventList.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/BoundingBox.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Camera2D.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Camera3D.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Color.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/FileData.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/FileText.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Font.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Functions.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Gamepad.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Image.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Keyboard.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Material.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Matrix.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Mesh.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/MeshUnmanaged.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Model.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/ModelAnimation.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Mouse.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Music.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Ray.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/RayCollision.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/RaylibException.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/raylib-cpp-utils.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/raylib-cpp.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/raylib.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/raymath.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Rectangle.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/RenderTexture.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Shader.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Sound.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Text.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Texture.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/TextureUnmanaged.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Touch.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Vector2.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Vector3.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Vector4.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/VrStereoConfig.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Wave.hpp"
    "/home/obob/cs381/as2/raylib-cpp/include/Window.hpp"
    )
endif()

