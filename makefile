OBJ = spell-card-gen.o

spell-card-gen: $(OBJ)
	gcc -o $@ $^

%.o: %.c
	gcc -c $< -g

clean:
	rm -rf *.o
