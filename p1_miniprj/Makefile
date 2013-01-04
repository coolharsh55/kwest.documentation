kwest : kwest_main.o import.o extract_audio_taglib.o dbfunc.o
	gcc -o kwest kwest_main.o import.o extract_audio_taglib.o dbfunc.o -lfuse -lsqlite3 -ltag_c 
kwest_main.o : kwest_main.c
	gcc -c kwest_main.c -D_FILE_OFFSET_BITS=64 -lfuse
import.o : import.c
	gcc -c import.c -lsqlite3
extract_audio_taglib.o : extract_audio_taglib.c
	gcc -c extract_audio_taglib.c -ltag_c
dbfunc.o: dbfunc.c
	gcc -c dbfunc.c -lsqlite3

clean:
	rm -rf *.o
	rm -rf *.db
	rm -rf *.txt
	rm -rf kwest
	rm -rf ~/.config/kwestdb
