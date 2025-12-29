const std = @import("std");
const http = std.http;

pub fn parse(website: []const u8)!void{
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const urli = try std.Uri.parse(website);
    var client = http.Client{ .allocator = gpa.allocator() };
    var buff: [4069]u8 = undefined;
    const option = http.Client.RequestOptions{ .server_header_buffer = &buff};
    var req = try client.open(http.Method.GET, urli, option);
    defer req.deinit();
    _ = try req.send();
    _ = try req.finish();
    _ = try req.wait();
    std.debug.print("Info: \n , content_disposition: {any} , {any} , {any} , {any}" , .{req.response.content_disposition , req.response.content_length , req.response.location , req.response.content_type , });
}
