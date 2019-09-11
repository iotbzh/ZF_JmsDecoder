all: zf


%.o: %.c
	gcc -c $< -o $@

%: %.o
	gcc $< -o $@
	
clean:
	rf -f zf