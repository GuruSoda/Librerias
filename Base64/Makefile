ejemplo.exe: base64.o ejemplo.o
	@echo "Terminado"
	$(CC) $? -o /tmp/$@
	/tmp/$@

%.o:%.c
	# @echo "1="$ "2="$@ "3="$? "4="$* 
	$(CC) -c $? -o $@

