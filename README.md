# Project Subsystem : Microcontroller & Software logic

## Code Logic Explaination

Once the power supply of the project is connected, the program starts. After the program starts, it displays the number of players and rounds on the LCD. Then, it enters a infinite loop that puts the system into sleep mode and wait for interrupts. The 6 expected interrupts are the following:
* player_number_plus_button_pressed
* player_number_minus_button_pressed
* round_number_plus_button_pressed
* round_number_minus_button_pressed
* shuffle_button_pressed
* deal_button_pressed

## Program Explaination

### Shuffling

This code expected the user to use a full deck of 52 cards only. The user should split it into 2 parts and put one part into the left pocket and the other into the right pocket. While suffling, the program makes the left shuffling motor and the right shuffling motor spin in order, so that it drops one card from the left pocket and one card from the right pocket in each round. Just in case the user didn't split the deck of card evenly, the program does 52 round of shuffling so that all 52 cards will be shuffled.

### Setting player's number and round number

The program ensures that there is at least 1 player and 1 round, so that it can deal at least 1 card while dealing. Also, the player has a number limit of 8, and it limit the round number to make sure there is at least 1 card to deal at the last round.

### Dealing

The program spin the base motor in different angles depends on the number of the player. It ensures the base motor spin a 360 degree each round. However, when there is only 1 player, the program won't spin the base motor. The program deals only 1 card when it turns to each specific angle each round.
