all: backend frontend

backend:
	gcc BackEnd.c funcs.c users_lib.h users_lib.o -o backend

frontend:
	gcc FrontEnd.c funcs.c users_lib.h users_lib.o -o frontend
