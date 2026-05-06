cmake_minimum_required(VERSION 3.15)
project(OPThread VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(OPThread 
    ${SOURCES} 
    "include/entities/algorithms/loom/loom_constants.h" 
    "include/entities/algorithms/loom/loom.h" 
    "src/entities/algorithms/geometry_utils.cpp" 
    "include/entities/algorithms/fractal/koch.h" 
    "src/entities/algorithms/fractal/koch.cpp" 
    "src/entities/algorithms/loom/loom.cpp" 
    "include/entities/algorithms/loom/loom_utils.h" 
    "include/entities/algorithms/loom/tessellated.h" 
    "include/entities/algorithms/loom/fibonacci.h" 
    "include/entities/algorithms/loom/multi_fibonacci.h" 
    "include/entities/algorithms/loom/golden_tessellation.h" 
    "include/entities/algorithms/loom/bezier_triangle.h" 
    "include/entities/algorithms/loom/pythagoras_tree.h" 
    "include/entities/algorithms/loom/gosper_curve.h" 
    "include/entities/algorithms/loom/chaos_blend.h" 
    "include/entities/algorithms/loom/intertwined_spirals.h" 
    "src/entities/algorithms/loom/loom_utils.cpp" 
    "src/entities/algorithms/loom/tessellated.cpp" 
    "src/entities/algorithms/loom/fibonacci.cpp" 
    "src/entities/algorithms/loom/multi_fibonacci.cpp" 
    "src/entities/algorithms/loom/golden_tessellation.cpp" 
    "src/entities/algorithms/loom/bezier_triangle.cpp" 
    "src/entities/algorithms/loom/pythagoras_tree.cpp" 
    "src/entities/algorithms/loom/gosper_curve.cpp" 
    "src/entities/algorithms/loom/voronoi.cpp" 
    "src/entities/algorithms/loom/chaos_blend.cpp" 
    "src/entities/algorithms/loom/intertwined_spirals.cpp" 
    "include/entities/algorithms/geometry_utils.h" 
    "include/entities/visual/performance_monitor.h" 
    "include/core/loom_orchestrator.h" 
    "include/entities/algorithms/fractal/NewtonFractal.h"
    "src/entities/visual/performance_monitor.cpp" 
    "src/entities/graph.cpp"
 "src/entities/algorithms/fractal/NewtonFractal.cpp" "include/entities/algorithms/fractal/NewtonField.h" "src/entities/algorithms/fractal/NewtonField.cpp" "include/entities/algorithms/fractal/NewtonLoomGenerator.h" "src/entities/algorithms/fractal/NewtonLoomGenerator.cpp" "include/entities/algorithms/fractal/newton_fractal.h" "include/entities/algorithms/loom/apollonian_gasket.h" "src/entities/algorithms/loom/apollonian_gasket.cpp" "include/ui/ScreenState.h" "include/ui/IScreen.h" "include/ui/ScreenMain.h" "include/ui/ScreenShowGraph.h" "include/ui/ScreenGenerate.h" "include/ui/ScreenPersistance.h" "include/ui/Button.h"  "include/entities/algorithms/loom/kruskal.h")

target_include_directories(OPThread PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SFML_ROOT "C:/SFML" CACHE PATH "Ruta de instalación de SFML 3")
target_include_directories(OPThread PRIVATE "${SFML_ROOT}/include")
target_link_directories(OPThread PRIVATE "${SFML_ROOT}/lib")

target_link_libraries(OPThread PRIVATE
    debug sfml-graphics-d optimized sfml-graphics
    debug sfml-window-d   optimized sfml-window
    debug sfml-system-d   optimized sfml-system
)

if(WIN32)
    file(GLOB SFML_DLLS "${SFML_ROOT}/bin/*.dll")
    if(SFML_DLLS)
        add_custom_command(TARGET OPThread POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_DLLS} $<TARGET_FILE_DIR:OPThread>
            COMMENT "Copiando DLLs de SFML")
    else()
        message(WARNING "No se encontraron DLLs de SFML en ${SFML_ROOT}/bin/")
    endif()
endif()

add_custom_command(TARGET OPThread POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:OPThread>/assets"
    COMMENT "Copiando assets")