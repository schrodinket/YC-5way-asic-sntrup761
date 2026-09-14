// f3_sub3: F_3 subtraction (a - b mod 3)
module f3_sub3(input [1:0] a, input [1:0] b, output [1:0] c);
    wire [1:0] nb = (b==2'd0)?2'd0:(2'd3-b);
    f3_add s(.a(a),.b(nb),.c(c));
endmodule
