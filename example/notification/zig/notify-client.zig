const std = @import("std");
const net = std.net;
const os = std.os.linux;

const ProtoclError = error{ InvalidHeader, MessageEmpty };

const ArgParseState = enum { Init, ReadEnpoint };

fn send_message(connection: net.Stream, message: []const u8) !void {
    const header = [4]u8{ 0, @truncate((message.len >> 16) & 0xff), @truncate((message.len >> 8) & 0xff), @truncate(message.len & 0xff) };
    try connection.writeAll(&header);
    try connection.writeAll(message);
}

fn read_message(allocator: std.mem.Allocator, connection: net.Stream) ![]u8 {
    var header = [4]u8{ 0, 0, 0, 0 };
    _ = try connection.readAll(&header);
    if (header[0] != 0) {
        return ProtoclError.InvalidHeader;
    }

    const size = (@as(usize, header[1]) << 16) | (@as(usize, header[2]) << 8) | header[3];
    if (size == 0) {
        return ProtoclError.MessageEmpty;
    }

    const message = try allocator.alloc(u8, size);
    errdefer allocator.free(message);
    _ = try connection.readAll(message);

    return message;
}

var done = false;
fn handler(_: i32, _: *const os.siginfo_t, _: ?*const anyopaque) callconv(.C) void {
    done = true;
    std.debug.print("Shutdown..\n", .{});
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var endpoint = try allocator.dupe(u8, "/tmp/oizys_example_notify.sock");
    defer allocator.free(endpoint);

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);
    var state = ArgParseState.Init;
    for (args[1..]) |arg| {
        switch (state) {
            ArgParseState.Init => {
                if (std.mem.eql(u8, arg, "-e") or std.mem.eql(u8, arg, "--endpoint")) {
                    state = ArgParseState.ReadEnpoint;
                } else {
                    std.debug.print("usage: notify-client [-e <endpoint>]\n", .{});
                    return;
                }
            },
            ArgParseState.ReadEnpoint => {
                allocator.free(endpoint);
                endpoint = try allocator.dupe(u8, arg);
                state = ArgParseState.Init;
            },
        }
    }

    var sa = os.Sigaction{
        .handler = .{ .sigaction = &handler },
        .mask = os.empty_sigset,
        .flags = os.SA.SIGINFO | os.SA.RESETHAND,
    };
    _ = os.sigaction(os.SIG.INT, &sa, null);

    std.debug.print("connect to {s}\n", .{endpoint});
    var connection = try net.connectUnixSocket(endpoint);
    defer connection.close();

    while (!done) {
        const message = try read_message(allocator, connection);
        defer allocator.free(message);
        std.debug.print("{s}\n", .{message});
    }
}
