# Project Subsystem : Microcontroller & Software logic

## Code Logic Explaination

Once the power supply of the project is connected, the program starts. Once the program starts, it displays the number of players and rounds on the LCD. Then, it enters a infinite loop, which every loop checks the value of 6 variables: player_number_plus_button_pressed, player_number_minus_button_pressed, round_number_plus_button_pressed, round_number_minus_button_pressed, shuffle_button, and deal_button. Each variable will be changed by the corresponded button on the UI(User Interface). Once the corresponded button is pressed, the variable will be changed to 1 and activate the corresponded function. Note that the variable will be changed back to 0 after the function is activated, so that it won't be activating the same function infinite times and allow the user to activate the function again by pressing the button.
