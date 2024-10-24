#include "gtest/gtest.h"
#include "Vdut.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <memory>
#include <cmath>

class TestDut : public ::testing::Test
{
public:
    void SetUp() override
    {
        top = std::make_unique<Vdut>();
        tfp = std::make_unique<VerilatedVcdC>();

        Verilated::traceEverOn(true);
        top->trace(tfp.get(), 99);
        tfp->open("waveform.vcd");

        initializeInputs();
    }

    void TearDown() override
    {
        top->final();
        tfp->close();
    }

    void initializeInputs()
    {
        top->clk = 0;
        top->rst = 0;
        top->en = 1;
    }

    // Runs the simulation for a clock cycle, evaluates the DUT, dumps waveform.
    void runSimulation()
    {
        for (int clk = 0; clk < 2; clk++)
        {
            top->eval();
            tfp->dump(2 * ticks + clk);
            top->clk = !top->clk;
        }
        ticks++;

        if (Verilated::gotFinish())
        {
            exit(0);
        }
    }

protected:
    unsigned int ticks = 0;
    std::unique_ptr<Vdut> top;
    std::unique_ptr<VerilatedVcdC> tfp;
};

TEST_F(TestDut, InitialStateTest)
{
    top->rst = 1;
    runSimulation();
    EXPECT_EQ(top->data_out, 0b0001);
}

/**
 * Here's to anyone who actually reads this code.
 * The answer to the LSFR7 lies here. You're welcome.
 * Thank you for taking your time to understand good testing.
 */
int generateLFSR7(int state)
{
    int b0 = (state >> 0) & 1;
    int b1 = (state >> 1) & 1;
    int b2 = (state >> 2) & 1;
    int b3 = (state >> 3) & 1;
    int b4 = (state >> 4) & 1;
    int b5 = (state >> 5) & 1;
    int b6 = (state >> 6) & 1;

    return (b5 << 6 | b4 << 5 | b3 << 4 | b2 << 3 | b1 << 2 | b0 << 1 |
            (b2 ^ b6));
}

TEST_F(TestDut, SequenceTest)
{
    top->rst = 1;
    runSimulation();
    top->rst = 0;
    int exp = 0b000'0001;

    for (int i = 0; i < pow(2, 7); i++)
    {
        exp = generateLFSR7(exp);
        runSimulation();
        EXPECT_EQ(top->data_out, exp);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    return res;
}