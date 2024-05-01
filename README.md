[![Build](https://github.com/falk-werner/liboizys/actions/workflows/build.yaml/badge.svg)](https://github.com/falk-werner/liboizys/actions/workflows/build.yaml)

# liboizys

Liboizys is a interprocess communication library based on 
[boost asio](https://www.boost.org/doc/libs/1_85_0/doc/html/boost_asio.html).
It provides a simple to use [WebSocket](https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API)-like API.

**Note**:  
*liboizys is optimized for usage in single thread environments. If you plan to use liboizys in a multithreaded environment, make sure that `send` is invoked from within the same thread that runs the assiciated `asio::io_context`.*

## Concept

The core of liboizys is the [session](inc/oizys/session_i.hpp) interface:

```C++
class session_i
{
public:
    virtual ~session_i() = default;
    virtual void send(std::string const & message) = 0;
    virtual void set_onclose(close_handler handler) = 0;
    virtual void set_onmessage(message_handler handler) = 0;
    virtual void close() = 0;
};
```

An instance of a session can be created using the `create_session` method:

```C++
boost::asio::io_context context;
boost::asio::local::stream_protocol::socket sock(context);

// ToDo: Connect socket

auto session = oizys::create_session(std::move(sock));
```

The API is resembles the widly used [WebSocket API](https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API):
- the `send` method is used to send messages to a connected peer
- the `set_onmessage` method is used to register a handler that is called whenever a message is received
- the `close` method is used to close the session
- the `set_onclose` method is used to register a handler that is called when the session is closed

Whenever an error occurs the session is closed.

**Note:**  
*The WebSocket API also specifies an `onerror` handler, which is not available in liboizys. Since each error leads to closing the session, this handler is omitted. The error code is instead provided to the close handler.*

## Protocol

Liboizys uses a simple binary protocol to transfer a series of messages between the
connected peers. Each message contains a 4 byte header and the actual content of the
message.

### Header

The header consists of a reserved byte and three bytes that encode the length of the message in [network byte order (big endian)](https://en.wikipedia.org/wiki/Endianness#Networking).

### Message Content

Binary content of the message.

### Constaints

- Messages without contents are prohibited and **must not** be transferred
- The max. message size is 16MByte - 1 byte (16777215 bytes)

### Examples

```
Hi -> 0x00 0x00 0x00 0x02 'H' 'i'

foo -> 0x00 0x00 0x00 0x03 'f' 'o' 'o'
```

## Quirks

- The library is optimized for usage in single threaded environment.  
  When used in a multithreaded environment, make sure to call `send` only from
  within the same thread that runs the `asio::io_context`.
- A `message_handler` should always be set in order to detect a closed connection early.  
  When a `message_handler` is set, the session is put into read mode. When the peer
  closes the connection, a pending read will immediatly return and the close handler
  is invoked.
- All handlers are removed when the session is closed.  
  Be prepared that all message handlers are removed when a session is closed. You
  should no access any variable captured by a `close_handler` after `close` is
  called.
- Finish of `send` cannot be detected.  
  The API does not allow to detect when a message is send nor when it is delivered
  to the peer.

## Build and Install

```bash
cmake -B build
cmake --build build
sudo cmake --install build
```

### CMake Options

| Option | Default | Description |
| ------ | ------- | ----------- |
| WITHOUT_TEST | OFF | Disables unit tests |
| WITHOUT_EXAMPLES | OFF | Disabled examples |

### Coverage

To enable code coverage, use `CMAKE_BUILD_TYPE=Coverage`:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Coverage
```

Afterwards, coverage of unit tests can be created using `coverage-report` build target:

```bash
cmake --build build --target coverage-report
```

### Dependencies

- [boost asio](https://www.boost.org/doc/libs/1_85_0/doc/html/boost_asio.html)
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)

Test dependencies:

- [googletest](https://github.com/google/googletest)

Code coverage dependencies:

- [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html)
- [lcov](https://github.com/linux-test-project/lcov)

Examples dependencies:

- [protobuf](https://protobuf.dev/)

## References

- [boost asio](https://www.boost.org/doc/libs/1_85_0/doc/html/boost_asio.html)
- [WebSocket](https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API)
- [protobuf](https://protobuf.dev/)
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
- [googletest](https://github.com/google/googletest)
