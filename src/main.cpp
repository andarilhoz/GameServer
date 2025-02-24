// GameServer.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#include "GameServer.h"
#include <iostream>
#include "Utils/Logger.h"

const int TCP_PORT = 5555;
const int UDP_PORT = 7777;

int main()
{
    Logger::setLogLevel(LogLevel::DEBUG);

    Logger::info("Main initializing");

    try {
        GameServer server(TCP_PORT, UDP_PORT);
        server.run();

        Logger::info("Enter para sair...");
        std::cin.get();
        server.stop();
    }
    catch (std::exception& e) {
        Logger::error("Erro: {}", e.what());
    }

    /*Server server(SERVER_PORT);
    server.start();*/
    return 0;
}

// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln
