module f3_mul (input [1:0] a, input [1:0] b, output [1:0] c);
    wire [3:0] prod = a * b;
    assign c = (prod >= 3) ? (prod - 3) : prod[1:0];
endmodule
