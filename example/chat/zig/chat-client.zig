// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

const std = @import("std");
const net = std.net;
const os = std.os.linux;

const protobuf = @import("protobuf");
const chat = @import("chat.pb.zig");

const ProtoclError = error{ InvalidHeader, MessageEmpty };

const ArgParseState = enum { Init, ReadEnpoint, ReadUser };

fn send_message(allocator: std.mem.Allocator, connection: net.Stream, user: []const u8, content: []const u8) !void {
    const message = &chat.chat_message{ .user = protobuf.ManagedString.managed(user), .content = protobuf.ManagedString.managed(content) };
    const data = try message.encode(allocator);
    defer allocator.free(data);

    const header = [4]u8{ 0, @truncate((data.len >> 16) & 0xff), @truncate((data.len >> 8) & 0xff), @truncate(data.len & 0xff) };
    try connection.writeAll(&header);
    try connection.writeAll(data);
}

fn read_message(allocator: std.mem.Allocator, connection: net.Stream) !chat.chat_message {
    var header = [4]u8{ 0, 0, 0, 0 };
    _ = try connection.readAll(&header);
    if (header[0] != 0) {
        return ProtoclError.InvalidHeader;
    }

    const size = (@as(usize, header[1]) << 16) | (@as(usize, header[2]) << 8) | header[3];
    if (size == 0) {
        return ProtoclError.MessageEmpty;
    }

    const data = try allocator.alloc(u8, size);
    defer allocator.free(data);
    _ = try connection.readAll(data);

    const message = try chat.chat_message.decode(data, allocator);
    return message;
}

fn print_chat(connection: net.Stream) !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    while (!done.isSet()) {
        const message = try read_message(allocator, connection);
        defer message.deinit();
        std.debug.print("{s}: {s}\n", .{ message.user.getSlice(), message.content.getSlice() });
    }
}

var done = std.Thread.ResetEvent{};
fn handler(_: i32, _: *const os.siginfo_t, _: ?*const anyopaque) callconv(.C) void {
    done.set();
    std.debug.print("Shutdown..\n", .{});
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var endpoint = try allocator.dupe(u8, "/tmp/com_example_chat.sock");
    defer allocator.free(endpoint);
    var user = try allocator.dupe(u8, "Zwen");
    defer allocator.free(user);

    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);
    var state = ArgParseState.Init;
    for (args[1..]) |arg| {
        switch (state) {
            ArgParseState.Init => {
                if (std.mem.eql(u8, arg, "-e") or std.mem.eql(u8, arg, "--endpoint")) {
                    state = ArgParseState.ReadEnpoint;
                } else if (std.mem.eql(u8, arg, "-u") or std.mem.eql(u8, arg, "--user")) {
                    state = ArgParseState.ReadUser;
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
            ArgParseState.ReadUser => {
                allocator.free(user);
                user = try allocator.dupe(u8, arg);
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

    var thread = try std.Thread.spawn(.{}, print_chat, .{connection});

    const in = std.io.getStdIn();
    var buf = std.io.bufferedReader(in.reader());

    while (!done.isSet()) {
        var reader = buf.reader();
        const line = reader.readUntilDelimiterAlloc(allocator, '\n', 1024) catch {
            break;
        };
        defer allocator.free(line);
        try send_message(allocator, connection, user, line);
    }

    thread.join();
}
