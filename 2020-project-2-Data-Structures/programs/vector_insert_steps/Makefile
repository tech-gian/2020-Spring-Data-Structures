# paths
MODULES = ../../modules
INCLUDE = ../../include

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -Werror -g -I$(INCLUDE)
LDFLAGS = -lm

# Αρχεία .o
OBJS = vector_insert_steps.o $(MODULES)/UsingDynamicArray/ADTVector.o

# Το εκτελέσιμο πρόγραμμα
EXEC = vector_insert_steps

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS = real
# Σαν όρισμα γίνεται δεκτό μόνο το 'real' ή το 'amortized'

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	valgrind ./$(EXEC) $(ARGS)
