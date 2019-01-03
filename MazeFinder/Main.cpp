#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include "Maze.h"

static constexpr char OPTION_LOAD_FILE = '1';
static constexpr char OPTION_PRINT_MAZE = '2';
static constexpr char OPTION_EXECUTE_DFS = '3';
static constexpr char OPTION_EXECUTE_BFS = '4';
static constexpr char OPTION_RELOAD_FILE = '5';
static constexpr char OPTION_QUIT = 'q';

static constexpr size_t SINGLE_CHAR_SIZE = 1;
static constexpr size_t FIRST_CHARACTER  = 0;

bool ImportMaze( const std::string & filename )
{
    return true;
}

void DisplayMenu(const std::string & filename)
{
    std::cout << "\n"
        << OPTION_LOAD_FILE << ". Load file\n";
    if ( !filename.empty() )
    {
        std::cout << OPTION_PRINT_MAZE << ". Print Maze\n"
                  << OPTION_EXECUTE_DFS << ". Execute DFS\n"
                  << OPTION_EXECUTE_BFS << ". Execute BFS\n"
                  << OPTION_RELOAD_FILE << ". Reload file\n";
        std::cout << "Currently loaded file: " << filename << "\n";
    }

    std::cout << OPTION_QUIT << ". Quit\n" << std::endl;
    std::cout << "Please enter your option: ";
}

bool ValidateUserInput( const char input, const bool mazeFileLoaded )
{
    switch ( static_cast<const char>( tolower( input ) ) )
    {
        case OPTION_PRINT_MAZE: //intentional fallthrough
        case OPTION_EXECUTE_DFS: //intentional fallthrough
        case OPTION_EXECUTE_BFS: //intentional fallthrough
        case OPTION_RELOAD_FILE: //intentional fallthrough
        {
            if ( !mazeFileLoaded )
            {
                return false;
            }
        }
        case OPTION_LOAD_FILE: //intentional fallthrough
        case OPTION_QUIT:
        {
            return true;
        } break;
        default:
        {
            return false;
        } break;
    }
}

char GetUserOption( const std::string & filename )
{
    std::string userInput;
    char output = NULL;

    do
    {
        DisplayMenu( filename );
        std::cin >> userInput;
        if ( userInput.size() == SINGLE_CHAR_SIZE )
        {
            output = static_cast< char >( tolower( userInput[FIRST_CHARACTER] ) );
        }
    } while ( !ValidateUserInput( output, !filename.empty() ) );

    return output;
}

bool LoadFile( Maze& maze, const std::string & filename )
{
    return maze.LoadMaze( filename );
}

int main( int argc, char ** argv )
{
    std::string filename;
    Maze maze;
    char choice = GetUserOption( filename );

    while ( choice != OPTION_QUIT )
    {
        switch ( choice )
        {
            case OPTION_LOAD_FILE:
            {
                std::cout << "Loading file..." << std::endl;
                filename = "1.txt";
                if ( !LoadFile( maze, filename ) )
                {
                    std::cout << "Error loading file: " << filename << std::endl;
                    filename.clear();
                }
            } break;
            case OPTION_PRINT_MAZE:
            {
                std::cout << "Printing maze..." << std::endl;
                maze.PrintMaze();
            } break;
            case OPTION_EXECUTE_DFS:
            {
                std::cout << "Executing Depth First Search..." << std::endl;
                int minimumNumSteps = maze.MinimumNumberOfSpacesDFS( 1, 1 );
                if ( minimumNumSteps == -1 )
                {
                    std::cout << "No solvable minimum path..." << std::endl;
                }
                else
                {
                    std::cout << "Minimum path in " << minimumNumSteps << " steps." << std::endl;
                }
            } break;
            case OPTION_EXECUTE_BFS:
            {
                std::cout << "Executing Breadth First Search..." << std::endl;
                int minimumNumSteps = maze.MinimumNumberOfSpacesBFS( 1, 1 );
                if ( minimumNumSteps == -1 )
                {
                    std::cout << "No solvable minimum path..." << std::endl;
                }
                else
                {
                    std::cout << "Minimum path in " << minimumNumSteps << " steps." << std::endl;
                }
            } break;
            case OPTION_RELOAD_FILE:
            {
                std::cout << "Reloading File..." << std::endl;
                if ( !LoadFile( maze, filename ) )
                {
                    std::cout << "Error reloading file: " << filename << std::endl;
                    filename.clear();
                }
            } break;
            case OPTION_QUIT:
            {
            } break;
            default:
            {
                std::cout << "Error: getting choice!" << std::endl;
                std::cout << "Value is: " << choice << std::endl;
            } break;
        }
        choice = GetUserOption( filename );
    }
    system( "pause" );
    return EXIT_SUCCESS;
}
