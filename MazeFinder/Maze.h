#ifndef __MAZE_H
#define __MAZE_H

#include <string>
#include <vector>

class Maze
{
    public:
        Maze() = default;
        ~Maze() = default;
        bool LoadMaze( const std::string & filename );
        bool MazeLoaded() const;
        int MinimumNumberOfSpacesBFS( const size_t start_row, const size_t start_column ) const;
        int MinimumNumberOfSpacesDFS( const size_t start_row, const size_t start_column ) const;
        size_t GetRowSize() const;
        size_t GetColumnSize() const;
        void PrintMaze() const;

    private:
        enum class MAZE_DATA_TYPE : char
        {
            CLEAR=0,
            BLOCKED=1,
            EXIT=2
        };

        MAZE_DATA_TYPE GetPositionalData( const size_t row, const size_t column ) const;
        bool SetPositionalData( const size_t row, const size_t column, const char value );
        void ClearMaze();
        void ReallocateMaze( const size_t rows, const size_t columns );

        int DepthFirstRecursion( const size_t current_row, const size_t current_column, std::vector<std::vector<bool>> & traversed, const int nodesInPath, int& numSteps) const;

        size_t m_rowSize{ 0 };
        size_t m_columnSize{ 0 };
        std::vector<std::vector<MAZE_DATA_TYPE>> m_mapData;
};

#endif // __MAZE_H