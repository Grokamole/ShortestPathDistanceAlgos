#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include "Maze.h"

static constexpr char OPTION_LOAD_FILE = '1'; ///< The option to load a file.
static constexpr char OPTION_PRINT_MAZE = '2'; ///< The option to print the maze.
static constexpr char OPTION_EXECUTE_DFS = '3'; ///< The option to execute depth first search.
static constexpr char OPTION_EXECUTE_BFS = '4'; ///< The option to execute breadth first search.
static constexpr char OPTION_RELOAD_FILE = '5'; ///< The option to reload the file.
static constexpr char OPTION_QUIT = 'q'; ///< The option to quit.

static constexpr size_t SINGLE_CHAR_SIZE = 1; ///< Single character size.
static constexpr size_t FIRST_CHARACTER  = 0; ///< Used to denote the first character in an array.
static constexpr int PATH_UNREACHABLE = -1; ///< Used to denote a path as unreachable

/** Displays the main menu.

@param filename The currently selected filename.
*/
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

/** Validates the user's input option.

@param input The user's choice.
@param mazeFileLoaded Whether the maze file is loaded.

@retval true if the user's input is valid.
@retval false if the user's input is invalid.
*/
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

/** Gets the filename from the user.

@return the user's chosen filename for a maze file.
*/
std::string GetUserFilename()
{
    std::string filename;
    std::cout << "Please enter filename: ";
    std::cin >> filename;
    return filename;
}

/** Get a valid option from the user.

@param filename The filename of the currently loaded file.

@return The character representing the user's valid character option.
*/
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

/** Load a maze file.

@param maze The maze to load the file into.
@param filename The filename to load into the maze.

@retval true If the maze could be successfully loaded.
@retval false If the maze could not be successfully loaded.
*/
bool LoadFile( Maze& maze, const std::string & filename )
{
    return maze.LoadMaze( filename );
}

/** Gets the starting coordinates for a run.

@param maxRow The maximum row index.
@param maxColumn The maximum column index.

@return a pair of starting coordinates in user X,Y order
*/
std::pair<size_t, size_t> GetStartingCoordinates( const size_t maxRow, const size_t maxColumn )
{
    std::string userOption;
    int x;
    int y;
    char comma;
    while ( userOption.empty() )
    {
        std::cout << "Please enter starting coordinate [0-" << maxColumn << ",0-" << maxRow << "] as X,Y: ";
        std::cin >> userOption;
        std::istringstream parseLine( userOption ); // used to parse user option

        parseLine >> x >> comma >> y; // get the x, comma, and y coordinate

        if ( parseLine.fail() || ( ',' != comma ) || x > maxColumn || y > maxRow )
        {
            userOption.clear();
            std::cout << std::endl;
        }
    }
    return { x, y };
}

/** Main. Runs the MazeFinder program.

@param argc Unused.
@param argv Unused.

@return EXIT_SUCCESS.
*/
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
                filename = GetUserFilename();
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
                const std::pair<size_t, size_t> coordinates = GetStartingCoordinates( maze.GetRowSize() - 1, maze.GetColumnSize() - 1 );
                const int minimumNumSteps = maze.MinimumNumberOfSpacesDFS( coordinates.second, coordinates.first );
                if ( minimumNumSteps == PATH_UNREACHABLE )
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
                const std::pair<size_t, size_t> coordinates = GetStartingCoordinates( maze.GetRowSize() - 1, maze.GetColumnSize() - 1 );
                const int minimumNumSteps = maze.MinimumNumberOfSpacesBFS( coordinates.second, coordinates.first );
                if ( minimumNumSteps == PATH_UNREACHABLE )
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

    return EXIT_SUCCESS;
}
