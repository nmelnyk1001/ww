# build an executable named ww from main.c
ww: main.o ww.o strbuf.o
	gcc -g -std=c99 -Wvla -Wall -o ww main.o ww.o strbuf.o
main.o: main.c ww.h
	gcc -c -g -std=c99 -Wvla -Wall main.c
ww.o: ww.c ww.h
	gcc -c -g -std=c99 -Wvla -Wall ww.c
strbuf.o: strbuf.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall strbuf.c 

clean: 
	$(RM) ww
	$(RM) ww.o
	$(RM) main.o
	$(RM) strbuf.o