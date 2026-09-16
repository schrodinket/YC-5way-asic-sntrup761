`timescale 1ns/1ps
// r3_u1_bridge dogrulama: rastgele e,ginv (Peng kodlamali) ver
// kopru sonucu = bagimsiz hesaplanan R/3 carpimi (Peng kodlamali) olmali
module tb_bridge;
    localparam P=761, W=1522;
    reg clk=0; always #5 clk=~clk;
    reg rst=1, start=0;
    reg [W-1:0] e_peng, ginv_peng;
    wire done, ov; wire [1:0] r3o;
    r3_u1_bridge dut(.clk(clk),.rst(rst),.start(start),
        .e_peng(e_peng),.ginv_peng(ginv_peng),
        .done(done),.output_valid(ov),.r3_output(r3o));

    // Peng kodlama <-> deger
    function integer pv; input [1:0] b; begin
        pv = (b==2'b00)?0 : (b==2'b01)?1 : (b==2'b11)?-1 : 99; end
    endfunction
    function [1:0] pb; input integer v; begin
        pb = (v==0)?2'b00 : (v==1)?2'b01 : 2'b11; end
    endfunction

    integer ea[0:P-1], ga[0:P-1];   // deger dizileri
    integer ref_c[0:P-1];           // referans R/3 sonucu (deger)
    integer i,j,k,acc,collected;
    reg [1:0] got[0:P-1];

    initial begin
        // rastgele {-1,0,1} uret + Peng kodla
        for(i=0;i<P;i=i+1) begin
            ea[i] = ($unsigned($random)%3); if(ea[i]==2) ea[i]=-1;
            ga[i] = ($unsigned($random)%3); if(ga[i]==2) ga[i]=-1;
            e_peng[2*i+:2]=pb(ea[i]);
            ginv_peng[2*i+:2]=pb(ga[i]);
        end
        // referans: R/3 carpim mod (x^761 - x - 1), katsayilar mod 3 {-1,0,1}
        // once duz konvolusyon (uzunluk 2P-1), sonra x^P=x+1 indirgeme
        begin: refcalc
            integer conv[0:2*P-2];
            for(k=0;k<2*P-1;k=k+1) conv[k]=0;
            for(i=0;i<P;i=i+1)
                for(j=0;j<P;j=j+1)
                    conv[i+j] = conv[i+j] + ea[i]*ga[j];
            // indirgeme: x^P = x+1, yani x^(P+m) = x^(m+1)+x^m
            for(k=2*P-2;k>=P;k=k-1) begin
                conv[k-P+1] = conv[k-P+1] + conv[k];
                conv[k-P]   = conv[k-P]   + conv[k];
                conv[k]=0;
            end
            for(k=0;k<P;k=k+1) begin
                ref_c[k] = ((conv[k]%3)+3)%3;
                if(ref_c[k]==2) ref_c[k]=-1;
            end
        end
        // kopruyu calistir
        rst=1; start=0; repeat(3) @(negedge clk); rst=0; @(negedge clk);
        start=1; collected=0;
        // ov geldikce topla
        while(collected<P) begin
            @(posedge clk); #1;
            if(ov) begin got[collected]=r3o; collected=collected+1; end
            if(collected==1) start=0;
        end
        // karsilastir
        begin: cmp
            integer err; err=0;
            for(k=0;k<P;k=k+1) begin
                if(pv(got[k]) != ref_c[k]) begin
                    err=err+1;
                    if(err<=5) $display("FARK k=%0d: kopru=%0d ref=%0d",k,pv(got[k]),ref_c[k]);
                end
            end
            $display("KOPRU DOGRULAMA: %0d/%0d hata -> %s",err,P,(err==0)?"PASS":"FAIL");
        end
        $finish;
    end
endmodule
