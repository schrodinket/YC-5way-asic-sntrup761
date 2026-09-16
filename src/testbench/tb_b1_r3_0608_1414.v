`timescale 1ns/1ps
module tb;
    reg clk, rst, start;
    reg [1535:0] ar, br;
    wire [1521:0] cr;
    wire done; wire [15:0] cycle_count;
    b1_r3_761_serial_0608_1414 dut(.clk(clk),.rst(rst),.start(start),.ar(ar),.br(br),
        .cr(cr),.done(done),.cycle_count(cycle_count));
    integer i,v,hata,r,fd;
    reg [1:0] beklenen [0:760];
    initial clk=0; always #5 clk=~clk;
    initial begin
        hata=0;
        fd=$fopen("inp_r3_test.txt","r"); ar=0;br=0;
        for(i=0;i<768;i=i+1) begin
            if(i<761) begin r=$fscanf(fd,"%d",v); ar[2*i+:2]=v[1:0]; end
            else ar[2*i+:2]=0;
        end
        for(i=0;i<768;i=i+1) begin
            if(i<761) begin r=$fscanf(fd,"%d",v); br[2*i+:2]=v[1:0]; end
            else br[2*i+:2]=0;
        end
        $fclose(fd);
        fd=$fopen("ref_r3_761.txt","r");
        for(i=0;i<761;i=i+1) begin r=$fscanf(fd,"%d",v); beklenen[i]=v[1:0]; end
        $fclose(fd);
        rst=1; start=0; repeat(2) @(negedge clk); rst=0; @(negedge clk);
        start=1; @(negedge clk); start=0;
        wait(done==1); #1;
        $display("cycle_count=%0d", cycle_count);
        for(i=0;i<761;i=i+1) if(cr[2*i+:2]!==beklenen[i]) begin
            if(hata<5) $display("FAIL c[%0d]: v=%0d bek=%0d",i,cr[2*i+:2],beklenen[i]);
            hata=hata+1; end
        if(hata==0) $display("=== b1_r3_761_serial PASS (761/761 == ref R/3, %0d cycle) ===",cycle_count);
        else $display("=== %0d HATA ===",hata);
        $finish;
    end
    initial begin #50000; $display("TIMEOUT"); $finish; end
endmodule
