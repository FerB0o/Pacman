# Directorios de origen y destino
SRC_DIR := src
BIN_DIR := bin
INCLUDES := -IC:/SFML/include -Iinclude
LIBS := -LC:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Obtener todos los archivos .cpp en el directorio de origen
CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(CPP_FILES:.cpp=.o)

# Nombre del ejecutable final
TARGET := $(BIN_DIR)/Pacman.exe

# Compilación del ejecutable
$(TARGET): $(OBJ_FILES)
	mkdir -p $(BIN_DIR)
	$(CXX) $(OBJ_FILES) -o $(TARGET) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Regla por defecto
all: $(TARGET)

# Limpiar los archivos binarios generados
clean:
	if exist "$(SRC_DIR)\*.o" del /Q "$(SRC_DIR)\*.o"
	if exist "$(BIN_DIR)\*.exe" del /Q "$(BIN_DIR)\*.exe"

# Regla para ejecutar el juego
run: $(TARGET)
	$(TARGET)
 
.PHONY: all clean run