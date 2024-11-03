module Auto_f1  #(
    parameter   WIDTH = 16
)(
    input logic clk,
    input logic rst,
    input logic en_c,
    input logic en_f,
    input logic [WIDTH-1:0] N,
    output logic tick,
    output logic [7:0] data_out
);
    

clktick clktick(
    .clk (clk),
    .rst (rst),
    .en (en_c),
    .N (N),
    .tick (tick)
);

f1_fsm f1_fsm(
    .clk (clk),
    .rst (rst),
    .en (en_f),
    .data_out (data_out)
);

endmodule
