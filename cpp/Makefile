.DEFAULT_GOAL := all
.PHONY: all

CXX=clang++
CXXFLAGS=--std=c++17 -g3

CLIENT_SRCS := client.cpp
SERVER_SRCS := server.cpp

BUILDDIR := build
CLIENT := $(BUILDDIR)/chatclient
SERVER := $(BUILDDIR)/chatserver

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRCS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRCS) -o $@

$(SERVER): $(SERVER_SRCS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(SERVER_SRCS) -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
