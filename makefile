all: backend frontend

rr: backend frontend swipe

clear:
	rm backend frontend BACKF* FRONTF* CONFIG

backend:
	gcc BackEnd.c funcs.c users_lib.h users_lib.o -o backend

frontend:
	gcc FrontEnd.c funcs.c -o frontend

swipe:
	clear

