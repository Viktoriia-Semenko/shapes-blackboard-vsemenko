#include <iostream>
#include <vector>
#include <memory>
using namespace std;

const int BOARD_WIDTH = 80;
const int BOARD_HEIGHT = 25;

class Shape {
public:
    virtual void draw(vector<vector<char>>& grid) const = 0;
    virtual ~Shape() = default;
};

class Triangle : public Shape {
    int x, y, height;
public:
    Triangle(int x, int y, int height) : x(x), y(y), height(height) {}

    void draw(vector<vector<char>>& grid) const override {
        if (height <= 0) return;
        for (int i = 0; i < height; ++i) {
            int left_most = x - i;
            int right_most = x + i;
            int position_y = y + i;


            if (position_y < BOARD_HEIGHT) {
                if (left_most >= 0 && left_most < BOARD_WIDTH)
                    grid[position_y][left_most] = '*';
                if (right_most >= 0 && right_most < BOARD_WIDTH && left_most != right_most)
                    grid[position_y][right_most] = '*';
            }
        }

        for (int j = 0; j < 2 * height - 1; ++j) {
            int baseX = x - height + 1 + j;
            int baseY = y + height - 1;
            if (baseX >= 0 && baseX < BOARD_WIDTH && baseY < BOARD_HEIGHT)
                grid[baseY][baseX] = '*';
        }
    }
};

class Rectangle : public Shape {
    int x, y, width, height;
public:
    Rectangle(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}

    void draw(vector<vector<char>>& grid) const override {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int posX = x + j;
                int posY = y + i;
                if (posX >= 0 && posX < BOARD_WIDTH && posY >= 0 && posY < BOARD_HEIGHT) {
                    if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                        grid[posY][posX] = '*';
                    }
                }
            }
        }
    }
};

class Board {
    vector<vector<char>> grid;
    vector<unique_ptr<Shape>> shapes;
public:
    Board() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}

    void addShape(unique_ptr<Shape> shape) {
        shapes.push_back(move(shape));
    }

    void clearBoard() {
        shapes.clear();
    }

    void drawBoard() {

        grid = vector<vector<char>>(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' '));

        for (const auto& shape : shapes) {
            shape->draw(grid);
        }
        // border for board
        cout << "-";
        for (int i = 0; i < BOARD_WIDTH; ++i) {
            cout << "-";
        }
        cout << "-\n";

        for (const auto& row : grid) {
            cout << "|";
            for (char c : row) {
                cout << c;
            }
            cout << "|\n";
        }

        cout << "-";
        for (int i = 0; i < BOARD_WIDTH; ++i) {
            cout << "-";
        }
        cout << "-\n";
    }
};

int main() {
    Board board;

    board.addShape(make_unique<Triangle>(10, 1, 5));
    board.addShape(make_unique<Rectangle>(10, 5, 10, 6));

    board.drawBoard();

    return 0;
}
