Interesting problems that were worked on for UW AMATH 583 - High Performance Scientific Computing. 

Includes: 
1. Strassen Matrix Multiplication Algorithm
   - After intially having a matrix multiplication computation time of about 2.5 minutes for large matrices, some adjustments were made and I brought the computation time down to ~4 seconds for two 512 X 512 sized matrices.
2. Multi-threaded Game of Life implementation
   - Since this is a shared memory model, no need to implement border exchanges of any sort which would be necessary for MPI. This code file includes a function to update the partial grids for the number of threads, and then a function to use threading to update the entire grid.
3. Threaded Elevator Simulator (coming soon)
