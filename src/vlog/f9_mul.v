module f9_mul (input [1:0] ar, input [1:0] ai, input [1:0] br, input [1:0] bi,
               output [1:0] cr, output [1:0] ci);
    wire [1:0] arbr,aibi,arbi,aibr;
    f3_mul m0(.a(ar),.b(br),.c(arbr));
    f3_mul m1(.a(ai),.b(bi),.c(aibi));
    f3_mul m2(.a(ar),.b(bi),.c(arbi));
    f3_mul m3(.a(ai),.b(br),.c(aibr));
    wire [1:0] neg_aibi = (aibi==0)?2'd0:(3-aibi);
    f3_add a0(.a(arbr),.b(neg_aibi),.c(cr));
    f3_add a1(.a(arbi),.b(aibr),.c(ci));
endmodule
