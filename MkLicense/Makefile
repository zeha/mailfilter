
OUT=../MailFilter/out/
BIN=mklic

$(OUT)$(BIN): mklic.cpp ../MailFilter/Licensing/adler32.c
	g++ -o $(OUT)$(BIN) mklic.cpp ../MailFilter/Licensing/adler32.c

clean:
	rm $(OUT)$(BIN)
