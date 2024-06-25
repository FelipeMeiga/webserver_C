PROJ_NAME = web_server_C

CC = gcc
CFLAGS = -Wall -Wpedantic -Wextra -O2 -g
LDFLAGS = -pthread

RM = rm -f

all: $(PROJ_NAME)

$(PROJ_NAME): main.o parse_options.o start_server.o handle_client.o init_log_buffer.o log_writer.o add_log_entry.o handle_signal.o get_content_type.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c parse_options.h start_server.h log.h
	$(CC) -c $(CFLAGS) -o main.o main.c

parse_options.o: parse_options.c parse_options.h
	$(CC) -c $(CFLAGS) -o parse_options.o parse_options.c

start_server.o: start_server.c start_server.h parse_options.h log.h handle_client.h
	$(CC) -c $(CFLAGS) -o start_server.o start_server.c

handle_client.o: handle_client.c handle_client.h log.h start_server.h
	$(CC) -c $(CFLAGS) -o handle_client.o handle_client.c

init_log_buffer.o: init_log_buffer.c log.h
	$(CC) -c $(CFLAGS) -o init_log_buffer.o init_log_buffer.c

log_writer.o: log_writer.c log.h start_server.h
	$(CC) -c $(CFLAGS) -o log_writer.o log_writer.c

add_log_entry.o: add_log_entry.c log.h start_server.h
	$(CC) -c $(CFLAGS) -o add_log_entry.o add_log_entry.c

handle_signal.o: handle_signal.c start_server.h log.h
	$(CC) -c $(CFLAGS) -o handle_signal.o handle_signal.c

get_content_type.o: get_content_type.c handle_client.h start_server.h
	$(CC) -c $(CFLAGS) -o get_content_type.o get_content_type.c

clean:
	$(RM) *.o

clean_all:
	$(RM) *.o *.d $(PROJ_NAME) *~

.PHONY: all clean clean_all

