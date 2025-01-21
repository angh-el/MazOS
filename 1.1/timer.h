void init_timer();
void onIrq0(struct interrupt_register *regs);

void kill_timer();

int getTicks();

void setFreq(uint32_t frequency);