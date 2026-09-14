module f3_mul_0608(input [1:0] a, input [1:0] b, output [1:0] c);
    wire [3:0] p = a * b; assign c = p % 3;
endmodule
