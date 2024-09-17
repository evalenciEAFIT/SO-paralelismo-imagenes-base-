CXX = g++
CXXFLAGS = -std=c++17 -Wall -fopenmp
LDFLAGS = -ljpeg

SRCS = main.cpp jpeg_manager.cpp procesador_imagen.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = programa_jpeg

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) imagen_ejemplo.jpg

install-deps:
	@echo "Instalando dependencias..."
	@chmod +x install_dependencies.sh
	@./install_dependencies.sh

.PHONY: all clean run install-deps