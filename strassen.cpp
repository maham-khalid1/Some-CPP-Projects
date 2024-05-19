#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template <typename T>
vector<vector<T>> addMatrix(const vector<vector<T>> &A, const vector<vector<T>> &B)
{
    int n = A.size();
    int m = A[0].size();
    vector<vector<T>> C(n, vector<T>(m));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

template <typename T>
vector<vector<T>> subtractMatrix(const vector<vector<T>> &A, const vector<vector<T>> &B)
{
    int n = A.size();
    int m = A[0].size();
    vector<vector<T>> C(n, vector<T>(m));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            C[i][j] = A[i][j] - B[i][j];
        }
    }
    return C;
}


//to pad a matrix with zeros up to a square matrix of specific dimension size
template <typename T> 
vector<vector<T>> paddedMat(const vector<vector<T>> &matrix, int dim)
{
    vector<vector<T>> padded(dim, vector<T>(dim, 0.0));
    int originaldim = matrix.size();
    for (int row = 0; row < originaldim; ++row) {
        for (int col = 0; col < originaldim; ++col){
            padded[row][col] = matrix[row][col];
        }
    }
    return padded;
}

template <typename T> //returns an unpadded matrix of size nrows x ncols
vector<vector<T>> unpaddedMat(const vector<vector<T>> &matrix, int dim)
{
    vector<vector<T>> unpadded(dim, vector<T>(dim));
    for (int row = 0; row < dim; ++row) {
        for (int col = 0; col < dim; ++col){
            unpadded[row][col] = matrix[row][col];
        }
    }
    return unpadded;

}

template <typename T>
vector<vector<T>> strassenMultiply(const vector<vector<T>> &A, const vector<vector<T>> &B)
{   

    std::vector<std::vector<T>> AA;
    std::vector<std::vector<T>> BB;
    int matDim = A.size();
    int dim1;

    // if we have an odd sized matrix, we pad it by 0's
    if (A.size()%2 !=0 && A.size() > 1)
    {
        dim1 = A.size()+1;
        AA = paddedMat(A, dim1);
        BB = paddedMat(B, dim1);
    }
    // if not, we just use the same matrices as before
    else{
        AA = A;
        BB = B;
    }
    int dim = AA.size();

    // base case
    // if (dim == 1){
    //     vector<vector<T>> CC (1, vector<T>(1));
    //     CC[0][0] = AA[0][0] * BB[0][0]; //AA , BB
    //     return CC;
    // }

    // Once we hit an 8x8 matrix size we revert to regular multiplication just to speed this up a bit
    if (dim <= 8)
    {
        vector<vector<T>> CC(dim, vector <T> (dim));
        for (int k = 0; k < dim; ++k){
            for (int i = 0; i < dim; ++i){
                for (int j = 0; j < dim; ++j){
                    CC[i][j] += AA[i][k]*BB[k][j];
                }
            }
        }
        return CC;
    }

    //creating smaller blocks of each matrix
    int newdim = dim/2;
    vector<vector<T>> A11 (newdim, vector<T>(newdim));
    vector<vector<T>> A12 (newdim, vector<T>(newdim));
    vector<vector<T>> A21 (newdim, vector<T>(newdim));
    vector<vector<T>> A22 (newdim, vector<T>(newdim));
    vector<vector<T>> B11 (newdim, vector<T>(newdim));
    vector<vector<T>> B12 (newdim, vector<T>(newdim));
    vector<vector<T>> B21 (newdim, vector<T>(newdim));
    vector<vector<T>> B22 (newdim, vector<T>(newdim));

    for (int i = 0; i < newdim; i++){
        for (int j = 0; j < newdim; j++){
            A11[i][j] = AA[i][j];
            A12[i][j] = AA[i][newdim + j];
            A21[i][j] = AA[newdim + i][j];
            A22[i][j] = AA[newdim + i][newdim + j];
            B11[i][j] = BB[i][j];
            B12[i][j] = BB[i][newdim + j];
            B21[i][j] = BB[newdim + i][j];
            B22[i][j] = BB[newdim + i][newdim + j];
        }
    }

    vector<vector<T>> P1 = strassenMultiply(addMatrix(A11, A22), addMatrix(B11, B22));
    vector<vector<T>> P2 = strassenMultiply(addMatrix(A21, A22), B11);
    vector<vector<T>> P3 = strassenMultiply(A11, subtractMatrix(B12, B22));
    vector<vector<T>> P4 = strassenMultiply(A22, subtractMatrix(B21, B11));
    vector<vector<T>> P5 = strassenMultiply(addMatrix(A11, A12), B22);
    vector<vector<T>> P6 = strassenMultiply(subtractMatrix(A21, A11), addMatrix(B11, B12));
    vector<vector<T>> P7 = strassenMultiply(subtractMatrix(A12,A22), addMatrix(B21, B22));

    vector<vector<T>> C11 = addMatrix(subtractMatrix(addMatrix(P1, P4), P5), P7);
    vector<vector<T>> C12 = addMatrix(P3, P5);
    vector<vector<T>> C21 = addMatrix(P2, P4);
    vector<vector<T>> C22 = addMatrix(subtractMatrix(addMatrix(P1, P3), P2), P6);

    vector<vector<T>> CC (dim, vector<T>(dim));
    for (int i = 0; i < dim/2; i++){
        for (int j = 0; j < dim/2; j++){
            CC[i][j] = C11[i][j];
            CC[i][j + dim/2] = C12[i][j];
            CC[i + dim/2][j] = C21[i][j];
            CC[i + dim/2][j + dim/2] = C22[i][j];
        }
    }
    
    // resizing back to original size
    CC = unpaddedMat(CC, matDim);
    return CC; 
}

template <typename T>
void printMatrix(const vector<vector<T>> &matrix)
{
    int n = matrix.size();
    int m = matrix[0].size();
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

// int
template vector<vector<int>> addMatrix<int>(const vector<vector<int>> &A, const vector<vector<int>> &B);
template vector<vector<int>> subtractMatrix<int>(const vector<vector<int>> &A, const vector<vector<int>> &B);
template vector<vector<int>> strassenMultiply<int>(const vector<vector<int>> &A, const vector<vector<int>> &B);
template vector<vector<int>> paddedMat(const vector<vector<int>> &A, int dim);
template vector<vector<int>> unpaddedMat(const vector<vector<int>> &matrix, int dim);
template void printMatrix<int>(const vector<vector<int>> &matrix);
// double
template vector<vector<double>> addMatrix<double>(const vector<vector<double>> &A, const vector<vector<double>> &B);
template vector<vector<double>> subtractMatrix<double>(const vector<vector<double>> &A, const vector<vector<double>> &B);
template vector<vector<double>> strassenMultiply<double>(const vector<vector<double>> &A, const vector<vector<double>> &B);
template vector<vector<double>> paddedMat(const vector<vector<double>> &A, int dim);
template vector<vector<double>> unpaddedMat(const vector<vector<double>> &matrix, int dim);
template void printMatrix<double>(const vector<vector<double>> &matrix);
