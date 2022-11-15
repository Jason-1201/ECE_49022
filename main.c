//===========================================================================
// Senior Design - ACSD
//===========================================================================

#include "stm32f0xx.h"
#include "lcd.h"

int player_number = 1;
int round_number = 1;
int round_number_1 = 0; // first digit of round number
int round_number_2 = 1; // second digit of round number

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

int num_ascii(int num)
{
	//In ascii, '0' => 48, '9' => 57
    return num + 48;
}

// LCD display via SPI
void spi_display()
{
	const char str1[] = "Player: ";
	const char str2[] = "Round: ";
    // move the cursor to the upper row position
    spi_cmd(0x02);

    // display "player:"
    for(int i=0;str1[i]!='\0';i++)
    {
        spi_data(str1[i]); //for each non-NUL character of the string.
    }
    //display player number
    spi_data(num_ascii(player_number));

    // move the cursor to the lower row (offset 0x40)
    spi_cmd(0xc0);
    //display "round:"
    for(int i=0;str2[i]!='\0';i++)
    {
        spi_data(str2[i]); //for each non-NUL character of the string.
    }

    //display round number
    spi_data(num_ascii(round_number_1));
    spi_data(num_ascii(round_number_2));

}


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

void set_number(int change_player_number, int change_round_number)
{
	// change_player_number = 0/1/-1 -> player_number+0 / player_number+1 / player_number-1
	// change_round_number = 0/1/-1 -> round_number+0 / round_number+1 / round_number-1
	int number_changed = 0;
	// Check if the player's number is changed
	// 1 <= player_number <= 8
	if(change_player_number == 1)
	{
		if(player_number < 8)
		{
			player_number++;
			number_changed = 1;
		}
	}
	else if(change_player_number == -1)
	{
		if(player_number > 1)
		{
			player_number--;
			number_changed = 1;
		}
	}
	// Check if the round number is changed
	// 1 <= round_number
	if(change_round_number == 1)
	{
		if(round_number < 52)
		{
			round_number++;
			number_changed = 1;
		}
	}
	else if(change_round_number == -1)
	{
		if(round_number > 1)
		{
			round_number--;
			number_changed = 1;
		}
	}
	int max_round_number;
	// If number is changed, make sure every round deal at least one card.
	if(number_changed)
	{
		max_round_number = 52 / player_number;
		if(52 % player_number)
		{
			max_round_number++;
		}
		if(round_number > max_round_number)
		{
			round_number = max_round_number;
		}
		round_number_1 = round_number / 10;
		round_number_2 = round_number % 10;
		// Display the changed number on the LCD
		spi_display();
	}
}

//will be using pc10 through pc15 as 6 buttons
void setup_button(){
    //Activate the RCC clock to GPIO Port C.
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
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
}

//set up interrupt for receiving button actions
void enable_exti(){
    // SYSCFG & COMP clock enable
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    // PA1 TO PA6
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PC; //PA6
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PC; //PA4
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PC; //PA5
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; //PA6
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PA; //PA4
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PA; //PA5
}

// An EXTI interrupt is generated on the rising edge of PA1.
void init_rtsr(){
    EXTI->RTSR |= EXTI_RTSR_TR10;
    EXTI->RTSR |= EXTI_RTSR_TR11;
    EXTI->RTSR |= EXTI_RTSR_TR12;
    EXTI->RTSR |= EXTI_RTSR_TR13;
    EXTI->RTSR |= EXTI_RTSR_TR4;
    EXTI->RTSR |= EXTI_RTSR_TR5;
}

// the EXTI interrupts are unmasked for pins 1-6.
void init_imr(){
    EXTI->IMR |= EXTI_IMR_MR10;
    EXTI->IMR |= EXTI_IMR_MR11;
    EXTI->IMR |= EXTI_IMR_MR12;
    EXTI->IMR |= EXTI_IMR_MR13;
    EXTI->IMR |= EXTI_IMR_MR4;
    EXTI->IMR |= EXTI_IMR_MR5;
}

//Enable the two interrupts for EXTI pins 0-1, 2-3 and EXTI pins 4-15.
void init_iser(){
    NVIC->ISER[0] |= 0x80;
}
//
//// Dealing motor function - PB2
//void motor(){
//    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
//    GPIOB->MODER |= GPIO_MODER_MODER2_0;
//
//    while(1){
//        GPIOB->ODR |= GPIO_ODR_2;
//        nano_wait(100000);
//        GPIOB->ODR &= ~GPIO_ODR_2;
//        nano_wait(100000);
//    }
//}

void EXTI4_15_IRQHandler(){
	if (GPIOC->IDR & (0x1<<10)){ // pc8 - add player number
		EXTI->PR = EXTI_PR_PR10;
		set_number(1, 0);
		nano_wait(300000000);
	}
	else if (GPIOC->IDR & (0x1<<11)){ //pc9 - decrease player number
		EXTI->PR = EXTI_PR_PR11;
		set_number(-1, 0);
		nano_wait(300000000);
	}
	else if (GPIOC->IDR & (0x1<<12)){ //pc10 - add round number
		EXTI->PR = EXTI_PR_PR12;
		set_number(0, 1);
		nano_wait(300000000);
	}
	else if (GPIOC->IDR & (0x1<<13)){ //pc11 - decrease round number
        EXTI->PR = EXTI_PR_PR13;
		set_number(0, -1);
        nano_wait(300000000);
    }
    else if (GPIOA->IDR & (0x1<<4)){ //pc12 - start shuffle process
        EXTI->PR = EXTI_PR_PR4;
        // add shuffle button function
        spi_display2("Shuffling   ", "Process  ");
        nano_wait(300000000);
    }
    else if (GPIOA->IDR & (0x1<<5)){ //pc13 - start deal process
        EXTI->PR = EXTI_PR_PR5;
        // add deal button function
        spi_display2("Dealing   ", "Process  ");
        nano_wait(300000000);
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

    spi_display();

    for(;;) {
        asm("wfi");
    }
}
