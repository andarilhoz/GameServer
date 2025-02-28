cmake_minimum_required(VERSION 3.10)
project(GameServer)

set(CMAKE_CXX_STANDARD 17)

# Garante que o CMake busque o Boost corretamente
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_ITERATOR_DEBUG_LEVEL=2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

include_directories(/usr/local/include /usr/local/include/nlohmann)

# Encontra o Boost
find_package(Boost REQUIRED COMPONENTS system thread)

# Cria o executável do servidor
add_executable(GameServer 
    src/Game/Player.h src/Game/Player.cpp
    src/Server/UdpServer.h src/Server/UdpServer.cpp
    src/Utils/Logger.h src/Utils/Logger.cpp
    "src/Game/GameServer.h" "src/Game/GameServer.cpp"
    src/Server/TcpServer.h src/Server/TcpServer.cpp
    src/main.cpp
    src/Game/GameState.h src/Game/GameState.cpp
    src/Game/MovementHandler.h src/Game/MovementHandler.cpp
     
     "src/Game/Food.h" "src/Game/Food.cpp" "src/Game/FoodController.h" "src/Game/FoodController.cpp" "src/Game/MapController.h" "src/Game/MapController.cpp" "src/Game/GridCell.cpp" "src/GameConfig.h")

# Adiciona diretórios de includes
target_include_directories(GameServer PRIVATE ${Boost_INCLUDE_DIRS})

# Linka Boost ao projeto
target_link_libraries(GameServer PRIVATE Boost::system Boost::thread)
