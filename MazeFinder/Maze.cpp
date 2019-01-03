#include "Maze.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_map>

bool Maze::LoadMaze( const std::string & filename )
{
    ClearMaze();

    std::ifstream file( filename );

    if ( !file.good() )
    {
        return false;
    }

    size_t rows = 0;
    size_t columns = 0;
    char commaCheck = 0;
    std::string line;

    if ( !std::getline( file, line ) )
    {
        return false;
    }

    std::istringstream parseLine( line );

    parseLine >> columns >> commaCheck >> rows;

    if ( parseLine.fail() || ',' != commaCheck )
    {
        return false;
    }

    ReallocateMaze( rows, columns );

    char fieldValue;

    for ( size_t row = 0; row < rows; ++row )
    {
        if ( !std::getline( file, line ) )
        {
            return false;
        }
        parseLine.clear();
        parseLine.str( line );
        for ( size_t column = 0; column < columns; ++column )
        {
            parseLine >> fieldValue;
            if ( parseLine.fail() )
            {
                return false;
            }
            if ( !SetPositionalData( row, column, fieldValue ) )
            {
                return false;
            }
        }
    }

    m_rowSize = rows;
    m_columnSize = columns;

    return true;
}

bool Maze::MazeLoaded() const
{
    return m_rowSize;
}

int Maze::MinimumNumberOfSpacesBFS( const size_t start_row, const size_t start_column ) const
{
    if ( start_row > m_rowSize || start_column > m_columnSize )
    {
        return -1;
    }
    if ( GetPositionalData( start_row, start_column ) == MAZE_DATA_TYPE::EXIT )
    {
        return 0;
    }

    std::unordered_map<size_t, int> distanceMap;
    std::queue<std::pair<size_t, size_t>> mazeQueue;

    mazeQueue.push( { start_row, start_column } );
    distanceMap[ mazeQueue.front().first * m_columnSize + mazeQueue.front().second ] = 0;

    size_t currentRow;
    size_t currentColumn;
    int distance;
    int numSteps = 0;
    while ( !mazeQueue.empty() )
    {
        ++numSteps;
        currentRow = mazeQueue.front().first;
        currentColumn = mazeQueue.front().second;
        distance = distanceMap[ currentRow * m_columnSize + currentColumn ];
        switch( GetPositionalData( currentRow, currentColumn ) )
        {
            case MAZE_DATA_TYPE::EXIT:
            {
                std::cout << "Num Steps: " << numSteps << std::endl;
                return distance;
            } break;
            case MAZE_DATA_TYPE::BLOCKED:
            {
                mazeQueue.pop();
                continue;
            }
            default:
            {
            } break;
        }

        ++distance;
        if ( ( currentRow > 0 ) && ( distanceMap.find( ( currentRow - 1 ) * m_columnSize + currentColumn )  == distanceMap.cend() ) )
        {
            distanceMap[ ( currentRow - 1 ) * m_columnSize + currentColumn ] = distance;
            mazeQueue.push( { currentRow - 1, currentColumn } );
        }
        if ( ( currentColumn > 0 ) && ( distanceMap.find( currentRow * m_columnSize + ( currentColumn - 1 ) ) == distanceMap.cend() ) )
        {
            distanceMap[ currentRow * m_columnSize + ( currentColumn - 1 ) ] = distance;
            mazeQueue.push( { currentRow, currentColumn - 1 } );
        }
        if ( ( currentRow + 1 < m_rowSize ) && ( distanceMap.find( ( currentRow + 1 ) * m_columnSize + currentColumn ) == distanceMap.cend() ) )
        {
            distanceMap[ ( currentRow + 1 ) * m_columnSize + currentColumn ] = distance;
            mazeQueue.push( { currentRow + 1, currentColumn } );
        }
        if ( ( currentColumn + 1 < m_columnSize ) && ( distanceMap.find( currentRow * m_columnSize + ( currentColumn + 1 ) ) == distanceMap.cend() ) )
        {
            distanceMap[currentRow * m_columnSize + ( currentColumn + 1 )] = distance;
            mazeQueue.push({ currentRow, currentColumn + 1 });
        }

        mazeQueue.pop();
    }

    std::cout << "Num Steps: " << numSteps << std::endl;
    return -1;
}

int Maze::MinimumNumberOfSpacesDFS( const size_t start_row, const size_t start_column ) const
{
    if ( start_row > m_rowSize || start_column > m_columnSize )
    {
        return -1;
    }
    if ( GetPositionalData( start_row, start_column ) == MAZE_DATA_TYPE::EXIT )
    {
        return 0;
    }

    std::vector<std::vector<bool>> traversed( m_rowSize, std::vector<bool>( m_columnSize, false ) );

    traversed[start_column][start_row] = true;
    
    std::vector<int> smallest_path;

    int numSteps = 0;

    const int northPath = DepthFirstRecursion( start_row - 1, start_column, traversed, 0, numSteps );
    if ( northPath > 0 )
    {
        smallest_path.push_back( northPath );
    }
    const int westPath = DepthFirstRecursion( start_row, start_column - 1, traversed, 0, numSteps );
    if ( westPath > 0 )
    {
        smallest_path.push_back( westPath );
    }

    const int eastPath = DepthFirstRecursion( start_row, start_column + 1, traversed, 0, numSteps );
    if ( eastPath > 0 )
    {
        smallest_path.push_back( eastPath );
    }

    const int southPath = DepthFirstRecursion( start_row + 1, start_column, traversed, 0, numSteps );
    if ( southPath > 0 )
    {
        smallest_path.push_back( southPath );
    }

    std::cout << "Num Steps: " << numSteps << std::endl;

    if ( smallest_path.empty() )
    {
        return -1;
    }

    return *std::min_element( smallest_path.cbegin(), smallest_path.cend() );
}

int Maze::DepthFirstRecursion( const size_t current_row, const size_t current_column, std::vector<std::vector<bool>>& traversed, const int nodesInPath, int & numSteps ) const
{
    if ( current_row >= m_rowSize || current_column >= m_columnSize )
    {
        return -1;
    }

    if ( traversed[current_row][current_column] )
    {
        return -1;
    }

    ++numSteps;

    switch ( GetPositionalData( current_row, current_column ) )
    {
        case MAZE_DATA_TYPE::BLOCKED:
        {
            return -1;
        } break;
        case MAZE_DATA_TYPE::EXIT:
        {
            return nodesInPath + 1;
        } break;
        default:
        {
        } break;
    }

    traversed[current_row][current_column] = true;

    std::vector<int> smallest_path;

    const int northPath = DepthFirstRecursion( current_row - 1, current_column, traversed, nodesInPath + 1, numSteps );
    if ( northPath > 0 )
    {
        smallest_path.push_back( northPath );
    }
    const int westPath = DepthFirstRecursion( current_row, current_column - 1, traversed, nodesInPath + 1, numSteps );
    if ( westPath > 0 )
    {
        smallest_path.push_back( westPath );
    }

    const int eastPath = DepthFirstRecursion( current_row, current_column + 1, traversed, nodesInPath + 1, numSteps );
    if ( eastPath > 0 )
    {
        smallest_path.push_back( eastPath );
    }

    const int southPath = DepthFirstRecursion( current_row + 1, current_column, traversed, nodesInPath + 1, numSteps );
    if ( southPath > 0 )
    {
        smallest_path.push_back( southPath );
    }
    
    traversed[current_row][current_column] = false;

    if ( smallest_path.empty() )
    {
        return -1;
    }

    return *std::min_element( smallest_path.cbegin(), smallest_path.cend() );
}

size_t Maze::GetRowSize() const
{
    return m_rowSize;
}

size_t Maze::GetColumnSize() const
{
    return m_columnSize;
}

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

Maze::MAZE_DATA_TYPE Maze::GetPositionalData( const size_t row, const size_t column ) const
{
    if ( row >= m_rowSize || column >= m_columnSize )
    {
        throw std::runtime_error( "Cannot get positional data where row or column exceeds size." );
    }

    return m_mapData[row][column];
}

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

void Maze::ClearMaze()
{
    m_rowSize = 0;
    m_columnSize = 0;
    m_mapData.clear();
}

void Maze::ReallocateMaze( const size_t rows, const size_t columns )
{
    m_mapData.resize( rows );
    for ( size_t row = 0; row < rows; ++row )
    {
        m_mapData[row].resize( columns );
    }
}
