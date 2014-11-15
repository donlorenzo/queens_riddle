#include "allegro.h"
#include <vector>
#include <cmath>

using namespace std;

#define wait_for_key { clear_keybuf(); while(! keypressed() ); }

const char*  MAX_POS = "16777216";

// timing
volatile long int g_timer;
long int old_timer;

void ttimer(  void )
{
   g_timer++;
}
END_OF_FUNCTION(ttimer);

typedef struct solution
{
   int pos[8];
} solution;

// GLOBALS
// Variable declaration
vector <int>      figure;
vector <solution> solutions;
BITMAP            *buffer;
solution          example0, example1, example2;
int               xoffs;
bool              debug;
int               delay;
long unsigned int cnt = 0;

// Function declaration
void              check_user(       void );
void              show_menu(       void );
void              start_solutions( void );
void              start_debug(     void );
bool              search(          int row );
bool              check(           void );
bool              check_prev(      int row );
void              show(            void );
void              play(            void );
void              draw(            void );
void              example(         void );



//functions
void play( void )
{
   int select = 1;
   bool Esc = false;
   int input;
   for( int i = 1; i <= 8; i++ )
      figure[i] = 1;

   do
   {
      draw();
      textprintf_ex( screen, font, 2,   5,  1, -1, "PLAY-MODE:" );
      textprintf_ex( screen, font, 1,  10,  1, -1, "----------" );
      textprintf_ex( screen, font, 2,  80,  1, -1, "Usage:" );
      textprintf_ex( screen, font, 2,  88,  1, -1, " RIGHT & LEFT" );// to select a queen use RIGHT and LEFT (curser)." );
      textprintf_ex( screen, font, 2,  96,  1, -1, " UP & DOWN" );
      textprintf_ex( screen, font, 2, 104,  1, -1, " ENTER = check" );

      textprintf_ex( screen, font, 2, 184, 12, -1, "Press ESC to" );
      textprintf_ex( screen, font, 1, 192, 12, -1, " return to menu" );
   
      circlefill( screen, SCREEN_H/8 * select + xoffs       + SCREEN_H/16,
                          SCREEN_H/8 * (figure[select] - 1) + SCREEN_H/16, 5, 2 );
      clear_keybuf();
      input = 0;
      while( input == 0 )
      {
         input = readkey() >> 8;
         if( ( input == KEY_LEFT ) && ( select > 1 ) )
            select--;
         else if( ( input == KEY_RIGHT ) && ( select < 8 ) )
            select++;
         else if( ( input == KEY_UP ) && ( figure[select] > 1 ) )
            figure[select]--;
         else if( ( input == KEY_DOWN ) && ( figure[select] < 8 ) )
            figure[select]++;
         else if( ( input == KEY_SPACE ) || ( input == KEY_ENTER ) )
            check_user();
         else if( input == KEY_ESC )
            Esc = true;
      }
   }while( !Esc );

   return;
}


void check_user( void )
{
   int errors = 0;
   bool exists = false;
   clear( screen );
   draw();

   for( int j = 1; j < 8; j++ )
   {
      for( int i = j + 1; i <= 8; i++ )
      {
         if( ( figure[j] == figure[i] ) ||
             ( figure[j] == figure[i] - ( j - i ) ) ||
             ( figure[j] == figure[i] + ( j - i ) ) ||
             ( figure[j] < 1 ) )
         {
            line( screen, SCREEN_H/8 * j + xoffs         + SCREEN_H/16,
                          SCREEN_H/8 * ( figure[j] - 1 ) + SCREEN_H/16,
                          SCREEN_H/8 * i + xoffs         + SCREEN_H/16,
                          SCREEN_H/8 * ( figure[i] - 1 ) + SCREEN_H/16, 4);
            errors++;
         }
      }
   }
   if( errors )
   {
      textprintf_ex( screen, font, 2, 40, 4, -1, "NOT a solution!" );
      textprintf_ex( screen, font, 2, 48, 4, -1, "%d mistakes", errors );
   }else
   {
      textprintf_ex( screen, font, 2, 40, 2, -1, "BINGO!!!" );
      textprintf_ex( screen, font, 2, 48, 2, -1, "0 mistakes" );

      for( int i = 0; i < solutions.size(); i++ )
      {
         if( ( solutions[i].pos[0] == figure[1] ) &&
             ( solutions[i].pos[1] == figure[2] ) &&
             ( solutions[i].pos[2] == figure[3] ) &&
             ( solutions[i].pos[3] == figure[4] ) &&
             ( solutions[i].pos[4] == figure[5] ) &&
             ( solutions[i].pos[5] == figure[6] ) &&
             ( solutions[i].pos[6] == figure[7] ) &&
             ( solutions[i].pos[7] == figure[8] ) )
         {
            textprintf_ex( screen, font, 2, 56, 2, -1, "solution # %d", i + 1 );
            exists = true;
            break;
         }
      }
      if( !exists )
      {
         solution temp;
         temp.pos[0] = figure[1];
         temp.pos[1] = figure[2];
         temp.pos[2] = figure[3];
         temp.pos[3] = figure[4];
         temp.pos[4] = figure[5];
         temp.pos[5] = figure[6];
         temp.pos[6] = figure[7];
         temp.pos[7] = figure[8];
         solutions.push_back( temp );
         textprintf_ex( screen, font, 2, 56, 2, -1, "added as" );
         textprintf_ex( screen, font, 2, 64, 2, -1, " solution #%d", solutions.size() );
      }
   }

   wait_for_key;
   return;
}


bool check( void )
{
   cnt++;
   for( int j = 1; j < 8; j++ )
   {
      for( int i = j + 1; i <= 8; i++ )
         if( ( figure[j] == figure[i] ) ||
             ( figure[j] == figure[i] - ( j - i ) ) ||
             ( figure[j] == figure[i] + ( j - i ) ) ||
             ( figure[j] < 1 ) )
            return( true );
   }
   return( false );
}


bool check_prev( int row )
{
   for( int j = 1; j < row; j++ )
   {
      if( ( figure[j] == figure[row] ) ||
          ( figure[j] == figure[row] - ( row - j ) ) ||
          ( figure[j] == figure[row] + ( row - j ) ) ||
          ( figure[j] < 1 ) )
         return( true );
   }
   return( false );
}


bool search( int row )
{
   if( !( key[KEY_ESC] ) || ( !debug ) )
   {
      if( debug )
      {
         if( key[KEY_PLUS_PAD] )
            delay++;
         if( ( key[KEY_MINUS_PAD] ) && ( delay > 0 ) )
            delay--;
      }

      if( row == 8 )
      {
         while( ( figure[8] != 9 ) && ( check() == true ) )
         {
            if( debug )
            {
               if( key[KEY_SPACE] )
                  wait_for_key;

               draw();
               rest( delay );
            }

            figure[8]++;
            search( 8 );
         }
   
         if( figure[8] == 9 )
         {
            return( true );
         }else
            return( false );
   
      }
      if( row != 8 )
      {
         if( search( row + 1 ) == true )
         {
            figure[row] += 1;
            while( ( check_prev( row ) ) && ( figure[row] < 9 ) )
               figure[row] += 1;
   
            for( int i = row + 1; i < 9; i++ )
               figure[i] = 1;
   
            if( figure[row] == 9 )
            {
               figure[row] = 1;
               return( true );
            }else
            {
               search( row );
            }
         }else
         {
            return( false );
         }
   
      }
      if( check() == false )
         return( false );
      else
         return( true );
   }
   return( true );
}


void draw( void )
{
   clear( buffer );

   for( int i=1; i < 9; i++ )
   {
      for( int j = int( fmodf( i, 2 ) ); j < 8; j += 2 )
      {
         rectfill( buffer, SCREEN_H/8 * i     + xoffs,     SCREEN_H/8 * j,
                           SCREEN_H/8 * (i+1) + xoffs - 1, SCREEN_H/8 * (j+1) - 1,
                           15 );
      }
   }
   rect( buffer, xoffs + SCREEN_H/8, 0, SCREEN_W - 1, SCREEN_H - 1, 1 );

   for( int i=1; i < 9; i++ )
   {
      circlefill( buffer, SCREEN_H/8 * i + xoffs       + SCREEN_H/16,
                          SCREEN_H/8 * (figure[i] - 1) + SCREEN_H/16, 5, 1 );
   }

   if( debug )
   {
      //display info
      textprintf_ex( buffer, font, 1,   1,  1, -1, "starting..." );
      textprintf_ex( buffer, font, 0,   6,  1, -1, "-----------" );
   
      textprintf_ex( buffer, font, 1,  40, 1, -1, "found" );
      textprintf_ex( buffer, font, 1,  48, 1, -1, " %d solutions", solutions.size() );

      textprintf_ex( buffer, font, 1,  70, 1, -1, "Delay: %d", delay );
      textprintf_ex( buffer, font, 1,  80, 1, -1, "Press:" );
      textprintf_ex( buffer, font, 1,  88, 1, -1, " + to slowdown" );
      textprintf_ex( buffer, font, 1,  96, 1, -1, " - to speedup" );
      textprintf_ex( buffer, font, 1, 104, 1, -1, " SPACE to pause" );
   
      textprintf_ex( buffer, font, 1, 184, 12, -1, "Press \"ESC\" to" );
      textprintf_ex( buffer, font, 0, 192, 12, -1, " return to menu" );
   }

   blit( buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
   return;
}


void example()
{
   clear( screen );

   //EXAMPLE 0
   for( int i = 0; i < 8; i++ )
   {
      figure[i+1] = example0.pos[i];
   }
   draw();
   textprintf_ex( screen, font, 1,  1, 1, -1, "Example 1:" );
   textprintf_ex( screen, font, 1,  6, 1, -1, "----------" );
   textprintf_ex( screen, font, 1, 17, 4, -1, "NOT a solution!" );
   textprintf_ex( screen, font, 1, 26, 4, -1, "4 mistakes" );

   line( screen, SCREEN_H/8 * 1 + xoffs + SCREEN_H/16, SCREEN_H/8 * 5 + SCREEN_H/16,
                 SCREEN_H/8 * 2 + xoffs + SCREEN_H/16, SCREEN_H/8 * 4 + SCREEN_H/16, 4 );
   line( screen, SCREEN_H/8 * 1 + xoffs + SCREEN_H/16, SCREEN_H/8 * 5 + SCREEN_H/16,
                 SCREEN_H/8 * 3 + xoffs + SCREEN_H/16, SCREEN_H/8 * 7 + SCREEN_H/16, 4 );
   line( screen, SCREEN_H/8 * 2 + xoffs + SCREEN_H/16, SCREEN_H/8 * 4 + SCREEN_H/16,
                 SCREEN_H/8 * 4 + xoffs + SCREEN_H/16, SCREEN_H/8 * 4 + SCREEN_H/16, 4 );
   line( screen, SCREEN_H/8 * 4 + xoffs + SCREEN_H/16, SCREEN_H/8 * 4 + SCREEN_H/16,
                 SCREEN_H/8 * 7 + xoffs + SCREEN_H/16, SCREEN_H/8 * 1 + SCREEN_H/16, 4 );
   wait_for_key;

   //EXAMPLE 1
   for( int i=0; i < 8; i++ )
   {
      figure[i+1] = example1.pos[i];
   }
   draw();
   textprintf_ex( screen, font, 1,  1, 1, -1, "Example 2:" );
   textprintf_ex( screen, font, 1,  6, 1, -1, "----------" );
   textprintf_ex( screen, font, 1, 17, 4, -1, "NOT a solution!" );
   textprintf_ex( screen, font, 1, 26, 4, -1, "1 mistakes" );

   line( screen, SCREEN_H/8 * 1 + xoffs + SCREEN_H/16, SCREEN_H/8 * 0 + SCREEN_H/16,
                 SCREEN_H/8 * 6 + xoffs + SCREEN_H/16, SCREEN_H/8 * 5 + SCREEN_H/16, 4 );
   wait_for_key;

   //EXAMPLE 2
   for( int i=0; i < 8; i++ )
   {
      figure[i+1] = example2.pos[i];
   }
   draw();
   textprintf_ex( screen, font, 1,  1, 1, -1, "Example 3:" );
   textprintf_ex( screen, font, 1,  6, 1, -1, "----------" );
   textprintf_ex( screen, font, 1, 17, 2, -1, "BINGO!!!" );
   textprintf_ex( screen, font, 1, 26, 2, -1, "0 mistakes" );

   wait_for_key;
   return;
}


void start_debug()
{
   //init
   solutions.clear();
   solution temp;
   bool not_found = true;
   clear_keybuf();
   clear( screen );
/*
   for( int i=1; i <= 8; i++ )
   {
      figure[i] = i;
   }
*/

   //starting positions
/* // good start
 * figure[1] = 1;
 * figure[2] = 3;
 * figure[3] = 5;
 * figure[4] = 7;
 * figure[5] = 2;
 * figure[6] = 4;
 * figure[7] = 6;
 * figure[8] = 8;
*/
   // diagonal
   for( int i = 1; i <= 8; i++ )
      figure[i] = i;

   //start
   old_timer = g_timer;
   not_found = search( 1 );
   if( !not_found )
   {
      for( int i=0; i<8; i++)
         temp.pos[i] = figure[i + 1];
      solutions.push_back( temp );

      //find more solutions
      do
      {
         figure[8]++;
         not_found = true;
         not_found = search( 1 );
      
         if( !not_found )
         {
            for( int i=0; i<8; i++)
               temp.pos[i] = figure[i + 1];
            solutions.push_back( temp );
         }
      } while( !( ( solutions[0].pos[0] == solutions[solutions.size()-1].pos[0] ) &&
                  ( solutions[0].pos[1] == solutions[solutions.size()-1].pos[1] ) &&
                  ( solutions[0].pos[2] == solutions[solutions.size()-1].pos[2] ) &&
                  ( solutions[0].pos[3] == solutions[solutions.size()-1].pos[3] ) &&
                  ( solutions[0].pos[4] == solutions[solutions.size()-1].pos[4] ) &&
                  ( solutions[0].pos[5] == solutions[solutions.size()-1].pos[5] ) &&
                  ( solutions[0].pos[6] == solutions[solutions.size()-1].pos[6] ) &&
                  ( solutions[0].pos[7] == solutions[solutions.size()-1].pos[7] ) ) &&
                  ( !key[KEY_ESC] ) );
      old_timer = g_timer - old_timer;
      if( !key[KEY_ESC] )
         solutions.pop_back();
   }

   if( key[KEY_ESC] )
   {
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 4, 0, "ERROR: User abort!" );
      while( key[KEY_ESC] )
         clear_keybuf();
      clear_keybuf();
   }else if( not_found )
   {
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 4, 0, "ERROR: could not find any solution! (%d)", old_timer );
   }else
   {
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2     , 2, 0, "Success! Found %d solution in %d ms", solutions.size(), old_timer );
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2 +  8, 2, 0, "Only %d solutions are unique.", solutions.size()/4 );
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2 + 16, 2, 0, "(Each of them can be rotated)" );
   }

   wait_for_key;
   return;
}


void show( void )
{
   clear( screen );
   if( solutions.size() == 0 )
   {
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2 - 8, 4, 0, "ERROR: No solutions found yet!" );
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2    , 4, 0, "Solve Problem firt!" );

      wait_for_key;
   }else
   {
      for( int i = 0; i < solutions.size(); i++ )
      {
         for( int j = 0; j < 8; j++ )
            figure[j + 1] = solutions[i].pos[j];
         draw();
         textprintf_ex( screen, font, 1,   1,  1, -1, "Solution %2d/%2d:", i+1, solutions.size() ); 
         textprintf_ex( screen, font, 0,   6,  1, -1, "---------------" ); 
         textprintf_ex( screen, font, 0,  80,  1, -1, "Press:" ); 
         textprintf_ex( screen, font, 0,  88,  1, -1, "-j to jump to #" ); 
         textprintf_ex( screen, font, 0,  96,  1, -1, "-any key" );
         textprintf_ex( screen, font, 0, 104,  1, -1, " to show next" );
         textprintf_ex( screen, font, 0, 112,  1, -1, "-LEFT or DOWN" );
         textprintf_ex( screen, font, 0, 120,  1, -1, " to show prev." );

         textprintf_ex( screen, font, 1, 183, 12, -1, "Press \"ESC\" to" );
         textprintf_ex( screen, font, 0, 191, 12, -1, " return to menu" );

         //input
         clear_keybuf();
         int input = 0;
         input = ( readkey() >> 8 );

         // show prev
         if( ( input == KEY_LEFT ) || ( input == KEY_DOWN ) )
         {
            if( i > 0 )
            {
               i -= 2;
            }else
            {
               i--;
            }
         }

         //jump to solution number X
         if( input == KEY_J )
         {
            int pos = 0;
            textprintf_ex( screen, font, 0, 40, 1, 0, "jump to" );
            if( i < int( solutions.size() - 1 ) )
               textprintf_ex( screen, font, 0, 48, 1, 0, " solution # %3d", i + 2 );
            else
            {
               i = -1;
               textprintf_ex( screen, font, 0, 48, 1, 0, " solution # %3d", i + 2 );
            }
            do
            {
               clear_keybuf();

               //readkey and shift to code
               input = ( readkey() >> 8 );

               if( ( input == KEY_BACKSPACE ) && ( pos > 0 ) )
                  pos = int( pos / 10 );
               else if( input != KEY_ENTER )
               {
                  input -= 27;
                  if( input > 9 )
                     input -= 10;
   
                  //add input if it is a number
                  if( ( 0 <= input ) && ( input <= 9 ) && ( pos < 100 ) )
                     pos = ( pos * 10 ) + input;
               }
               //textout
               textprintf_ex( screen, font, 0, 40, 1, 0, "jump to " );
               textprintf_ex( screen, font, 0, 48, 1, 0, " solution # %3d", pos );

            }while( input != KEY_ENTER ); // [KEY_ENTER] after modification!

            //jump if input is korrect
            if( pos != 0 )
               i = pos - 2;
         }else if( input == KEY_ESC )
         {
            break;
         }
         if( i > int( solutions.size() - 2 ) )
            i = solutions.size() - 2;
      }
   }

   return;
}


void start_solutions( void )
{
   clear( screen );
   textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2 - 8, 1, -1, "starting..." );

   //init
   solutions.clear();
   solution temp;
   bool not_found = true;
   clear_keybuf();

   // start positions
/* // first line
 * for( int i=1; i <= 8; i++ )
 *    figure[i] = 1;
*/

   // diagonal
   for( int i = 1; i <= 8; i++ )
      figure[i] = i;

   //start
   old_timer = g_timer;
   not_found = search( 1 );
   if( !not_found )
   {
      for( int i=0; i<8; i++)
         temp.pos[i] = figure[i + 1];
      solutions.push_back( temp );

      //find more solutions
      do
      {
         figure[8]++;
         not_found = true;
         not_found = search( 1 );
      
         if( !not_found )
         {
            for( int i=0; i<8; i++)
               temp.pos[i] = figure[i + 1];
            solutions.push_back( temp );
         }
      } while( !( ( solutions[0].pos[0] == solutions[solutions.size()-1].pos[0] ) &&
                  ( solutions[0].pos[1] == solutions[solutions.size()-1].pos[1] ) &&
                  ( solutions[0].pos[2] == solutions[solutions.size()-1].pos[2] ) &&
                  ( solutions[0].pos[3] == solutions[solutions.size()-1].pos[3] ) &&
                  ( solutions[0].pos[4] == solutions[solutions.size()-1].pos[4] ) &&
                  ( solutions[0].pos[5] == solutions[solutions.size()-1].pos[5] ) &&
                  ( solutions[0].pos[6] == solutions[solutions.size()-1].pos[6] ) &&
                  ( solutions[0].pos[7] == solutions[solutions.size()-1].pos[7] ) ) &&
                  ( !key[KEY_ESC] ) );
      old_timer = g_timer - old_timer;
      if( !key[KEY_ESC] )
         solutions.pop_back();
   }

   if( not_found )
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 4, 0, "ERROR: could not find any solution! (%d)", old_timer );
   else
   {
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2, 2, 0, "Success! Found %d solution in %d ms", solutions.size(), old_timer );
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2 +  8, 2, 0, "Only %d solutions are unique.", solutions.size()/4 );
      textprintf_centre_ex( screen, font, SCREEN_W/2, SCREEN_H/2 + 16, 2, 0, "(Each of them can be rotated)" );
   }

   wait_for_key;
   return;
}


void show_menu( void )
{
   bool Esc = false;

   do
   {

      clear( screen );
      //Welcome
      textprintf_ex( screen, font,  5,  10,  1, -1, "Welcome to another");
      textprintf_ex( screen, font,  5,  18,  1, -1, " \"the other one\"-Production");
      //About
      textprintf_ex( screen, font,  5,  60,  1, -1, "This Program finds solutions");
      textprintf_ex( screen, font,  5,  68,  1, -1, "to the Queens-Riddle!");
      //Problem
      textprintf_ex( screen, font,  5,  80,  1, -1, "Which is:");
      textprintf_ex( screen, font,  5,  88,  1, -1, "  Put eight Queens on a chessboard so");
      textprintf_ex( screen, font,  5,  96,  1, -1, "  that they can not strike each other!");
      //MAX_POS
      textprintf_ex( screen, font,  5, 112,  1, -1, "(there are (8^8) = (2^24) = %s", MAX_POS );
      textprintf_ex( screen, font,  5, 120,  1, -1, " possible combinations!!!)" );
      //Make choice
      textprintf_ex( screen, font,  5, 140,  1, -1, "Press \"E\" to look at an Example" );
      textprintf_ex( screen, font,  5, 148,  1, -1, "Press \"P\" to try to solve it by hand :)" );
      textprintf_ex( screen, font,  5, 156,  1, -1, "Press \"SPACE\" let the Computer do it" );
      textprintf_ex( screen, font,  5, 164,  1, -1, "Press \"S\" to show solutions" );
      textprintf_ex( screen, font,  5, 172,  1, -1, "Press \"D\" to look at Solving-Process" );
      textprintf_ex( screen, font,  5, 184, 12, -1, "Press \"ESC\" to Quit" );
   
      clear_keybuf();
      int input = 0;
      input = ( readkey() >> 8 );

      if( input == KEY_SPACE )
         start_solutions();
      else if( input == KEY_D )
      {
         debug = true;
         start_debug();
         debug = false;
      }else if( input == KEY_E )
         example();
      else if( input == KEY_S )
         show();
      else if( input == KEY_P )
         play();
      else if( input == KEY_ESC )
         Esc = true;
   }while( !Esc );
}

         
int main( int argc, char *argv[] )
{
     
   // init system
   allegro_init();
   install_keyboard();
   install_mouse();
   install_timer();

   if( argc > 1 )
   {
      allegro_message( "\n\n*******************************************************************************\n" );
      allegro_message( "*                                                                             *\n" );
      allegro_message( "* Usage: queens2                                                              *\n" );
      allegro_message( "*  There are no arguments needed!                                             *\n" );
      allegro_message( "* -------------------------------                                             *\n" );
      allegro_message( "* About: Queens2.exe (v2.0)                                                   *\n" );
      allegro_message( "*  This Programm was written by Lorenz Quack and released on 18.10.2003       *\n" );
      allegro_message( "*  It finds solutions to the Queens-Riddle which is:                          *\n" );
      allegro_message( "*  \"Put eight Queens on a chessboard so that they can not strike each other!\" *\n" );
      allegro_message( "*                                                                             *\n" );
      allegro_message( "*  There are 8^8 = 2^24 = 16777216 different combinations if you assume       *\n" );
      allegro_message( "*    that in each row there is only one queen...                              *\n" );
      allegro_message( "*  Furthermore you can reduce the number of possibilitys by saying that in    *\n" );
      allegro_message( "*    each column there is only one queen as well!                             *\n" );
      allegro_message( "*  In that case you're down by only 8! = 8*7*6... = 40320 combinations :)     *\n" );
      allegro_message( "*                                                                             *\n" );
      allegro_message( "*******************************************************************************\n\n" );
//      textprintf_ex( screen, font,  5, 112,  1, -1, "(there are (8^8) = (2^24) = %s", MAX_POS );
  //    textprintf_ex( screen, font,  5, 120,  1, -1, " posible combinations!!!)" );

   }else
   {
      // init timer
      LOCK_VARIABLE( g_timer );
      LOCK_FUNCTION( ttimer );
      install_int( ttimer, 1 );
   
      // init GFX
      if (set_gfx_mode(GFX_AUTODETECT, 320, 200, 0, 0) != 0)
      {
         set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
         allegro_message("Error setting graphics mode\n%s\n", allegro_error);
         return 1;
      }
   
   
      // init Variables
      xoffs  = SCREEN_W - SCREEN_H - SCREEN_H/8;
      debug  = false;
      delay  = 50;
      buffer = create_bitmap( SCREEN_W, SCREEN_H );
      show_mouse( NULL );
   
   
      figure.clear();
      figure.push_back( 0 );
   
      for( int i=0; i < 8; i++ )
         figure.push_back( 1 );
   
      //set examples
      example0.pos[0] = 6;
      example0.pos[1] = 5;
      example0.pos[2] = 8;
      example0.pos[3] = 5;
      example0.pos[4] = 1;
      example0.pos[5] = 4;
      example0.pos[6] = 2;
      example0.pos[7] = 7;
   
      example1.pos[0] = 1;
      example1.pos[1] = 5;
      example1.pos[2] = 7;
      example1.pos[3] = 2;
      example1.pos[4] = 4;
      example1.pos[5] = 6;
      example1.pos[6] = 8;
      example1.pos[7] = 3;
   
      example2.pos[0] = 3;
      example2.pos[1] = 5;
      example2.pos[2] = 8;
      example2.pos[3] = 4;
      example2.pos[4] = 1;
      example2.pos[5] = 7;
      example2.pos[6] = 2;
      example2.pos[7] = 6;
   
      // start programm
      show_menu();
   
      // show credits
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message( "\n Queens-Riddle v2.0 by Lorenz Quack (18.10.2003)\n" );
      allegro_message( " ===============================================\n" );
      allegro_message( " Thanks for using!\n\n" );
   }

   //destruct
   figure.clear();
   solutions.clear();
   destroy_bitmap( buffer );
   allegro_exit();

   return( 0 );
} END_OF_MAIN();
