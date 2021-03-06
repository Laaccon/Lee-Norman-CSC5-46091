/* 
 * File:   main.cpp
 * Author: Norman Lee
 * Created on July 17, 2015, 3:09 AM
 * Purpose: Heist Dice - a game based off Zombie Dice.
 */

//System Libraries
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;

//Global Constants
const int PLYR = 2;
const int RND = 3;

//Function Prototypes
void title();
char grab();
void scrBrd(int[][RND], int);
char roll(char);
void clear();
int sumttl();
void play(bool &, bool &, unsigned short &, unsigned short &, int [], short &, string, int[][RND], int);
void oplay(bool &, bool &, unsigned short &, unsigned short &, int [], short &, string, int[][RND], int);
void result(unsigned short &, unsigned short &, bool &, bool &);

//Structures
struct FnlTbl
{
    string name;
    int rnd1;
    int rnd2;
    int rnd3;
    int ttl;
};

//Execution begins here
int main(int argc, char** argv) {
    
    //Get seed off time and get "random" numbers
    int seed = time(0);
    srand(seed);
    
    //Declare and Initialize variables
    const int SSIZE = 3;
    int totScor[SSIZE] = {};//Array to keep track of both you and your opponents score
    int table[PLYR][RND] = {};//table for score in each round.
    unsigned short rndPts = 0, strike = 0; //points and strikes accrued this round
    string pTeam, oTeam; //Team Names
    bool pTurn = true, again = true; //Is it the player's turn? / do you want to play again
    short round = 0; //The round number.  Not the Indiana Jones character.
    int tRnd = 3; // Total rounds to be played5
    float cash = 500.00f, bet = 0.0f; // how much cash you have and how much you choose to bet
    
    //Open file
    ifstream inFile; // File that holds the amount of money you have
    inFile.open("bank.txt");
    inFile >> cash;
    inFile.close();
    
    //Output Start Page
    title();
    //Get names of the teams
    cout << "Enter the name of your team:";
    getline(cin, pTeam);
    cout << "Enter the name of your opponents:";
    getline(cin, oTeam);
    cout << "You have $" << cash << ", how much do you want to bet: ";
    cin >> bet;
    cin.ignore();
    if (bet > cash)
    {
        cout << "You shouldn't gamble with more money than you have. Please get help.";
        exit(1);
    }
    //Start the game and loop for the number of rounds 
    for (int i = 1; i <= tRnd; i++)
    {
        //Initialize/Reset conditions for the start of the round
        again = true;
        round++; //Add one to start the round
        //Start your turn
        pTurn = true;
        play(again, pTurn, rndPts, strike, totScor, round, pTeam, table, PLYR);
        //Start opponent's turn
        pTurn = false;
        oplay(again, pTurn, rndPts, strike, totScor, round, oTeam, table, PLYR);
        //End of round phase
        clear();
        scrBrd(table, PLYR);
        cout << "Round " << round << " is over. \n" << pTeam << " has " << totScor[0] << " points.\n" << oTeam << " has " << totScor[1] << " points.\n"; 
        cout << "Press enter to continue\n";
        cin.ignore();
    }
    clear();
    //Structure used to make the table for the final report
    FnlTbl player = 
    {
        pTeam,
        table[0][0],
        table[0][1],
        table[0][2],
        totScor[0]
    };
    cout << setw(16) << left << "Player:" << right << setw(9) << "Round 1" << setw(10) << "Round 2"
            << setw(10) << "Round 3"  << setw(12) << "Total\n";
    cout << "--------------------------------------------------------------------\n";
    cout << setw(15) << left << player.name << right << setw(10) << player.rnd1 << setw(10) << player.rnd2 
            << setw(10) << player.rnd3 << setw(10) << player.ttl << endl;
    FnlTbl comp =
    {
        oTeam,
        table[1][0],
        table[1][1],
        table[1][2],
        totScor[1]
    };
    cout << setw(15) << left << comp.name << right << setw(10) << comp.rnd1 << setw(10) << comp.rnd2 
        << setw(10) << comp.rnd3 << setw(10) << comp.ttl << endl;
    //Determine who won
    if (totScor[0] > totScor[1])
    {
        cout << pTeam << " Wins!";
        cash += bet;
        cout << "You now have $" << cash << ".\n";
    }
    else if (totScor[1] > totScor[0])
    {
        cout << oTeam << " Wins... ";
        cash -= bet;
        cout << "You now have $" << cash << ".\n";
    }
    else
    {
        cout << "It's a draw.";
    }
    //Open file to edit the new amount of money
    ofstream outFile;
    outFile.open("bank.txt");
    outFile << cash;
    outFile.close();
    return 0;
}

//***********************************grab***************************************
// Purpose: Simulate grabbing a dice out of the bag and getting a random color
// Outputs: color -> g = green, r = red, y = yellow
char grab()
{
    char color;
    short val;
    val = rand()% 10 + 1;
    //determine color of dice grabbed
    if (val <= 5) //50% chance for green
    {
        color = 'g';
        cout << "A Green Dice has been pulled out of the bag.\n";
    }
    else if (val >= 9) //20% chance for red
    {
        color = 'r';
        cout << "A Red Dice has been pulled out of the bag.\n";
    }
    else
    {
        color = 'y';//30% chance for yellow
        cout << "A Yellow Dice has been pulled out of the bag.\n";
    }
    return (color);	
}

//***********************************roll****************************************
//Purpose: Roll a dice and get a weighted outcome based on the color
//Input: dice -> the color of dice 
//Output: rslt -> weighted results, green has the highest positive outcome, reds have the lowest
char roll(char dice)
{
    char val, rslt;
    val = rand()%6 + 1;
    if (dice == 'r') //Red dice, weighted with 50% chance of a strike
    {
        if (val <= 3) //%50 chance of a strike
        {
            rslt = 'X'; //you got caught/a strike
        }
        else if (val == 6) //1/6 chance for a point
        {
            rslt = 'L'; //you got loot/a point
        }
        else //1/3 chance to get away
        {
            rslt = 'F'; //you failed to get loot but got away
        }
    }
    else if (dice == 'g') //Green dice, weighted with a 50% chance to get you Loot
    {
        if (val == 1)//1/6 chance of a strike
        {
            rslt = 'X'; //you got caught/a strike
        }
        else if (val >= 4)//1/2 chance of a point
        {
            rslt = 'L'; //you got loot/a point
        }
        else//1/3 chance of getting 
        {
            rslt = 'F'; //you failed to get loot but got away
        }
    }
    else if (dice == 'y') //Yellow dice, weighted with even odds for all outcomes
    {
        if (val <= 2)
        {
            rslt = 'X'; //you got caught/a strike
        }
        else if (val >= 5)
        {
            rslt = 'L'; //you got loot/a point
        }
        else
        {
            rslt = 'F'; //you failed to get loot but got away
        }
    }
    return (rslt);
}

//**************************result***********************************
//Purpose: Add points or strike and output ascii picture
//Inputs:
//	rndPts -> points for this round
//	strike -> number of strikes
//      again  -> play again?
//      pTurn  -> is it the players turn
//Outputs:
//	rndPts
//	strike
void result(unsigned short &rndPts, unsigned short &strike, bool &again, bool &pTurn)
{
    char rslt, color, rolling; // copied values for color and the value you get from rolling
    color = grab(); //grab a dice from the bag and find out which color you got	//Check if user wants to roll
    if (pTurn == true)
    {
    cout << "Do you wish to roll the dice?(y/n)";
        cin >> rolling;
        cin.ignore();
        if (rolling == 'n')
        {
            again = false;
        }
    }
    if (pTurn == false || rolling == 'y')
    {
        rslt = roll(color); //roll the dice and get the value
        clear();
        //Output  representation of the dice and increment strikes or points based on roll result  
        if (rslt == 'L')
        {
            cout << ".-----------.\n"  
                 << "|   $$$$$   |\n" 
                 << "|    $ $    |\n"  
                 << "|  $     $  |\n"  
                 << "| $       $ |\n"  
                 << "|  $$$$$$$  |\n"  
                 << "'-----------'\n";
            cout << "Successfully stole some loot!\n";
            rndPts++;
        }
        else if (rslt == 'X')
        {
            cout << ".-----------.\n" 
                 << "| x       x |\n" 
                 << "|   x   x   |\n" 
                 << "|     x     |\n"  
                 << "|   x   x   |\n"  
                 << "| x       x |\n"  
                 << "'-----------'\n";
            cout << "Got caught and a strike was placed on your record.\n";
            strike++;
        }
        else
        {
            cout << ".-----------.\n"  
                 << "| RRRRR     |\n" 
                 << "| R   R     |\n"  
                 << "| R   RRRR  |\n"  
                 << "| R       R |\n"  
                 << "| RRRRRRRRR |\n"  
                 << "'-----------'\n";
            cout << "Failed at the attempt but got away.\n";
        }
    }
}
//Purpose: Generate a title with information
void title()
{              
    cout << " . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ..-----------. . . . . .\n"
         << ". . . . . $ . .$. .$$$$ . $ .$$$$$. $$$$$$. . . 8888. 8 .888. .8888 . . . |   $$$$$   |. . . . . \n"
         << " . . . . .$. . $ . $ . . .$.$  . .$. .$. . . . .8. .8.8.8. .8. 8 . . . . .|    $ $    | . . . . .\n"
         << ". . . . . $$$$$$. .$$$. . $ .$$$$ . . $ . . . . 8 . 8 8 8 . . .888. . . . |  $     $  |. . . . . \n"
         << " . . . . .$. . $ . $ . . .$.$. . $$. .$. . . . .8. .8.8.8. .8. 8 . . . . .| $       $ | . . . . .\n"
         << ". . . . . $ . .$. .$$$$ . $ .$$$$$. . $ . . . . 8888. 8 .888. .8888 . . . |  $$$$$$$  |. . . . . \n"
         << " . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .'-----------' . . . . .\n"
         << "How to play Heist Dice: Every round you get to roll the dice.\n\n"
         << "There are 3 different outcomes you get from rolling: A loot bag, shoes, and a strike.\n"
         << "--------------------------------------------------------------------------------------------------\n"
         << "Loot bags: you score a point for the round.\n"
         << "Shoes: nothing good or bad happens.\n"
         << "Strikes: If you get 3 strikes, the round ends and you lose any points you made for that round.\n"
         << ".-----------.     .-----------.     .-----------.\n"  
         << "|   $$$$$   |     | RRRRR     |     | x       x |\n" 
         << "|    $ $    |     | R   R     |     |   x   x   |\n"  
         << "|  $     $  |     | R   RRRR  |     |     x     |\n"  
         << "| $       $ |     | R       R |     |   x   x   |\n"  
         << "|  $$$$$$$  |     | RRRRRRRRR |     | x       x |\n"  
         << "'-----------'     '-----------'     '-----------'\n\n"
         << "There are 3 types of die: Green, Red and Yellow.\n"
         << "--------------------------------------------------------------------------------------------------\n"
         << "Green: Has the highest amount of loot bags and only 1 strike.\n"
         << "Red: Has the highest amount of strikes and only 1 loot bag.\n"
         << "Yellow: All outcomes have an equal chance.\n"
         << "--------------------------------------------------------------------------------------------------\n"
         << "After you grab a dice from the bag you can choose to stop and keep the points you have accrued.\n"
         << "There are 5 rounds, you and your opponent take turns rolling for each round.\n";           
    cout << "<Press Enter to continue>\n";
    cin.ignore();
}
          
//Purpose: uses 40 new lines to clear the screen
void clear()
{
    for(int i = 1; i <=40; i++)
    {
    cout << endl;
    }
}
//*******************************play*************************************
//Purpose: Player's turn to play the game
//Inputs: 
// again -> play again, true or false
// rndPts -> points this round
// strike -> strikes this round
// totScor[] -> total scored points
// round -> the round number
// oTeam -> the computer's team name
//Outputs:
// again -> play again?
// rndPts -> points this round
// strike -> strikes this round
// totScor[] -> total scored points 
// oTeam -> the computer's team name
//*************************************************************************
void play(bool &again, bool &pTurn, unsigned short &rndPts, unsigned short &strike, int totScor[], short &round, string pTeam, int table[][RND], int PLYR)
{
    while(again == true)
    {
        cout << "Round " << round << "<Your turn>" << endl;
        result(rndPts, strike, again, pTurn);
        if (again == false)
        {
            totScor[0] += rndPts;
            table[0][(round-1)] += rndPts;
            rndPts = 0;
            strike = 0;
            cout << "Added point(s) to total. " << pTeam << " has " << totScor[0] << " total points.\n\n";
        }
        if (strike >= 3)
        {
            cout << pTeam << " member <" << round <<"> has 3 strikes. Facing life in prison, you give up all your loot for a plea deal.\n\n";
            rndPts = 0;
            again = false;
            strike = 0;
        }
        else
        {
            if (again == true)
            {
            cout << pTeam << " member <" << round << "> has " << rndPts << " points this round and " << strike << " strikes. " << pTeam << " has " << totScor[0] << " total points.\n\n";
            }
        }
        cout << "<Press Enter to continue>\n";
        cin.ignore();
    }
}
//*************************************oplay****************************************
//Purpose: Have the computer opponent play
//Inputs: 
// again -> play again, true or false
// rndPts -> points this round
// strike -> strikes this round
// totScor[] -> total scored points
// round -> the round number
// oTeam -> the computer's team name
//Outputs:
// again -> play again?
// rndPts -> points this round
// strike -> strikes this round
// totScor[] -> total scored points 
// oTeam -> the computer's team name
//**********************************************************************************
void oplay(bool &again, bool &pTurn, unsigned short &rndPts, unsigned short &strike, int totScor[], short &round, string oTeam, int table[][RND], int PLYR)
{
    again = true;
    while (again == true)
    {
        result(rndPts, strike, again, pTurn);
        cout << oTeam << " member " << round <<" has "<< rndPts << " points and " << strike << " strikes.\n" << endl;
        cout << "Round " << round << " <Opponent's turn>" << endl;
        cout << "<Press enter to continue>\n";
        cin.ignore();
        if (strike >= 2)
        {
            //Add round points to total score and reinitialize
            totScor[1] += rndPts;
            table[1][(round-1)] += rndPts;
            rndPts = 0;
            strike = 0;
            cout << oTeam << " chooses to stop. \n" << "Adding point(s) to total. " << totScor[1] << " total points for "<< oTeam <<".\n";
            again = false;
            cout << "<Press enter to continue>\n";
            cin.ignore();
        }
    }
}
//******************************scrBrd****(**************************
//Purpose: Keep tally for each of the rounds
//Inputs:  
//  table[][RND] -> the table to output
//  PLYR -> the player row
//Outputs:
//  table[PLYR][RND] -> a table with the score for each round so far
//*******************************************************************
void scrBrd (int table[][RND], int PLYR)
{
    cout << "Scoreboard:\n";
    for(int p = 0; p < PLYR; p++)
    {
        for(int r = 0; r < RND; r++)
        {
            cout << table[p][r] << " ";
        }
        cout << endl;
    }
}