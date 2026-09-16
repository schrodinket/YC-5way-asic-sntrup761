`timescale 1ns/1ps
module tb;
    reg clk, rst, start;
    reg [1535:0] ar, br;
    wire [3069:0] cr;
    wire done; wire [15:0] cc;
    b1_real_serial_0608 dut(.clk(clk),.rst(rst),.start(start),.ar(ar),.br(br),.cr(cr),.done(done),.cycle_count(cc));
    integer i,v,hata,r,fd;
    reg [1:0] beklenen [0:1534];
    initial clk=0; always #5 clk=~clk;
    initial begin
        hata=0;
        fd=$fopen("inp_r3_test.txt","r"); ar=0;br=0;
        for(i=0;i<768;i=i+1) begin if(i<761) begin r=$fscanf(fd,"%d",v); ar[2*i+:2]=v[1:0]; end else ar[2*i+:2]=0; end
        for(i=0;i<768;i=i+1) begin if(i<761) begin r=$fscanf(fd,"%d",v); br[2*i+:2]=v[1:0]; end else br[2*i+:2]=0; end
        $fclose(fd);
        fd=$fopen("ham_ref_1535.txt","r");
        for(i=0;i<1535;i=i+1) begin r=$fscanf(fd,"%d",v); beklenen[i]=v[1:0]; end
        $fclose(fd);
        rst=1;start=0;repeat(2)@(negedge clk);rst=0;@(negedge clk);start=1;@(negedge clk);start=0;
        wait(done==1);#1;
        for(i=0;i<1535;i=i+1) if(cr[2*i+:2]!==beklenen[i]) begin
            if(hata<5)$display("HAM FAIL[%0d]: v=%0d bek=%0d",i,cr[2*i+:2],beklenen[i]); hata=hata+1; end
        if(hata==0)$display("=== HAM PASS (1535/1535) - b1_real_serial dogru ===");
        else $display("=== HAM %0d HATA - sorun b1_real_serial'de ===",hata);
        $finish;
    end
    initial begin #50000;$display("TIMEOUT");$finish;end
endmodule
