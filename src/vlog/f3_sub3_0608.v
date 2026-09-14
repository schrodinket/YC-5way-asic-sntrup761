module f3_sub3_0608(input [1:0] a, input [1:0] b, output [1:0] c);
    wire [1:0] nb = (b==2'd0)?2'd0:(2'd3-b); f3_add_0608 s(.a(a),.b(nb),.c(c));
endmodule
