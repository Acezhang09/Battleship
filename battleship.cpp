// clang++ -std=c++17 -Wall -Werror -Wextra -Wpedantic -g3 -o testingstuff testingstuff.cpp
// ./battleship
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <time.h>
#include <map>

using namespace std;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"

typedef vector< string > stringrow;
typedef vector < stringrow > stringmatrix;

const int GRID_LENGTH { 10 };
const int GRID_WIDTH { 10 };

const string ANSI_START { "\033[" };
const string START_COLOUR_PREFIX {"1;"};
const string START_COLOUR_SUFFIX {"m"};
const string STOP_COLOUR  {"\033[0m"};

vector<string> letter_coords = {"A","B","C","D","E","F","G","H","I","J"};
vector<string> number_coords = {" ","1","2","3","4","5","6","7","8","9","10"};

bool isSunk = false;



struct Point {
    int row;
    int col; 
};

struct Ship{
    int length;
    int horizontal;
    int going_up_or_right;
    Point starting_point;

};

typedef vector < Ship > shipvector;
typedef vector < Point > pointvector;
typedef vector < vector < Point > > ;


auto GenerateMatrix() -> stringmatrix {
    // Generates the Grid
    stringmatrix grid;
    for (int i = 0; i < GRID_LENGTH+1; i++) {
        stringrow row;
        for (int j = 0; j < GRID_WIDTH+1; j++){
            if (i == 0){
                row.push_back(number_coords[j]);
            }
            else {
                if (j == 0){
                    row.push_back(letter_coords[i-1]);
                }
                else {
                    row.push_back("-");
                }
            }
        }
        grid.push_back(row);
    }
    return grid;
}

auto PrintMatrix(stringmatrix grid, pointvector hits, pointvector misses) -> void {
    for (int i = 0; i < int(hits.size()); i++){
            grid[hits[i].row][hits[i].col] = "\033[1m\033[31mX\033[0m";      
    }
    for (int i = 0; i < int(misses.size()); i++){
            grid[misses[i].row][misses[i].col] = "\033[34mO\033[0m";      
    }
    // Prints the grid upon every iteration
    for (int i = 0; i < int(grid.size()); i++) {
        for (int j = 0; j < int(grid[0].size()); j++){
            cout << grid[i][j] << "\t";
        }
        cout << endl;
    }
}

// KEY EDGE CASES: SHIPS CANNOT INTERSECT OR GO OFF THE GRID!!!
auto GenerateShips() -> shipvector {
    shipvector ship_data;
    
    // Randomizes ship positions everytime game is played
    srand(time(NULL));
    
    struct Ship carrier;
    carrier.length = 5;
    carrier.horizontal = rand() % 2;
    carrier.going_up_or_right = rand() % 2;
    carrier.starting_point.row = rand() % 10 + 1;
    carrier.starting_point.col = rand() % 10 + 1;
    ship_data.push_back(carrier);
    // Push back from largest ship to smallest for convenience in next method

    struct Ship battleship;
    battleship.length = 4;
    battleship.horizontal = rand() % 2;
    battleship.going_up_or_right = rand() % 2;
    battleship.starting_point.row = rand() % 10 + 1;
    battleship.starting_point.col = rand() % 10 + 1;
    ship_data.push_back(battleship);

    struct Ship cruiser;
    cruiser.length = 3;
    cruiser.horizontal = rand() % 2;
    cruiser.going_up_or_right = rand() % 2;
    cruiser.starting_point.row = rand() % 10 + 1;
    cruiser.starting_point.col = rand() % 10 + 1;
    ship_data.push_back(cruiser);

    struct Ship submarine;
    submarine.length = 3;
    submarine.horizontal = rand() % 2;
    submarine.going_up_or_right = rand() % 2;
    submarine.starting_point.row = rand() % 10 + 1;
    submarine.starting_point.col = rand() % 10 + 1;
    ship_data.push_back(submarine);

    struct Ship destroyer;
    destroyer.length = 2;
    destroyer.horizontal = rand() % 2;
    destroyer.going_up_or_right = rand() % 2;
    destroyer.starting_point.row = rand() % 10 + 1;
    destroyer.starting_point.col = rand() % 10 + 1;
    ship_data.push_back(destroyer);
    return ship_data;
}

auto IsPossible(pointvector hitlist, pointvector all) -> bool {
    bool is_possible = true;
    for (int k = 0; k < int(hitlist.size()); k++) {
        // Test to see if ship goes off the grid
        if (hitlist[k].row > 10 || hitlist[k].row < 1 || hitlist[k].col > 10 || hitlist[k].col < 1){
            is_possible = false;
        }
        // Checks to see if ship intersects with any other ships
        for (int l = 0; l < int(all.size()); l++){
            if (hitlist[k].row == all[l].row && hitlist[k].col == all[l].col){
                is_possible = false;
            }
            
        }
    }
    return is_possible;
}
int hitsRemaining;
auto ObtainShipCoordinates(shipvector ship_data) -> pointvector {
    pointvector all_ship_coords;
    
    for (int i = 0; i < int(ship_data.size()); i++){
        bool possible = false;
        while (possible == false){
            pointvector on_ship;
            if (ship_data[i].horizontal == 1){
                if(ship_data[i].going_up_or_right == 1){ // Ship is horizontal and extending right
                    for (int j = 0; j < ship_data[i].length; j++){
                        Point hit_point = {ship_data[i].starting_point.row, int(ship_data[i].starting_point.col + j)};
                        on_ship.push_back(hit_point);
                    }
                }
                else{
                    for (int j = 0; j < ship_data[i].length; j++){ // Ship is horizontal and extending left
                        Point hit_point = {ship_data[i].starting_point.row, int(ship_data[i].starting_point.col - j)};
                        on_ship.push_back(hit_point);
                    }
                }
            }
            else {
                if(ship_data[i].going_up_or_right == 1){ // Ship is vertical and extending up
                    for (int j = 0; j < ship_data[i].length; j++){
                        Point hit_point = {int(ship_data[i].starting_point.row - j), ship_data[i].starting_point.col};
                        on_ship.push_back(hit_point);
                    }
                }
                else{
                    for (int j = 0; j < ship_data[i].length; j++){ // Ship is vertical and extending down
                        Point hit_point = {int(ship_data[i].starting_point.row + j), ship_data[i].starting_point.col};
                        on_ship.push_back(hit_point);
                    }
                }
    
            }
            possible = IsPossible(on_ship, all_ship_coords);
            if (possible == false){
                int reset_row = rand() % 10 + 1;
                int reset_col = rand() % 10 + 1;
                ship_data[i].starting_point.row = reset_row;
                ship_data[i].starting_point.col = reset_col;
            }
            else{
                for (int i = 0; i < int(on_ship.size()); i++){
                    all_ship_coords.push_back(on_ship[i]);
                }
            }
        }
    }
    
    return all_ship_coords;
    
}

int successfulGuesses = 0;
int totalGuesses = 0;
pointvector chosenpoints;
auto UserInputCoordinates() -> Point {
    Point userguess;
    Point empty = {-1,-1};
    string row_string;
    string col_string;
    int row_int;
    int col_int;
    map<string, int> input = {{"A", 1}, {"B", 2},{"C", 3},{"D", 4},{"E", 5},{"F", 6},{"G", 7},{"H", 8},{"I", 9},{"J", 10}};


    cout << "Enter the horizontal coordinate of your missile (A-J): "; // Type a number and press enter
    cin >>  row_string; // Get user input from the keyboard
    row_int = input[row_string];

    cout << "Enter vertical coordinate of your missile (1-10): "; // Type a letter and press enter
    cin >>  col_string ;
    cout << endl; // Get user input from the keyboard
    
    bool isNumber = true;
    for (int i = 0; i < int(col_string.length()); i++){
        if (isdigit(col_string[i]) == false){
         isNumber = false;
         cout<<"Enter a valid entry"<<endl;
        return empty;
        }else{
            col_int = stoi(col_string);
        }
    }
    
    bool already_chosen = false;
         
    
    for (int i = 0; i < int(chosenpoints.size()); i++){
        if (chosenpoints[i].row == row_int && chosenpoints[i].col == col_int){
            already_chosen = true;
        }
    }

    if(int(row_int)>10 || int(row_int)<1 || col_int>10 || col_int<1){
        cout<<"Enter a valid entry"<<endl;
        return empty;
    }
    else if (already_chosen == true){
        cout <<"Coordinate already chosen, please try again\n"<<endl;
        return empty;
    }
    else{
        userguess.row = row_int;
        userguess.col = col_int;
        chosenpoints.push_back(userguess);
    }

    return userguess;

}

auto checkHit (Point userguess, pointvector ship_coords) -> bool{

    bool doesHit = true;

    //check if the guess has been done before
    //chevking the guess has a bug and idk what so we can look at it together
     
       //take in user row and col input, go through all_ship_coords to find a match
        for (int j = 0; j<int(ship_coords.size());j++){
            if(userguess.row == ship_coords[j].row && userguess.col == ship_coords[j].col){
                //cout<<"Hit"<<endl;
                doesHit = true;
                
                break;
            }else{
                doesHit = false;
                //cout<<"Miss"<<endl;
            }
        }
    
   
     return doesHit;
    
}

int main(){
    stringmatrix grid = GenerateMatrix();
    shipvector ship_data = GenerateShips();
    pointvector all_ship_coords = ObtainShipCoordinates(ship_data);
    hitsRemaining = int(all_ship_coords.size());
    pointvector hits;
    pointvector misses;
    PrintMatrix(grid,hits,misses);
    cout<<"Welcome to Battleship! Try to hit and sink all the ships!"<<endl;
    while(isSunk == false){
        Point userguess = UserInputCoordinates();
        if (userguess.row == -1 && userguess.col == -1){
            continue;
        }
        bool hit = checkHit(userguess, all_ship_coords);
        if (hit == true){
            hits.push_back(userguess);
            hitsRemaining = hitsRemaining - 1;
            successfulGuesses = successfulGuesses +1;
            totalGuesses = totalGuesses + 1;
            if (hitsRemaining == 0){
                isSunk = true;
                break;
            }
        }else{
            totalGuesses = totalGuesses +1;
            misses.push_back(userguess);
        }
        PrintMatrix(grid,hits,misses);
        cout<<"Remaining hits:"<<hitsRemaining<<"\t"<<"\t"<<"Successfull Guesses:"<<successfulGuesses<<"\t"<<"\t"<<"Total Guesses:"<<totalGuesses<<endl;
        }
        if (isSunk == true){
            cout<<"\033[33mCongrats you have sunk all the ships!\033[0m"<<endl;
        }

}






