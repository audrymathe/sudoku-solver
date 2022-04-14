#include <iostream>
#include <vector>
#include <sstream>
#include <forward_list>
#include <cmath>
#include <chrono>

using namespace std;

//Dancing link algorithm used.

int gridSi1ze ;

gridSize =0; // we are going to sum the size of the grid the user inputs hence we are starting from 0, 
//as the user inputs the size may change.

vector<vector<int>> grid;
forward_list<int> finalsolution; 

bool isSolved = false; // check if there is a finalsolution or not

class SudokuGame

{
private:
    class header; 
    class node
    {
    public:
        int row;
        header *head;
        node *up, *down, *left, *right;
    };
 
 //this is how the node and header for the dancing links must be like
    class header : public node
    {
    public:
        int sum;
        header *left, *right;
    };

    int n, numRows, numCols;   
    header *rootNode; //

    vector<vector<node>> rows;
    vector<header> cols;

public:
    SudokuGame
    ()
    {
        n = gridSize;             
        numCols = (n * n * 4) + 1; 
        numRows = n * n * n;       


        cols = vector<header>(numCols, header());                      //gives columns
        rows = vector<vector<node>>(numRows, vector<node>(4, node())); // gives  rows
       
        rootNode = &(cols[numCols - 1]); 

        for (int a = 0; a < numCols; a++)
        {
            

            cols[a].up = &cols[a];
            cols[a].down = &cols[a];
            cols[a].left = &(cols[(a + numCols - 1) % numCols]);
            cols[a].right = &(cols[(a + 1) % numCols]);
            cols[a].sum = 0; 
        }

        int n2 = n * n;         
        int boxWidth = sqrt(n); // taking the size of the divided square blocks

    
        for (int r = 0; r < numRows; r++) 
        {
            int colInd, currRow = r / (n * n), currCol = (r / n) % n, otherv = r % n;
         

            if (grid[currRow][currCol] != 0 && grid[currRow][currCol] != otherv + 1)
                continue;

            for (int a = 0; a < 4; a++)
            {
                int startCol = a * n2; 

                switch (a)
                {
                case 0: 
                    colInd = r / n;
                    break;
                case 1: 
                    colInd = n2 + (currRow * n) + otherv;
                    break;
                case 2: 
                    colInd = startCol + ((r / n) % n) * n + otherv;
                    break;
                case 3: 
                    colInd = startCol + (boxWidth * (currRow / boxWidth) + (currCol / boxWidth)) * n + otherv;
                    break;

                }

           

                rows[r][a].row = r;
                rows[r][a].down = &cols[colInd];

               
                rows[r][a].right = &(rows[r][(a + 1) % 4]);
                rows[r][a].left = &(rows[r][(a + 3) % 4]);
                rows[r][a].up = cols[colInd].up;   
                rows[r][a].head = &cols[colInd];   
                rows[r][a].up->down = &rows[r][a]; 
                rows[r][a].down = &cols[colInd];   

                cols[colInd].up->down = &(rows[r][a]); 
                cols[colInd].up = &(rows[r][a]);
                cols[colInd].sum++;
            }
        }
    };

    void print()
    {
        int currRow, currCol, otherv;

       

        for (int &r : finalsolution) 
        {
           
            currCol = (r / n) % n;
            currRow = r / (n * n);
            otherv = r % n + 1;

            grid[currRow][currCol] = otherv; 
        }

        for (int a = 0; a < n; a++) 
        {
            for (int b = 0; b < n; b++)
            {
                if (grid[a][b] >= 10)
                    cout << char(grid[a][b] + 55);
                else
                    cout << grid[a][b];

                if (b != n - 1)
                    cout << " ";
            }

            cout << endl;
        }
    }

    void solve()
    {
        if (rootNode->right == rootNode) // Means we've found a finalsolution
        {
            isSolved = true;
            print();
            return;
        }

        header *bestCol = rootNode->right;

        for (header *a = rootNode->right; a != rootNode; a = a->right)
        {
            if (a->sum < bestCol->sum) 
                bestCol = a;

           
        }

        cover(bestCol); 

        for (node *currRow = bestCol->down; currRow != bestCol; currRow = currRow->down)
        {
          
            finalsolution.push_front(currRow->row); // pushing node to the finalsolution set

            for (node *currCol = currRow->right; currCol != currRow; currCol = currCol->right) 
                cover(currCol->head);                                                          

            solve();             
            finalsolution.pop_front();

            for (node *currCol = currRow->left; currCol != currRow; currCol = currCol->left) 
                uncover(currCol->head);
        }

        uncover(bestCol);
    }

private:
    void cover(header *col)
    {
        col->right->left = col->left;  //lft right col are now pointing to each other
        col->left->right = col->right; // delete col from set of cols

        for (node *r = col->down; r != col; r = r->down) // looping through all the nodes in a col
        {
            for (node *a = r->right; a != r; a = a->right) // looping through all the nodes that are on the fight of the ref node
            {
                a->up->down = a->down; //up,down node to ref node point to each other
                a->down->up = a->up; //delete
                a->head->sum--; // so we keep decrementing cols if they are affected by those node deletions
            }
        }
    }
// opposite of covering if we covered left to right we use the opposite direction to uncover
//down to up when covering uncover will then take opposite directions . make sure to do that
    void uncover(header *col)
    {
        
        for (node *r = col->up; r != col; r = r->up) 
        {
            for (node *a = r->left; a != r; a = a->left) 
            {
                a->up->down = a; 
                a->down->up = a;
                a->head->sum++; 
            }
        }

        col->right->left = col; 
        col->left->right = col;
    }
};

int main()
{
    string input;

 
    while (getline(cin, input))
    {
      // first deal with the first line to count how many cols are there

      // this method allows to take input dynamically

        stringstream fistStr(input);
        vector<int> inputNos;
        string currentString;

        while (getline(fistStr, currentString, ' '))
            if (int(currentString[0]) >= 65 && int(currentString[0]) <= 90) // hexadecimal decimal conversions.
                numbers.push_back(currentString[0] - 55);
            else
                numbers.push_back(stoi(currentString));

        grid.push_back(numbers);
    }

    gridSize = grid.size(); 

   

    SudokuGame
     sudGrid;
    sudGrid.solve();

    if (!isSolved)
        cout << "No Solution" << endl;

   
    return 0;
}
