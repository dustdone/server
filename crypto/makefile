
OBJS = main.o hmac_sha1.o sha1.o memxor.o
CC = g++ -std=c++11

main: $(OBJS)
	$(CC) -o $@ $^
	rm $(OBJS)

main.o: main.cpp hmac.h Base64.h
	$(CC) -c $<

hmac_sha1.o: hmac_sha1.c hmac.h sha1.h memxor.h
	$(CC) -c $<

sha1.o: sha1.c sha1.h memxor.h
	$(CC) -c $<

memxor.o: memxor.c memxor.h
	$(CC) -c $<

clean:
	rm main $(OBJS)
