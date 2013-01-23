#include <string>
#include <cmath>
#include <stdio.h>

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>
#include <cons/vdp2.h>

using namespace std;

#define BUTTON_UP       1
#define BUTTON_DOWN     2
#define BUTTON_LEFT     3
#define BUTTON_RIGHT    4
#define BUTTON_A        5
#define BUTTON_Z        6

class TicTacToe
{
    public:
        TicTacToe();                // constructor
        int Pick_Player();          // member functions
        int Pick_Row();
        int Pick_Column();
        int Check_Board();
        void Clear_Board();
        void Choice_by_Player(int);
        void Choice_of_Row(int);
        void Choice_of_Column(int);
        void Tic_Tac_Toe_Board();
        bool Check_Move(int,int);
        void pos_crsr(int,int);
        void print_str(char*);
        void delay(int);
        bool is_button_pressed(int);
    private:
        int row;
        int column;
        int player;
        int board[3][3];
        char display_board[3][3];
        struct cons cons;
        struct smpc_peripheral_digital *digital;
};

TicTacToe::TicTacToe()
{
    cons_vdp2_init(&cons);
    cons_write(&cons, "[H[m");

    delay(2);
    digital = smpc_peripheral_digital_port(1);
    delay(2);

    row = 0;
    column = 0;
    player = 1;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = 0;
            display_board[i][j] = ' ';
        }
    }
}

int TicTacToe::Pick_Player()
{
    return player;
}

int TicTacToe::Pick_Row()
{
    return row;
}

int TicTacToe::Pick_Column()
{
    return column;
}

void TicTacToe::Choice_by_Player(int a)
{
    player = a;
}

void TicTacToe::Choice_of_Row(int b)
{
    row = b;
}

void TicTacToe::Choice_of_Column(int c)
{
    column = c;
}

bool TicTacToe::Check_Move(int row, int column)
{
    if ( board[row][column] != 0 )
    {
        pos_crsr(20-13, 3);
        print_str((char*)"Space occupied - Try Again");
        delay(120);
        pos_crsr(20-13, 3);
        print_str((char*)"                          ");
        return 0;
    }
    else
    {
        board[row][column] = player;
        return 1;
    }
}   // end of Check_Move

int TicTacToe::Check_Board()
{
    int i = 0;

    // check for tie
    for (i = 0; i < 9; i++)
    {
        if (board[i/3][i%3] == 0)
            break;
    } // end of for loop
    if ( i == 9 )
        return 3; // tie game!

    // check rows
    if (( (board[0][0] == player) && (board[0][1] == player) && (board[0][2] == player) ) ||
        ( (board[1][0] == player) && (board[1][1] == player) && (board[1][2] == player) ) ||
        ( (board[2][0] == player) && (board[2][1] == player) && (board[2][2] == player) ))
        return player;

    // check columns
    if (( (board[0][0] == player) && (board[1][0] == player) && (board[2][0] == player) ) ||
        ( (board[0][1] == player) && (board[1][1] == player) && (board[2][1] == player) ) ||
        ( (board[0][2] == player) && (board[1][2] == player) && (board[2][2] == player) ))
        return player;

    // check diagonals
    if (( (board[0][0] == player) && (board[1][1] == player) && (board[2][2] == player) ) ||
        ( (board[0][2] == player) && (board[1][1] == player) && (board[2][0] == player) ))
        return player;

    return 0;
} // end of Check_Board

void TicTacToe::Clear_Board()
{
    row = 0;
    column = 0;
    player = 1;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = 0;
            display_board[i][j] = ' ';
        }
    }
}

void TicTacToe::Tic_Tac_Toe_Board()
{
    char temp[4];

    pos_crsr(20-8, 21);
    if ( player == 1 )
        print_str((char*)"Current Player: X");
    else
        print_str((char*)"Current Player: O");

    pos_crsr(20-6, 6);
    print_str((char*)"   |   |   ");
    pos_crsr(20-6, 7);
    print_str((char*)"   |   |   ");
    pos_crsr(20-6, 8);
    print_str((char*)"___|___|___");
    pos_crsr(20-6, 9);
    print_str((char*)"   |   |   ");
    pos_crsr(20-6, 10);
    print_str((char*)"   |   |   ");
    pos_crsr(20-6, 11);
    print_str((char*)"___|___|___");
    pos_crsr(20-6, 12);
    print_str((char*)"   |   |   ");
    pos_crsr(20-6, 13);
    print_str((char*)"   |   |   ");
    pos_crsr(20-6, 14);
    print_str((char*)"   |   |   ");

    temp[1] = 0;
    for ( int row = 0; row < 3; row ++)
    {
        for ( int column = 0; column < 3; column++)
        {
            if ( board[row][column] == 0)
            {
                display_board[row][column] = ' ';
            }
            if ( board[row][column] == 1)
            {
                display_board[row][column] = 'X';
            }
            if ( board[row][column] == 2)
            {
                display_board[row][column] = 'O';
            }
            pos_crsr(20-6+1 + column*4, 6+1 + row*3);
            temp[0] = display_board[row][column];
            print_str(temp);
        }
    }

}   // end of Tic_Tac_Toe_Board

void TicTacToe::pos_crsr(int x,int y)
{
    char temp[32];
    sprintf(temp, "[%d;%dH", y, x);
    cons_write(&cons, temp);
}

void TicTacToe::print_str(char *str)
{
    cons_write(&cons, str);
}

void TicTacToe::delay(int count)
{
    while ( count > 0 )
    {
        vdp2_tvmd_vblank_in_wait();
        vdp2_tvmd_vblank_out_wait();
        count--;
    }
}

bool TicTacToe::is_button_pressed(int button)
{
    switch(button)
    {
        case BUTTON_UP:
            return !digital->button.up ? true : false;
        case BUTTON_DOWN:
            return !digital->button.down ? true : false;
        case BUTTON_LEFT:
            return !digital->button.left ? true : false;
        case BUTTON_RIGHT:
            return !digital->button.right ? true : false;
        case BUTTON_A:
            return !digital->button.a_trg ? true : false;
        case BUTTON_Z:
            return !digital->button.z_trg ? true : false;
    }
    return false;
}

int main()
{
    TicTacToe game;
    bool test;
    bool more = true;
    int row = 0;
    int column= 0;
    int player;
    int check = 0;

    uint16_t blcs_color[] = {
            0x9C00
    };

    vdp2_init();
    vdp2_tvmd_blcs_set(false, VRAM_ADDR_4MBIT(3, 0x1FFFE), blcs_color, 0);

    smpc_init();

    TicTacToe();

    while ( more )
    {
        game.Tic_Tac_Toe_Board();
        player = game.Pick_Player();

        game.pos_crsr(20-8, 3);
        game.print_str((char*)"Choose a square");
        while ( !game.is_button_pressed(BUTTON_A) )
        {
            game.delay(2);
            if ( game.is_button_pressed(BUTTON_Z) )
            {
                more = 0;
                break;
            }
        }
        while ( game.is_button_pressed(BUTTON_A) )
            game.delay(2);
        game.pos_crsr(20-8, 3);
        game.print_str((char*)"               ");
        game.delay(30);

        if ( !more )
            break;

        row = 1;
        if ( game.is_button_pressed(BUTTON_UP) )
            row = 0;
        else if ( game.is_button_pressed(BUTTON_DOWN) )
            row = 2;
        column = 1;
        if ( game.is_button_pressed(BUTTON_LEFT) )
            column = 0;
        else if ( game.is_button_pressed(BUTTON_RIGHT) )
            column = 2;

        game.Choice_of_Row(row);
        game.Choice_of_Column(column);

        test = game.Check_Move( game.Pick_Row(), game.Pick_Column());
        if ( test == 0 )
        {
            // invalid move
            continue;
        }
        game.Tic_Tac_Toe_Board();

        check = game.Check_Board();
        if ( (check == 1) || (check == 2) )
        {
            game.pos_crsr(20-4, 3);
            if ( check == 1 )
                game.print_str((char*)"X wins!");
            else
                game.print_str((char*)"O wins!");
            game.delay(240);
            game.pos_crsr(20-4, 3);
            game.print_str((char*)"        ");
            // reset the board
            game.Clear_Board();
        }
        else if ( check == 3 )
        {
            game.pos_crsr(20-8, 3);
            game.print_str((char*)"The game is tied");
            game.delay(240);
            game.pos_crsr(20-8, 3);
            game.print_str((char*)"                ");
            // reset the board
            game.Clear_Board();
        }

        if ( player == 1)
        {
            player = 2;
        }
        else
        {
            player = 1;
        }
        game.Choice_by_Player(player);

    } // end of outer while loop

    return 0;
} // end of main function

