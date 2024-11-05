#include "verilated.h"
#include "verilated_vcd_c.h"
#include "VAuto_f1.h"   // Ensures math constants (FP_ZERO, FP_NAN, etc.) are declared

#include "vbuddy.cpp"     // include vbuddy code

#define MAX_SIM_CYC 1000000
#define ADDRESS_WIDTH 9
#define RAM_SZ pow(2,ADDRESS_WIDTH)

int main(int argc, char **argv, char **env) {
  int simcyc;     // simulation clock count
  int tick;       // each clk cycle has two ticks for two edges

  Verilated::commandArgs(argc, argv);
  // init top verilog instance
  VAuto_f1* top = new VAuto_f1;

  // init trace dump
  Verilated::traceEverOn(true);
  VerilatedVcdC* tfp = new VerilatedVcdC;
  top->trace (tfp, 99);
  tfp->open ("Auto_f1.vcd");
 
  // init Vbuddy
  if (vbdOpen()!=1) return(-1);
  vbdHeader("L3T3:Auto_f1");       

  // initialize simulation input 
  top->clk = 1;
  top->rst = 1;
  top->en_c = 1;
  top->en_f = 1;
  top->N = 29;
 
  // run simulation for MAX_SIM_CYC clock cycles
  for (simcyc=0; simcyc<10000; simcyc++) {
    // dump variables into VCD file and toggle clock
    for (tick=0; tick<2; tick++) {
      tfp->dump (2*simcyc+tick);
      top->clk = !top->clk;
      top->eval ();
    }
    if (simcyc >= 2)
        top->rst = 0;

    top->en_f = top->tick;
    vbdBar(top->data_out & 0xFF);
    vbdCycle(simcyc);

    // either simulation finished, or 'q' is pressed
    if ((Verilated::gotFinish()) || (vbdGetkey()=='q')) 
      exit(0);
  }

  vbdClose();     // ++++
  tfp->close(); 
  printf("Exiting\n");
  exit(0);
}
