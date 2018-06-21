bin_PROGRAMS = server_am client_am
server_am_SOURCES = \
	main_server.cpp\
	server.cpp\
	session.cpp\
	ext_includes.hpp\
	parser.cpp
client_am_SOURCES = \
	client.cpp\
	client_connection.cpp
AM_CXX = /usr/bin/clang++
AM_CPPFLAGS = -ggdb3 -Wall -Wpedantic -Iinclude -std=c++17 
AM_LDFLAGS = -pthread 
