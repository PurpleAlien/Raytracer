EXE_NAME = raytrace 
OBJS = raytrace.o vectors.o fileformat.o tokenizer.o colour.o sphere.o triangle.o 3dsloader.o noise.o
CC = gcc 
FLAGS = -Wall -O2
LIBS = -lm -lpthread

.c.o:
	$(CC) $< -c $(FLAGS) $(INC)

$(EXE_NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(FLAGS) $(LIBS)
clean:
	rm -f $(OBJS) $(EXE_NAME)
