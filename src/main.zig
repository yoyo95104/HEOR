const std = @import("std");
const parse = @import("./root.zig");

pub fn main()!void{
    var buff: [1024]u8 = undefined;
    const reader = std.io.getStdIn().reader();
    const writer = std.io.getStdOut().writer();
    try writer.print("Enter the website name: \n", .{});
    const word = std.mem.trimRight(u8, try reader.readUntilDelimiter(&buff, '\n'), "\r");
    if (word.len > 0) {
        try writer.print("{c}", .{word[0]});
            for (word[1..]) |letter| {
                try writer.print(" {c}", .{letter});
            }
        try writer.writeAll("\n");
    }else {
        try writer.print("No input detected ...\n", .{});
    }
    try parse.parse(word);
}
