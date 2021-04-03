SET(IMGUI_STATIC true)
FILE(GLOB 
    cimgui_files 
	"../../lib/cimgui/*.cpp" 
	"../../lib/cimgui/imgui/*.cpp"
	"../../lib/cimgui/imgui/backends/imgui_impl_glfw.cpp"
	"../../lib/cimgui/imgui/backends/imgui_impl_opengl3.cpp"
)

ADD_LIBRARY(cimgui ${cimgui_files})

TARGET_COMPILE_DEFINITIONS(cimgui PRIVATE IMGUI_IMPL_API=extern\ \"C\" IMGUI_IMPL_OPENGL_LOADER_GLAD)
TARGET_INCLUDE_DIRECTORIES(cimgui PRIVATE ../../lib/cimgui/imgui)
TARGET_LINK_LIBRARIES(cimgui PRIVATE stdc++)
TARGET_INCLUDE_DIRECTORIES(cimgui PUBLIC ../../lib/cimgui/generator/output)
TARGET_INCLUDE_DIRECTORIES(cimgui PUBLIC ../../lib/cimgui)
TARGET_LINK_LIBRARIES(cimgui PRIVATE glfw glad)
TARGET_COMPILE_DEFINITIONS(cimgui INTERFACE CIMGUI_DEFINE_ENUMS_AND_STRUCTS)

IF(WIN32)
    TARGET_LINK_LIBRARIES(cimgui PRIVATE imm32)
ENDIF()

IF(APPLE)
	TARGET_LINK_LIBRARIES(cimgui PRIVATE "-framework CoreFoundation")
ENDIF()