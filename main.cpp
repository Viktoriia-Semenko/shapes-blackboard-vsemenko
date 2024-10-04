#include <iostream>
#include <vector>
#include <memory>
#include <map>

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
};

class Board {
    vector<vector<char>> grid;
    map<int, shared_ptr<Shape>> shapes;
    int shape_id = 1;
public:
    Board() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}

    void addShape(shared_ptr<Shape> shape) {
        shapes[shape_id++] = shape;
    }

    void undo() {
        if (!shapes.empty()) {
            shapes.erase(--shape_id);
        } else {
            cout << "No shapes to undo\n";
        }
    }

    void list_shapes() const {
        if (shapes.empty()) {
            cout << "No shapes on the board\n";
        } else {
            for (const auto &[id, shape]: shapes) {
                cout << id << " " << shape->get_shapes_info() << "\n";
            }
        }
    }

    void clear_board() {
        if (!shapes.empty()) {
            shapes.clear();
        } else {
            cout << "No shapes to clear\n";
        }
    }

    void draw() {

        vector<vector<char>> tempGrid = grid;

        for (const auto& [id, shape] : shapes) {
            shape->draw(tempGrid);
        }

        cout << "-";
        for (int i = 0; i < BOARD_WIDTH; ++i) {
            cout << "-";
        }
        cout << "-\n";

        for (const auto& row : tempGrid) {
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

class CLI {
    Board board;

public:

    void list_available_shapes() {
        cout << "> circle coordinates radius\n";
        cout << "> triangle coordinates height\n";
        cout << "> square coordinates side\n";
        cout << "> rectangle coordinates width height\n";
    }

    void run() {
        string command;
        while (true) {
            cout << "> ";
            cin >> command;

            if (command == "draw") {
                board.draw();
            } else if (command == "list") {
                board.list_shapes();
            } else if (command == "shapes") {
                    list_available_shapes();
            } else if (command == "clear") {
                board.clear_board();
            } else if (command == "undo") {
                board.undo();
            } else if (command == "add") {
                string shape_type;
                cin >> shape_type;
                if (shape_type == "rectangle") {
                    int x, y, width, height;
                    cin >> x >> y >> width >> height;
                    board.addShape(make_shared<Rectangle>(x, y, width, height));
                } else if (shape_type == "triangle") {
                    int x, y, height;
                    cin >> x >> y >> height;
                    board.addShape(make_shared<Triangle>(x, y, height));
                }
                else if(shape_type == "circle") {
                    int x, y, radius;
                    cin >> x >> y >> radius;
                    board.addShape(make_shared<Circle>(x, y, radius));
                }
                else if(shape_type == "square") {
                    int x, y, side;
                    cin >> x >> y >> side;
                    board.addShape(make_shared<Square>(x, y, side));
                }
            } else if (command == "exit") {
                break;
            } else {
                cout << "Unknown command!\n";
            }
        }
    }
};

int main() {
    CLI cli;
    cli.run();
    return 0;
}
