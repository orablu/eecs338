CC = clang
# -g : allows use of GNU Debugger
# -Wall : show all warnings
CFLAGS = -g -Wall
LIBS = # None yet...
CSOURCE = client.c smoker_clnt.c smoker_xdr.c
SSOURCE = server.c smoker_svc.c smoker_xdr.c
RPCSRC = smoker.x
RPCGEN = smoker.h smoker_clnt.c smoker_svc.c smoker_xdr.c
CLIENT = client
SERVER = server

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CSOURCE) $(RPCGEN)
	$(CC) $(LIBS) $(CFLAGS) -o $(CLIENT) $(CSOURCE)

$(SERVER): $(SSOURCE) $(RPCGEN)
	$(CC) $(LIBS) $(CFLAGS) -o $(SERVER) $(SSOURCE)

$(RPCGEN): $(RPCSRC)
	rpcgen $(RPCSRC)

# 'clean' rule for remove non-source files
# To use, call 'make clean'
# Note: you don't have to call this in between every
#       compilation, it's only in case you need to
#       clean out your directory for some reason.
clean:
	@# Using the '@' sign suppresses echoing
	@# the line while the command is run
	rm -f $(CLIENT) $(SERVER) $(RPCGEN)
