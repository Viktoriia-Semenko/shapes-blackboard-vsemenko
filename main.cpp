#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

const int BOARD_WIDTH = 80;
const int BOARD_HEIGHT = 25;

class Shape {
protected:
    int x, y;
public:
    Shape(int x, int y) : x(x), y(y) {}
    virtual void draw(vector<vector<char>>& grid) const = 0;
    virtual string get_shapes_info() const = 0;
    virtual bool is_equal(const shared_ptr<Shape>& other) const = 0;
    virtual ~Shape() = default;
};

class Triangle : public Shape {
    int height;
public:
    Triangle(int x, int y, int height) : Shape(x, y), height(height) {}

    void draw(vector<vector<char>>& grid) const override {
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

    string get_shapes_info() const override {
        return "triangle " + to_string(x) + " " + to_string(y) + " " + to_string(height);
    }

    bool is_equal(const shared_ptr<Shape>& other) const override {
        auto other_triangle = dynamic_pointer_cast<Triangle>(other);
        return other_triangle != nullptr && x == other_triangle->x && y == other_triangle->y && height == other_triangle->height;
    }
};

class Rectangle : public Shape {
    int width, height;
public:
    Rectangle(int x, int y, int width, int height) : Shape(x, y), width(width), height(height) {}

    void draw(vector<vector<char>>& grid) const override {
        for (int i = 0; i < width; ++i) {
            if (x + i < BOARD_WIDTH) {
                if (y >= 0 && y < BOARD_HEIGHT) grid[y][x + i] = '*';
                if (y + height - 1 >= 0 && y + height - 1 < BOARD_HEIGHT) grid[y + height - 1][x + i] = '*';
            }
        }
        for (int i = 0; i < height; ++i) {
            if (y + i < BOARD_HEIGHT) {
                if (x >= 0 && x < BOARD_WIDTH) grid[y + i][x] = '*';
                if (x + width - 1 >= 0 && x + width - 1 < BOARD_WIDTH) grid[y + i][x + width - 1] = '*';
            }
        }
    }

    string get_shapes_info() const override {
        return "rectangle " + to_string(x) + " " + to_string(y) + " " + to_string(width) + " " + to_string(height);
    }
    bool is_equal(const shared_ptr<Shape>& other) const override {
        auto other_rectangle = dynamic_pointer_cast<Rectangle>(other);
        return other_rectangle != nullptr && x == other_rectangle->x && y == other_rectangle->y && width == other_rectangle->width && height == other_rectangle->height;
    }
};

class Circle : public Shape {
private:
    int radius;
public:
    Circle(int x, int y, int radius) : Shape(x, y), radius(radius) {}

    void draw(vector<vector<char>>& grid) const override {

        for (int i = -radius; i <= radius; ++i) {
            for (int j = -radius; j <= radius; ++j) {

                int distSquared = i * i + j * j;
                int radiusSquared = radius * radius;

                if (distSquared >= radiusSquared - radius && distSquared <= radiusSquared + radius) {
                    int posX = x + j;
                    int posY = y + i;
                    if (posX >= 0 && posX < BOARD_WIDTH && posY >= 0 && posY < BOARD_HEIGHT)
                        grid[posY][posX] = '*';
                }
            }
        }
    }

    string get_shapes_info() const override {
        return "circle " + to_string(x) + " " + to_string(y) + " " + to_string(radius);
    }
    bool is_equal(const shared_ptr<Shape>& other) const override {
        auto other_circle = dynamic_pointer_cast<Circle>(other);
        return other_circle != nullptr && x == other_circle->x && y == other_circle->y && radius == other_circle->radius;
    }
};

class Square : public Shape {
private:
    int side;

public:
    Square(int x, int y, int side) : Shape(x, y), side(side) {}

    void draw(vector<vector<char>>& grid) const override {

        for (int i = 0; i < side; ++i) {

            if (x + i < grid[0].size() && y < grid.size()) {
                grid[y][x + i] = '*';
            }
            if (x + i < grid[0].size() && y + side - 1 < grid.size()) {
                grid[y + side - 1][x + i] = '*';
            }
        }

        for (int i = 1; i < side - 1; ++i) {
            if (x < grid[0].size() && y + i < grid.size()) {
                grid[y + i][x] = '*';
            }
            if (x + side - 1 < grid[0].size() && y + i < grid.size()) {
                grid[y + i][x + side - 1] = '*';
            }
        }
    }
    string get_shapes_info() const override {
        return "square " + to_string(x) + " " + to_string(y) + " " + to_string(side);
    }
    bool is_equal(const shared_ptr<Shape>& other) const override {
        auto other_square = dynamic_pointer_cast<Square>(other);
        return other_square != nullptr && x == other_square->x && y == other_square->y && side == other_square->side;
    }
};

class Board {
    vector<vector<char>> grid;
    map<int, shared_ptr<Shape>> shapes;
    int shape_id = 1;


    bool can_be_on_board(int x, int y, int width, int height) {
        if (x < 0 || y < 0 || x + width > BOARD_WIDTH || y + height > BOARD_HEIGHT) {
            return false;
        }
        return true;
    }
    bool can_be_on_board(int x, int y, int radius) {
        if (x - radius < 0 || x + radius >= BOARD_WIDTH || y - radius < 0 || y + radius >= BOARD_HEIGHT) {
