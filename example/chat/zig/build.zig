// SPDX-License-Identifier: MPL-2.0
// SPDX-FileCopyrightText: Copyright 2024 Falk Werner

const std = @import("std");
const protobuf = @import("protobuf");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const protobuf_dep = b.dependency("protobuf", .{
        .target = target,
        .optimize = optimize,
    });

    const gen_proto = b.step("gen-proto", "generates zig files from protocol buffer definitions");

    const protoc_step = protobuf.RunProtocStep.create(b, protobuf_dep.builder, target, .{
        .destination_directory = .{
            .path = ".",
        },
        .source_files = &.{
            "messages.proto",
        },
        .include_directories = &.{"."},
    });

    gen_proto.dependOn(&protoc_step.step);

    const exe = b.addExecutable(.{
        .name = "chat-client",
        .root_source_file = .{ .path = "chat-client.zig" },
        .target = b.host,
    });
    b.installArtifact(exe);
    exe.root_module.addImport("protobuf", protobuf_dep.module("protobuf"));

    const run_exe = b.addRunArtifact(exe);
    exe.step.dependOn(gen_proto);

    const run_step = b.step("run", "Run the application");
    run_step.dependOn(&run_exe.step);
}
