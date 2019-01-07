// Joseph Miller (c) 2019

#include "Maze.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_map>

static constexpr size_t MAX_ROWS = 256; ///< Used to limit the number of rows so as not to likely run out of stack space for depth first search.
static constexpr size_t MAX_COLUMNS = 256; ///< Used to limit the number of columns so as not to likely run out of stack space for depth first search.
static constexpr int PATH_UNREACHABLE = -1; ///< Used to denote a path as unreachable

/** Loads the maze into memory via filename.

@param filename The path to the maze to load.

@retval true if the maze was successfully loaded.
@retval false if the maze was not successfully loaded.
*/
bool Maze::LoadMaze( const std::string & filename )
{
    ClearMaze(); // clear the maze if there is existing maze data

    std::ifstream file( filename ); // holds the file to get map data from

    if ( !file.good() ) // check that the file could be sucessfully accessed
    {
        return false;
    }

    size_t rows = 0; // holds the number of rows from the map
    size_t columns = 0; // holds the number of columns from the map
    char commaCheck = 0; // used to make sure there is a comma between the row and column
    std::string line; // used to hold line data

    if ( !std::getline( file, line ) ) // get the first line of the file which should contain the "rows, columns" data
    {
        return false;
    }

    std::istringstream parseLine( line ); // used to parse each line

    parseLine >> columns >> commaCheck >> rows; // get the columns, the comma, and rows

    if ( parseLine.fail() || ',' != commaCheck ) // check that the data retrieval did not fail and that the comma exists
    {
        return false;
    }

    if ( ( columns > MAX_COLUMNS ) || ( rows > MAX_ROWS ) ) // make sure we didn't go over the MAX limit
    {
        return false;
    }

    ReallocateMaze( rows, columns ); // allocate the underlying data structure

    char fieldValue; // holds the value for each byte of map data

    for ( size_t row = 0; row < rows; ++row ) // iterate over the map row by row
    {
        if ( !std::getline( file, line ) ) // check that we could get the line
        {
            return false;
        }
        parseLine.clear(); // clear the parseline data
        parseLine.str( line ); // import the next row
        for ( size_t column = 0; column < columns; ++column ) // for each column value in the row
        {
            parseLine >> fieldValue; // get the byte of data
            if ( parseLine.fail() ) // check that the retrieval was successful
            {
                return false;
            }
            if ( !SetPositionalData( row, column, fieldValue ) ) // set the map data with the byte
            {
                return false;
            }
        }
    }

    m_rowSize = rows; // set the number of rows
    m_columnSize = columns; // set the number of columns

    return true; // map successfully parsed
}

/** Checks for map load validation.

@retval true Maze was successfully loaded.
@retval false Maze was not successfully loaded.
*/
bool Maze::MazeLoaded() const
{
    return m_rowSize;
}

/** Retrieves the minimum number of spaces using breadth first search.
    @note This function has the side effect of reporting the number of steps taken via std out for reporting purposes.

@param start_row the row the start location begins with.
@param start_column the column the start location begins with.

@return The minimum number of spaces from the start to the closest end point, or -1 if no end point could be reached from start or if start was invalid.
*/
int Maze::MinimumNumberOfSpacesBFS( const size_t start_row, const size_t start_column ) const
{
    if ( start_row > m_rowSize || start_column > m_columnSize ) // check that the row and column for start is within bounds
    {
        return PATH_UNREACHABLE;
    }
    if ( GetPositionalData( start_row, start_column ) == MAZE_DATA_TYPE::EXIT ) // If the start is the exit, the end point costs 0 spaces
    {
        return 0;
    }

    std::unordered_map<size_t, int> distanceMap; // holds the distance from start
    std::queue<std::pair<size_t, size_t>> mazeQueue; // the queue of points for breadth first search

    mazeQueue.push( { start_row, start_column } ); // enqueue the starting position
    distanceMap[ mazeQueue.front().first * m_columnSize + mazeQueue.front().second ] = 0; // the first point is 0 from itself

    size_t currentRow; // holds the currently selected row
    size_t currentColumn; // holds the currently selected column
    int distance; // holds the distance traveled from the starting coordinate
    int numSteps = 0; // holds the number of steps
    while ( !mazeQueue.empty() ) // while there are spaces in the queue to process...
    {
        ++numSteps; // increase the number of steps taken
        currentRow = mazeQueue.front().first; // set the current row from the front of the queue
        currentColumn = mazeQueue.front().second; // set the current column from the front of the queue
        distance = distanceMap[ currentRow * m_columnSize + currentColumn ]; // get the distance of the current point
        switch( GetPositionalData( currentRow, currentColumn ) ) // get the value of the current point
        {
            case MAZE_DATA_TYPE::EXIT: // exit found, report number of steps and return distance from start
            {
                std::cout << "Num Steps: " << numSteps << std::endl;
                return distance;
            } break;
            case MAZE_DATA_TYPE::BLOCKED: // this is a blocked point, dequeue since we can't travel to it
            {
                mazeQueue.pop();
                continue;
            }
            default: // cleared point
            {
            } break;
        }

        ++distance; // increase distance for surrounding points
        // if we haven't traveled to the left, enqueue the point
        if ( ( currentRow > 0 ) && ( distanceMap.find( ( currentRow - 1 ) * m_columnSize + currentColumn )  == distanceMap.cend() ) )
        {
            distanceMap[ ( currentRow - 1 ) * m_columnSize + currentColumn ] = distance;
            mazeQueue.push( { currentRow - 1, currentColumn } );
        }
        // if we haven't traveled up, enqueue the point
        if ( ( currentColumn > 0 ) && ( distanceMap.find( currentRow * m_columnSize + ( currentColumn - 1 ) ) == distanceMap.cend() ) )
        {
            distanceMap[ currentRow * m_columnSize + ( currentColumn - 1 ) ] = distance;
            mazeQueue.push( { currentRow, currentColumn - 1 } );
        }
        // if we haven't traveled right, enqueue the point
        if ( ( currentRow + 1 < m_rowSize ) && ( distanceMap.find( ( currentRow + 1 ) * m_columnSize + currentColumn ) == distanceMap.cend() ) )
        {
            distanceMap[ ( currentRow + 1 ) * m_columnSize + currentColumn ] = distance;
            mazeQueue.push( { currentRow + 1, currentColumn } );
        }
        // if we haven't traveled down, enqueue the point
        if ( ( currentColumn + 1 < m_columnSize ) && ( distanceMap.find( currentRow * m_columnSize + ( currentColumn + 1 ) ) == distanceMap.cend() ) )
        {
            distanceMap[currentRow * m_columnSize + ( currentColumn + 1 )] = distance;
            mazeQueue.push({ currentRow, currentColumn + 1 });
        }

        // finished processing
        mazeQueue.pop();
    }

    // could not find an end point, so report the number of steps attempted and return -1
    std::cout << "Num Steps: " << numSteps << std::endl;
    return PATH_UNREACHABLE;
}

/** Retrieves the minimum number of spaces using a depth first search.

@param start_row The row of the starting point.
@param start_column The column of the starting point.

@return the minimum number of spaces from the starting point to the nearest exit point, or -1 if no exit point could be found or the starting point is invalid.
*/
int Maze::MinimumNumberOfSpacesDFS( const size_t start_row, const size_t start_column ) const
{
    // if out of bounds, report that the end can't be found
    if ( start_row > m_rowSize || start_column > m_columnSize )
    {
        return PATH_UNREACHABLE;
    }
    // if the starting point is the exit, the number of spaces is 0
    if ( GetPositionalData( start_row, start_column ) == MAZE_DATA_TYPE::EXIT )
    {
        return 0;
    }

    std::vector<std::vector<bool>> traversed( m_rowSize, std::vector<bool>( m_columnSize, false ) );  // holds the list of traversed spaces

    traversed[start_row][start_column] = true; // the starting position is traversed
    
    std::vector<int> smallest_path; // holds the smallest paths

    int numSteps = 0; // holds the number of steps traversed

    AddValueToPathSizeVectorIfPositive( smallest_path, DepthFirstRecursion( start_row - 1, start_column, traversed, 0, numSteps ) );
    AddValueToPathSizeVectorIfPositive( smallest_path, DepthFirstRecursion( start_row, start_column - 1, traversed, 0, numSteps ) );
    AddValueToPathSizeVectorIfPositive( smallest_path, DepthFirstRecursion( start_row, start_column + 1, traversed, 0, numSteps ) );
    AddValueToPathSizeVectorIfPositive( smallest_path, DepthFirstRecursion( start_row + 1, start_column, traversed, 0, numSteps ) );

    std::cout << "Num Steps: " << numSteps << std::endl; // report the number of total steps taken

    // if there was no valid path, return -1
    if ( smallest_path.empty() )
    {
        return PATH_UNREACHABLE;
    }

    // return the smallest path from the path list
    return *std::min_element( smallest_path.cbegin(), smallest_path.cend() );
}

/** The recursive DFS function for determining the shortest path.

@param current_row The starting row of the current space to check.
@param current_column The starting column of the current space to check.
@param traversed The vector of currently traversed spaces.
@param nodesInPath, the number of nodes currently in the path.
@param numSteps, the number of steps used.

@return The shortest number of steps from this point on, or -1 if the point cannot reach an exit.
*/
int Maze::DepthFirstRecursion( const size_t current_row, const size_t current_column, std::vector<std::vector<bool>>& traversed, const int nodesInPath, int & numSteps ) const
{
    // if the point is outside of bounds, return -1
    if ( current_row >= m_rowSize || current_column >= m_columnSize )
    {
        return PATH_UNREACHABLE;
    }

    // if the point has been traversed, return -1
    if ( traversed[current_row][current_column] )
    {
        return PATH_UNREACHABLE;
    }

    ++numSteps; // increase the number of steps

    // check the current space
    switch ( GetPositionalData( current_row, current_column ) )
    {
        case MAZE_DATA_TYPE::BLOCKED: // if blocked, this space won't be able to get to the exit
        {
            traversed[current_row][current_column] = true; // only traverse this space once
            return PATH_UNREACHABLE;
        } break;
        case MAZE_DATA_TYPE::EXIT: // exit found, return the path size
        {
            return nodesInPath + 1;
        } break;
        default:
        {
        } break;
    }

    traversed[current_row][current_column] = true; // this space is traversed

    std::vector<int> smallest_path;

    AddValueToPathSizeVectorIfPositive( smallest_path, DepthFirstRecursion( current_row - 1, current_column, traversed, nodesInPath + 1, numSteps ) );
    AddValueToPathSizeVectorIfPositive( smallest_path, DepthFirstRecursion( current_row, current_column - 1, traversed, nodesInPath + 1, numSteps ) );
    AddValueToPathSizeVectorIfPositive( smallest_path, DepthFirstRecursion( current_row, current_column + 1, traversed, nodesInPath + 1, numSteps ) );
    AddValueToPathSizeVectorIfPositive( smallest_path, DepthFirstRecursion( current_row + 1, current_column, traversed, nodesInPath + 1, numSteps ) );
    
    traversed[current_row][current_column] = false; // remove this space from the path

    if ( smallest_path.empty() ) // if there is no path, return -1
    {
        return PATH_UNREACHABLE;
    }

    return *std::min_element( smallest_path.cbegin(), smallest_path.cend() ); // return the minimum path size
}

/** Helper function for pushing only positive non-zero values into a vector.

@param pathSizes the path vector to add positive non-zero values to.
@param value the value to check
*/
void Maze::AddValueToPathSizeVectorIfPositive( std::vector<int>& pathSizes, const int value ) const
{
    if ( value > 0 )
    {
        pathSizes.push_back( value );
    }
}

/** Retrieves the currently set number of rows in the maze.

@return the current number of rows.
*/
size_t Maze::GetRowSize() const
{
    return m_rowSize;
}

/** Retrieves the currently set number of columns in the maze.

@return the current number of columns.
*/
size_t Maze::GetColumnSize() const
{
    return m_columnSize;
}

/** Prints the maze to std output.
*/
void Maze::PrintMaze() const
{
    if ( !MazeLoaded() )
    {
        std::cout << "Error: Maze not in a valid state." << std::endl;
    }
    for ( size_t row = 0; row < m_rowSize; ++row )
    {
        for ( size_t column = 0; column < m_columnSize; ++column )
        {
            switch ( GetPositionalData( row, column ) )
            {
                case MAZE_DATA_TYPE::BLOCKED:
                {
                    std::cout << 'B';
                } break;
                case MAZE_DATA_TYPE::CLEAR:
                {
                    std::cout << '.';
                } break;
                case MAZE_DATA_TYPE::EXIT:
                {
                    std::cout << 'X';
                } break;
                default:
                {
                    std::cout << '?';
                } break;
            }
        }
        std::cout << '\n';
    }
}

/** Retrieves the value set in the maze at the position.

@param row The row to retrieve the value from.
@param column The column to retrieve the value from.
*/
Maze::MAZE_DATA_TYPE Maze::GetPositionalData( const size_t row, const size_t column ) const
{
    if ( row >= m_rowSize || column >= m_columnSize )
    {
        throw std::runtime_error( "Cannot get positional data where row or column exceeds size." );
    }

    return m_mapData[row][column];
}

/** Sets the position in the maze to be a specific value.

@param row The row position to set the data to.
@param column The column position to set the data to.
@param value The value to set the data to.

@retval true if the data was successfully set.
@retval false if the data was not successfully set.
*/
bool Maze::SetPositionalData( const size_t row, const size_t column, const char value )
{
    switch ( value )
    {
        case '0':
        {
            m_mapData[row][column] = MAZE_DATA_TYPE::CLEAR;
        } break;
        case '1':
        {
            m_mapData[row][column] = MAZE_DATA_TYPE::BLOCKED;
        } break;
        case '2':
        {
            m_mapData[row][column] = MAZE_DATA_TYPE::EXIT;
        } break;
        default:
        {
            return false;
        } break;
    }

    return true;
}

/** Clears the maze data from the object.
*/
void Maze::ClearMaze()
{
    m_rowSize = 0;
    m_columnSize = 0;
    m_mapData.clear();
}

/** Sets up the internal data structure for maze data.

@param rows The number of rows in the maze.
@param columns The number of columns in the maze.
*/
void Maze::ReallocateMaze( const size_t rows, const size_t columns )
{
    m_mapData.resize( rows );
    for ( size_t row = 0; row < rows; ++row )
    {
        m_mapData[row].resize( columns );
    }
}
