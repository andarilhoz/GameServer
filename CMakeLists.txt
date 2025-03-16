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

# Cria o execut�vel do servidor
add_executable(GameServer 
    "src/Game/Entities/Player.h" "src/Game/Entities/Player.cpp"
    "src/Game/Networking/UdpServer.h" "src/Game/Networking/UdpServer.cpp"
    "src/Utils/Logger.h" "src/Utils/Logger.cpp"
    "src/Game/Core/GameServer.h" "src/Game/Core/GameServer.cpp"
    "src/Game/Networking/TcpServer.h" "src/Game/Networking/TcpServer.cpp"
    "src/main.cpp"
    "src/Game/Core/GameState.h" "src/Game/Core/GameState.cpp"
    "src/Game/Systems/MovementHandler.h" "src/Game/Systems/MovementHandler.cpp"
     
     "src/Game/Entities/Food.h" "src/Game/Entities/Food.cpp" "src/Game/Systems/FoodController.h" "src/Game/Systems/FoodController.cpp" "src/Game/Systems/MapController.h" "src/Game/Systems/MapController.cpp" "src/Game/Entities/GridCell.cpp" "src/Game/Config/GameConfig.h" "src/Game/Core/GameLoop.h" "src/Game/Core/GameLoop.cpp" "src/Game/Systems/PlayerSystem.h" "src/Game/Systems/PlayerSystem.cpp" "src/Game/Networking/ConnectionManager.h" "src/Game/Networking/ConnectionManager.cpp" "src/Game/Networking/Messages/MessageHandler.h" "src/Game/Networking/Messages/GameMessage.h" "src/Game/Networking/Messages/GameMessage.cpp" "src/Game/Networking/Messages/MessageHandler.cpp" "src/Utils/Time.h" "src/Utils/Time.cpp" "src/Game/Systems/CollisionSystem.h" "src/Game/Systems/CollisionSystem.cpp")

# Adiciona diret�rios de includes
target_include_directories(GameServer PRIVATE ${Boost_INCLUDE_DIRS})

# Linka Boost ao projeto
target_link_libraries(GameServer PRIVATE Boost::system Boost::thread)
