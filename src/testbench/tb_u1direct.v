`timescale 1ns/1ps
// U1'i DOGRUDAN test: {0,1,2} girdi (C native), remap YOK
// compare u1_r3 single-engine output against an independent reference (mod-3, reduced)
module tb_u1direct;
    localparam P=761, W=1522;
    reg clk=0; always #5 clk=~clk;
    reg rst=1, start=0;
    reg [W-1:0] ar, br;
    wire [W-1:0] cr; wire done; wire [15:0] cc;
    u1_r3_1engine dut(.clk(clk),.rst(rst),.start(start),.ar(ar),.br(br),
        .cr(cr),.done(done),.cycle_count(cc));

    integer ea[0:P-1], ga[0:P-1], ref_c[0:P-1];
    integer i,j,k;
    initial begin
        // {0,1,2} girdi (C native encoding, -1 yok)
        for(i=0;i<P;i=i+1) begin
            ea[i]=$random%3; if(ea[i]<0)ea[i]=-ea[i];
            ga[i]=$random%3; if(ga[i]<0)ga[i]=-ga[i];
            ar[2*i+:2]=ea[i][1:0];
            br[2*i+:2]=ga[i][1:0];
        end
        // reference: convolution + x^761=x+1 reduction, mod-3 {0,1,2}
        begin: rc
            integer conv[0:2*P-2];
            for(k=0;k<2*P-1;k=k+1) conv[k]=0;
            for(i=0;i<P;i=i+1) for(j=0;j<P;j=j+1) conv[i+j]=conv[i+j]+ea[i]*ga[j];
            for(k=2*P-2;k>=P;k=k-1) begin
                conv[k-P+1]=conv[k-P+1]+conv[k];
                conv[k-P]=conv[k-P]+conv[k];
                conv[k]=0;
            end
            for(k=0;k<P;k=k+1) ref_c[k]=((conv[k]%3)+3)%3;
        end
        rst=1; start=0; repeat(3)@(negedge clk); rst=0; @(negedge clk);
        start=1; @(negedge clk); start=0;
        wait(done); #1;
        begin: cmp
            integer err; err=0;
            for(k=0;k<P;k=k+1) begin
                if(cr[2*k+:2] != ref_c[k][1:0]) begin
                    err=err+1;
                    if(err<=8) $display("FARK k=%0d: u1=%0d ref=%0d",k,cr[2*k+:2],ref_c[k]);
                end
            end
            $display("U1 DOGRUDAN (remap yok): %0d/%0d error -> %s",err,P,(err==0)?"PASS":"FAIL");
        end
        $finish;
    end
endmodule
