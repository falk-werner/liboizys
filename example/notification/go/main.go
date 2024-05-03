// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

package main

import (
	"flag"
	"net"
	"io"
	"fmt"
	"os"
	"os/signal"	
)

func read_message(connection net.Conn, c chan string) {
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
	
		message := string(data[:]);
		c <- message;	
	}
}

func main() {
	endpoint := flag.String("e", "/tmp/oizys_example_notify.sock", "endpoint to connect");
	flag.Parse();

	connection, err := net.Dial("unix", *endpoint);
	if err != nil {
		panic(err);
	}
	defer connection.Close()

	c := make(chan string)
	go read_message(connection, c)

	shutdown_requested := make(chan os.Signal, 1)
	signal.Notify(shutdown_requested, os.Interrupt)	

	done := false;
	for !done {
		select {
		case message := <-c:
			fmt.Println(message);
		case _ = <- shutdown_requested:
			done = true
		}
	
	}
}