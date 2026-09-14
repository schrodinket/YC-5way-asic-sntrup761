// r3_u1_bridge: emulates the Peng decap R/3 interface using the U1 multiplier
// Peng F_3 encoding: signed 2-bit {00=0, 01=+1, 11=-1}
// U1   F_3 encoding: 2-bit       {00=0, 01=+1, 10=-1}
// Remap: only -1 differs (Peng 11 <-> U1 10)

module r3_u1_bridge #(
    parameter P = 761,
    parameter W = 1522   // 2*P
)(
    input  clk,
    input  rst,
    input  start,                 // one cycle at start of mult_R3
    input  [W-1:0] e_peng,        // captured e (Peng encoding, 2 bits per coefficient)
    input  [W-1:0] ginv_peng,     // captured g^-1 (Peng encoding)
    output reg done,
    output reg output_valid,      // for calc_weight and bram_e
    output reg [1:0] r3_output,   // result in Peng encoding, one coefficient per cycle
    output [W-1:0] result_wide,   // full 2*p-bit result
    output result_ready           // result_wide valid
);
    // Peng -> U1 input remap: 11->10, others unchanged
    
    function [1:0] p2u; input [1:0] x; begin
        p2u = (x==2'b11) ? 2'b10 : x;
    end endfunction
    // U1 -> Peng output remap: 10->11, others unchanged
    function [1:0] u2p; input [1:0] x; begin
        u2p = (x==2'b10) ? 2'b11 : x;
    end endfunction

    // convert inputs to U1 encoding (combinational)
    wire [W-1:0] e_u1, ginv_u1;
    genvar i;
    generate
        for(i=0;i<P;i=i+1) begin: remap_in
            assign e_u1[2*i+:2]    = p2u(e_peng[2*i+:2]);
            assign ginv_u1[2*i+:2] = p2u(ginv_peng[2*i+:2]);
        end
    endgenerate

    // U1 multiplier
    reg u1_start;
    reg u1_rst_pulse;
    reg [2:0] rst_cnt;
    wire [W-1:0] u1_cr;
    wire u1_done;
    wire [15:0] u1_cc;
    u1_r3_1engine u1(
        .clk(clk), .rst(rst | u1_rst_pulse), .start(u1_start),
        .ar(e_u1), .br(ginv_u1),
        .cr(u1_cr), .done(u1_done), .cycle_count(u1_cc)
    );

    // convert U1 result to Peng encoding
    wire [W-1:0] cr_peng;
    generate
        for(i=0;i<P;i=i+1) begin: remap_out
            assign cr_peng[2*i+:2] = u2p(u1_cr[2*i+:2]);
        end
    endgenerate

    // FSM: start -> run U1 -> stream result
    localparam S_IDLE=3'd0, S_MULT=3'd1, S_STREAM=3'd2, S_DONE=3'd3, S_RESET=3'd4, S_STARTGAP=3'd5;
    reg [2:0] st;
    reg [10:0] idx;   
    reg [W-1:0] result_lat;  // latched U1 result
    reg result_valid_r;
    assign result_wide  = result_lat;
    assign result_ready = result_valid_r;

    // output_valid and r3_output are combinational, indexed by idx
    
    always @(*) begin
        if (st==S_STREAM) begin
            output_valid = 1'b1;
            r3_output    = result_lat[2*idx+:2];
        end else begin
            output_valid = 1'b0;
            r3_output    = 2'b00;
        end
    end

    always @(posedge clk) begin
        if (rst) begin
            st<=S_IDLE; u1_start<=1'b0; done<=1'b0; idx<=11'd0; result_valid_r<=1'b0;
            u1_rst_pulse<=1'b0; rst_cnt<=3'd0;
        end else begin
            case(st)
                S_IDLE: begin
                    done<=1'b0; idx<=11'd0; result_valid_r<=1'b0;
                    if(start) begin
                        u1_rst_pulse<=1'b1;  // reset U1 to clear the pipeline
                        rst_cnt<=3'd4;       // 4 cycle reset
                        st<=S_RESET;
                    end
                end
                S_RESET: begin
                    if(rst_cnt==3'd0) begin
                        u1_rst_pulse<=1'b0;
                        // u1_start asserted in S_STARTGAP
                        st<=S_STARTGAP;
                    end else begin
                        rst_cnt<=rst_cnt-3'd1;
                    end
                end
                S_STARTGAP: begin
                    // reset deasserted last cycle; assert start now
                    u1_start<=1'b1;
                    st<=S_MULT;
                end
                S_MULT: begin
                    u1_start<=1'b0;
                    if(u1_done) begin
                        result_lat<=cr_peng;  // latch result
                        result_valid_r<=1'b1; // result ready
                        st<=S_STREAM; idx<=11'd0;
                    end
                end
                S_STREAM: begin
                    // one coefficient per cycle
                    if(idx==P-1) st<=S_DONE;
                    else idx<=idx+1'b1;
                end
                S_DONE: begin
                    done<=1'b1;
                    if(!start) st<=S_IDLE;
                end
            endcase
        end
    end
endmodule
