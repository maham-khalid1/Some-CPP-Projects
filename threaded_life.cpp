#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cstdio>

// Threaded Conway Game of Life on a Square Grid

//this function will update each partial mini square created for each of the threads
void updatePartial(std::vector<std::vector<bool>> &grid, std::vector<std::vector<bool>> &newGrid, 
                   int gridSize, int startRow, int endRow, int startCol, int endCol)
{
    for (int i = startRow; i < endRow; ++i) {
        for (int j = startCol; j < endCol; ++j) {
            int aliveNeighbors = 0;
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    if (x == 0 && y == 0) continue;
                    int neighborX = i + x;
                    int neighborY = j + y;
                    // if (neighborX >= 0 && neighborX < endRow && neighborY >= 0 && neighborY < endCol) {
                    // the upper one is bad actually, each thread seems to become a mini grid(?) and then neighboring cells aren't considered
                    if (neighborX >= 0 && neighborX < gridSize && neighborY >= 0 && neighborY < gridSize) {
                        if (grid[neighborX][neighborY]) {
                            aliveNeighbors++;
                        }
                    }
                }
            }
            if (grid[i][j]) {
                if (aliveNeighbors < 2 || aliveNeighbors > 3) {
                    newGrid[i][j] = false;
                } else {
                    newGrid[i][j] = true;
                }
            } else {
                if (aliveNeighbors == 3) {
                    newGrid[i][j] = true;
                }
            }
        }
    }
}


// function to update the grid in each of the threads
void updateGrid(std::vector<std::vector<bool>> &grid, int gridSize, int numThreads)
{
    std::vector<std::vector<bool>> newGrid(gridSize, std::vector<bool>(gridSize, false));
    std::vector<std::thread> threads;

    int partitions = int(sqrt(numThreads)); // How many blocks in each direction
    int gridPartition = gridSize / partitions; // The size of each of the blocks

    for (int i = 0; i < partitions; i++)
    {
        for (int j = 0; j < partitions; j++)
        {
            int startRow = gridPartition * i;
            int endRow = (i == partitions - 1) ? gridSize : gridPartition * (i + 1);
            int startCol = gridPartition * j;
            int endCol = (j == partitions - 1) ? gridSize : gridPartition * (j + 1);
            threads.emplace_back(updatePartial, std::ref(grid), std::ref(newGrid), 
                                 gridSize, startRow, endRow, startCol, endCol); 
        }
    }

    // joining the threads in row major order since that's kind of how each smaller partition was built above
    for (int i = 0; i < partitions; i++)
    {
        for (int j = 0; j < partitions; j++)
        {
            threads[i + j * partitions].join();
        }
    }
    grid = newGrid;
    
}
