/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include <stdio.h>

int player_number = 1;
int round_number = 1;

void display_player_number()
{
	// (code) Display the player's number on the LCD
}

void display_round_number()
{
	// (code) Display the round number on the LCD
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
		if(round_number < 8)
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
		// Display the changed number on the LCD
		display_player_number();
		display_round_number();
	}
}

void shuffle_one_left_pocket_card()
{
	// (code) Make the motor spin to drop one card from the left pocket
}

void shuffle_one_right_pocket_card()
{
	// (code) Make the motor spin to drop one card from the right pocket
}

void shuffle_cards()
{
	// Make sure the whole deck of card is shuffled
	int i;
	for(i = 0; i < 52; i++)
	{
		shuffle_one_left_pocket_card();
		shuffle_one_right_pocket_card();
	}
}

void deal_one_card()
{
	// (code) Make the dealer motor spin to deal one card
}

void base_motor_spin(int spin_angle)
{
	// No need to spin if the spin_angle is 0
	if(spin_angle == 0)
	{
		return;
	}
	// (code) Spin the base motor for a certain angle
}

void deal_cards()
{
	int i, j;
	int spin_angle, remain_angle;
	spin_angle = 360 / player_number;
	remain_angle = 360 % player_number;
	for(i = 0; i < round_number; i++)
	{
		if(player_number == 1)
		{
			// No need to spin the base motor if there is only 1 player
			deal_one_card();
		}
		else
		{
			for(j = 0; j < player_number; j++)
			{
				base_motor_spin(spin_angle);
				deal_one_card();
			}
			// Make sure the base motor spin a fully 360 degree every round
			base_motor_spin(remain_angle);
		}
	}
}

int main(void)
{
	int player_number_plus_button_pressed = 0;
	int player_number_minus_button_pressed = 0;
	int round_number_plus_button_pressed = 0;
	int round_number_minus_button_pressed = 0;
	int shuffle_button = 0;
	int deal_button = 0;
	// Display the player's number and the round number on the LCD
	display_player_number();
	display_round_number();
	while(1)
	{
		// When buttons for changing player's number is pressed
		if(player_number_plus_button_pressed)
		{
			player_number_plus_button_pressed = 0;
			set_number(1, 0);
		}
		else if(player_number_minus_button_pressed)
		{
			player_number_minus_button_pressed = 0;
			set_number(-1, 0);
		}
		// When buttons for changing round number is pressed
		if(round_number_plus_button_pressed)
		{
			round_number_plus_button_pressed = 0;
			set_number(0, 1);
		}
		else if(round_number_minus_button_pressed)
		{
			round_number_minus_button_pressed = 0;
			set_number(0, -1);
		}
		// When shuffle button is pressed
		if(shuffle_button)
		{
			shuffle_button = 0;
			shuffle_cards();
		}
		// When deal button is pressed
		if(deal_button)
		{
			deal_button = 0;
			deal_cards();
		}
	}
}
