// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

package main

import (
	"flag"
	"net"
	"io"
	"fmt"
)

func main() {
	endpoint := flag.String("e", "/tmp/com_example_echo.sock", "endpoint to connect");
	message := flag.String("m", "Yay!", ",message to send");	
	flag.Parse();

	connection, err := net.Dial("unix", *endpoint);
	if err != nil {
		panic(err);
	}
	defer connection.Close()

	data := []byte(*message)
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

	_, err = io.ReadFull(connection, header)
	if err != nil {
		panic(err);
	}

	if header[0] != 0 {
		panic("protocol error: header[0] must be 0")
	}
	size := (header[1] << 16) | (header[2] << 8) | header[3];
	if size == 0 {
		panic("protocol error: empty messages are not allowed");
	}

	data = make([]byte, size);
	_, err = io.ReadFull(connection, data);
	if err != nil {
		panic(err)
	}

	msg := string(data[:]);
	fmt.Println(msg);

}