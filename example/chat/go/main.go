// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

package main

import (
	"flag"
	"net"
	"io"
	"bufio"
	"fmt"
	"os"
	"os/signal"
	"strings"
	"google.golang.org/protobuf/proto"
)

func read_message(connection net.Conn, c chan ChatMessage) {
	for {
		header := make([]byte, 4);
		_, err := io.ReadFull(connection, header)
		if err != nil {
			return;
		}
	
		if header[0] != 0 {
			fmt.Println("protocol error: header[0] must be 0")
			return;
		}
		size := (header[1] << 16) | (header[2] << 8) | header[3];
		if size == 0 {
			fmt.Println("protocol error: empty messages are not allowed");
			return;
		}
	
		data := make([]byte, size);
		_, err = io.ReadFull(connection, data);
		if err != nil {
			fmt.Println(err)
			return;
		}
	
		message := &ChatMessage{};
		err = proto.Unmarshal(data, message);
		if err != nil {
			fmt.Println(err)
			return;
		}
		c <- *message;
	}
}

func read_console(c chan string) {
	for {
		reader := bufio.NewReader(os.Stdin)
		text, _ := reader.ReadString('\n')
		c <- strings.TrimSpace(text)
	}
}

func main() {
	endpoint := flag.String("e", "/tmp/com_example_chat.sock", "endpoint to connect");
	user := flag.String("u", "Jimmy", "user name");
	flag.Parse();

	connection, err := net.Dial("unix", *endpoint);
	if err != nil {
		panic(err);
	}
	defer connection.Close()

	c := make(chan ChatMessage)
	go read_message(connection, c)

	c2 := make(chan string)
	go read_console(c2)

	shutdown_requested := make(chan os.Signal, 1)
	signal.Notify(shutdown_requested, os.Interrupt)	

	done := false;
	for !done {
		select {
		case message := <-c:
			fmt.Printf("%s: %s\n", message.User, message.Content);
		case content := <-c2:
			message := &ChatMessage{};
			message.User = *user
			message.Content = content

			data, err := proto.Marshal(message);
			if err != nil {
				panic(err);
			}

			header := make([]byte, 4);
			header[0] = 0;
			header[1] = byte((len(data) >> 16) & 0xff);
			header[2] = byte((len(data) >>  8) & 0xff);
			header[3] = byte(len(data) & 0xff);
		
			_, err = connection.Write(header);
			if err != nil {
				panic(err);
			}
		
			_, err = connection.Write(data);
			if err != nil {
				panic(err);
			}		
		case _ = <- shutdown_requested:
			done = true
		}
	
	}
}