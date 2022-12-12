//===========================================================================
// Senior Design - ACSD
//===========================================================================

#include "stm32f0xx.h"
#include "lcd.h"


//In ascii, 0 = 48,1=49 9 = 57
int player_number = 49;
int round_number1 = 48; // first digit of round number
int round_number2 = 49; // second digit of round number

// Prototypes for miscellaneous things in lcd.c
void nano_wait(unsigned int);

// Set up SPI2 with PB12, 13, 15
void setup_spi2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~GPIO_MODER_MODER12;//alternate functions:10
    GPIOB->MODER &= ~GPIO_MODER_MODER13;
    GPIOB->MODER &= ~GPIO_MODER_MODER15;
    GPIOB->MODER |= GPIO_MODER_MODER12_1;//NSS
    GPIOB->MODER |= GPIO_MODER_MODER13_1;//SCK
    GPIOB->MODER |= GPIO_MODER_MODER15_1;//mosi pin

    SPI2->CR1 |= SPI_CR1_BR;//Set the baud rate as low as possible (maximum divisor for BR).
    SPI2->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_3;//Configure the interface for a 10-bit word size. 1001
    SPI2->CR1 |= SPI_CR1_MSTR;//Configure the SPI channel to be in "master mode".
    SPI2->CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP; // SS Output enable bit and enable NSSP.
    SPI2->CR1 |= SPI_CR1_SPE; //Enable the SPI channel.
}

// send cmd to SPI
void spi_cmd(int val)
{
   while((SPI2->SR & SPI_SR_TXE) == 0);
   *((uint32_t*)&SPI2->DR) = val;
}

// send data to SPI
void spi_data(int val)
{
    while((SPI2->SR & SPI_SR_TXE) == 0);
    *((uint32_t*)&SPI2->DR) = (val| 0x200);
}

// screen initiation
void spi_init_oled(void)
{
    nano_wait(1000000);
    spi_cmd(0x38); // set for 8-bit operation
    spi_cmd(0x08); // turn display off
    spi_cmd(0x01); // clear display
    nano_wait(2000000);
    spi_cmd(0x06); // set the display to scroll
    spi_cmd(0x02); // move the cursor to the home position
    spi_cmd(0x0c); // turn the display on
}

// LCD display via SPI
void spi_display(const char * str1, const char * str2)
{
    // move the cursor to the upper row position
    spi_cmd(0x02);

    // display "player:"
    for(int i=0;str1[i]!='\0';i++)
    {
        spi_data(str1[i]); //for each non-NUL character of the string.
    }
    //display player number
    spi_data(player_number);

    // move the cursor to the lower row (offset 0x40)
    spi_cmd(0xc0);
    //display "round:"
    for(int i=0;str2[i]!='\0';i++)
    {
        spi_data(str2[i]); //for each non-NUL character of the string.
    }

    //display round number
    spi_data(round_number1);
    spi_data(round_number2);

}

// Display function for deal and shuffle buttons
void spi_display2(const char * str1, const char * str2)
{
    // move the cursor to the upper row position
    spi_cmd(0x02);

    // display "player:"
    for(int i=0;str1[i]!='\0';i++)
    {
        spi_data(str1[i]); //for each non-NUL character of the string.
    }

    // move the cursor to the lower row (offset 0x40)
    spi_cmd(0xc0);
    //display "round:"
    for(int i=0;str2[i]!='\0';i++)
    {
        spi_data(str2[i]); //for each non-NUL character of the string.
    }
}


void round(int operator){
    if(operator == 1){ //add
        if(round_number2 == 57){ //9
            round_number2 = 48;
            round_number1 = round_number1 + 1;
        }else{
            round_number2 = round_number2 + 1;
        }
    }else if(operator == 2){ //minus
        if(round_number2 == 49){ //1
            if(round_number1 == 48){
                round_number2 = 49;
                round_number1 = 48;
            }else{
                round_number2 = 48;
            }
        }else if(round_number2 == 48){
            if(round_number1 == 48){
                round_number2 = 49;
            }else{
                round_number2 = 57;
                round_number1 = round_number1 - 1;
            }
        }
        else{
            round_number2 = round_number2 - 1;
        }
    }
}

void player(int operator){
    if(operator == 1){ // add
        if(player_number == 56){ //8 = max player number
            player_number = 56;
        }else{

            player_number = player_number + 1;
        }
    }else if(operator == 2){ // minus
        if(player_number == 49){ //1 = min player number
            player_number = 49;
        }else{
            player_number = player_number - 1;
        }
    }
}

//will be using pc10 - pc13, pa4, and pa5 as 6 buttons
void setup_button(){
    //Activate the RCC clock to GPIO Port C.
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOC->MODER &= ~GPIO_MODER_MODER8; //pc8
    GPIOC->MODER |= GPIO_MODER_MODER8_0;
    GPIOC->ODR |= GPIO_ODR_8; //always high

    // INPUT (00)
    GPIOC->MODER &= ~GPIO_MODER_MODER10;
    GPIOC->MODER &= ~GPIO_MODER_MODER11;
    GPIOC->MODER &= ~GPIO_MODER_MODER12;
    GPIOC->MODER &= ~GPIO_MODER_MODER13;
    GPIOA->MODER &= ~GPIO_MODER_MODER4;
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    // CLEAR
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR10;
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR11;
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR12;
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5;
    // PULL DOWN (10)
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR10_1;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR11_1;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR12_1;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_1;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_1;
    //  PULL UP (01)
//    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR10_0;
//    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR11_0;
//    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR12_0;
//    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_0;
//    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR4_0;
//    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_0;
}

//set up interrupt for receiving button actions
void enable_exti(){
    // SYSCFG & COMP clock enable
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    // PA1 TO PA6
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PC; //PC10
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PC; //PC11
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PC; //PC12
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; //PC13
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PA; //PA4
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PA; //PA5
}

// An EXTI interrupt is generated on the rising edge of each button.
void init_rtsr(){
    EXTI->RTSR |= EXTI_RTSR_TR10;
    EXTI->RTSR |= EXTI_RTSR_TR11;
    EXTI->RTSR |= EXTI_RTSR_TR12;
    EXTI->RTSR |= EXTI_RTSR_TR13;
    EXTI->RTSR |= EXTI_RTSR_TR4;
    EXTI->RTSR |= EXTI_RTSR_TR5;
}

// the EXTI interrupts are unmasked for each button pin.
void init_imr(){
    EXTI->IMR |= EXTI_IMR_MR10;
    EXTI->IMR |= EXTI_IMR_MR11;
    EXTI->IMR |= EXTI_IMR_MR12;
    EXTI->IMR |= EXTI_IMR_MR13;
    EXTI->IMR |= EXTI_IMR_MR4;
    EXTI->IMR |= EXTI_IMR_MR5;
}

//Enable the interrupt for EXTI pins 4-15.
void init_iser(){
    NVIC->ISER[0] |= 0x80;
}

void EXTI4_15_IRQHandler(){
    if (GPIOC->IDR & (0x1<<10)){ // pc10 - add player number
        EXTI->PR = EXTI_PR_PR10;
        player(1);
        spi_display("Player: ", "Round: ");
        nano_wait(300000000);
    }
    else if (GPIOC->IDR & (0x1<<11)){ //pc11 - decrease player number
        EXTI->PR = EXTI_PR_PR11;
        player(2);
        spi_display("Player: ", "Round: ");
        nano_wait(300000000);
        }
    else if (GPIOC->IDR & (0x1<<12)){ //pc12 - add round number
        EXTI->PR = EXTI_PR_PR12;
        round(1);
        spi_display("Player: ", "Round: ");
        nano_wait(300000000);
    }
    else if (GPIOC->IDR & (0x1<<13)){ //pc13 - decrease round number
        EXTI->PR = EXTI_PR_PR13;
        round(2);
        spi_display("Player: ", "Round: ");
        nano_wait(300000000);
    }
    else if (GPIOA->IDR & (0x1<<4)){ //pa4 - start shuffle process
        EXTI->PR = EXTI_PR_PR4;
        spi_display2("Shuffling   ", "Process   ");
        shuffle();
        spi_display("Player: ", "Round: ");
        nano_wait(300000000);
    }
    else if (GPIOA->IDR & (0x1<<5)){ //pa5 - start deal process
        EXTI->PR = EXTI_PR_PR5;
        spi_display2("Dealing   ", "Process   ");

        setup_deal();

            for(int j = 0; j < 2; j++){
                  nano_wait(300000000); //
            }
            int round_number = (round_number1-48) * 10 + (round_number2-48);
            int player = (player_number-48);

            for(int i = 0; i < round_number; i++){
                  for(int j = 0; j < player; j++){
                        deal();
                        if(player == 1)
                        {
                              continue;
                        }
                        for(int k = 0; k < (600/player); k++){
                              GPIOB->ODR |= GPIO_ODR_6;
                              nano_wait(1000000);
                              GPIOB->ODR &= ~GPIO_ODR_6;
                              nano_wait(1000000);
                        }
                  }
            }
            spi_display("Player: ", "Round: ");

        nano_wait(300000000);
    }
}

// Dealing motor function - PB2
void motor(){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER |= GPIO_MODER_MODER2_0;

    while(1){
        GPIOB->ODR |= GPIO_ODR_2;
        nano_wait(100000);
        GPIOB->ODR &= ~GPIO_ODR_2;
        nano_wait(100000);
    }
}

void init_card_motors(){
      RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
      GPIOB->MODER |= GPIO_MODER_MODER6_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER2_0 |GPIO_MODER_MODER0_0;
}

void deal_random(int PinNum){
      int pulse, numPulses;
      numPulses = 38; // # of steps
      pulse = 500000; //pulse on-time necessary for logic high detection (~500ms)

      switch(PinNum){
      case 0:
            {
                  for(int i = 0; i < numPulses; i ++){
                                    GPIOB->ODR |= GPIO_ODR_0;
                                    nano_wait(pulse);
                                    GPIOB->ODR &= ~GPIO_ODR_0;
                  }
                  return;
            }
      case 4:
            {
                  for(int i = 0; i < numPulses; i ++){
                        GPIOB->ODR |= GPIO_ODR_4;
                        nano_wait(pulse);
                        GPIOB->ODR &= ~GPIO_ODR_4;
                  }
                  return;
            }
      case 2:
            {
                  for(int i = 0; i < numPulses; i ++){
                              GPIOB->ODR |= GPIO_ODR_2;
                              nano_wait(pulse);
                              GPIOB->ODR &= ~GPIO_ODR_2;
                  }
                  return;
            }
      case 6:
            {
                  for(int i = 0; i < numPulses; i ++){
                        GPIOB->ODR |= GPIO_ODR_6;
                        nano_wait(pulse);
                        GPIOB->ODR &= ~GPIO_ODR_6;
                  }
                  return;
            }

      }
}


void shuffle(){
      for(int i = 0; i < 20; i++){
            for(int h = 0; h < 5; h++){//5->3
                  deal_random(4);
                  nano_wait(500000); // continuous mode 5->10
            }

            for(int j = 0; j < 1; j++){
                  nano_wait(100000000); // 0.5s delay 75->100
            }


            for(int k = 0; k < 5; k++){//5->3
                  deal_random(0);
                  nano_wait(500000); // continuous mode
            }

            for(int j = 0; j < 1; j++){
                  nano_wait(100000000); // 0.5s delay 75->100
            }
      }
}

void setup_deal(){ //deal one card?
      for(int j = 0; j < 2; j++){
            deal_random(2);
            nano_wait(750000);
      }
}


void deal(){ //deal one card?
      for(int i = 0; i < 5; i++){
                  deal_random(2);
                  nano_wait(700000);
      }
}

int main(void)
{
    setup_button();
    setup_spi2();
    spi_init_oled();

    enable_exti();
    init_rtsr();
    init_imr();
    init_iser();

    spi_display("Player: ", "Round: ");

    init_card_motors();

    for(;;) {
        asm("wfi");
    }
}
