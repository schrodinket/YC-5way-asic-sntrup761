module f3_add_0608(input [1:0] a, input [1:0] b, output [1:0] c);
    wire [2:0] s = a + b; assign c = (s >= 3) ? (s - 3) : s;
endmodule
