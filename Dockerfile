# Usa Alpine como base (imagem leve)
FROM alpine:latest

# Define o diretório de trabalho dentro do container
WORKDIR /app

# Instala dependências essenciais
RUN apk add --no-cache \
    curl \
    boost-dev \
    boost-system \
    boost-thread \
    g++ \
    cmake \
    make \
    boost-dev \
    linux-headers \
    libstdc++ \
    libc6-compat

RUN mkdir -p /usr/local/include/nlohmann

# Baixa o JSON for Modern C++ diretamente do GitHub
RUN curl --fail --silent --show-error -L -o /usr/local/include/nlohmann/json.hpp \
    https://github.com/nlohmann/json/releases/latest/download/json.hpp


# Copia o CMakeLists.txt e os arquivos do código-fonte (respeitando a estrutura)
COPY CMakeLists.txt ./
COPY src/ ./src/

# Cria o diretório de build e compila o projeto
RUN mkdir build && cd build && cmake .. && make

# Define o comando padrão para rodar o servidor
CMD ["./build/GameServer"]
