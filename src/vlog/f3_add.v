module f3_add (input [1:0] a, input [1:0] b, output [1:0] c);
    wire [2:0] sum = a + b;
    assign c = (sum >= 3) ? (sum - 3) : sum[1:0];
endmodule
