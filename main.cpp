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


    bool can_be_on_board(int x, int y, int width, int height) {
        if (x < 0 || y < 0 || x + width > BOARD_WIDTH || y + height > BOARD_HEIGHT) {
            return false;
        }
        return true;
    }
    bool can_be_on_board(int x, int y, int radius) {
        if (x - radius < 0 || x + radius >= BOARD_WIDTH || y - radius < 0 || y + radius >= BOARD_HEIGHT) {
            return false;
        }
        return true;
    }

public:
    Board() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}

    void addShape(shared_ptr<Shape> shape, const string& type, int x, int y, int size1, int size2 = 0) {

        bool canFit = false;

        if (type == "rectangle" || type == "square") {
            canFit = can_be_on_board(x, y, size1, (type == "square") ? size1 : size2);
        } else if (type == "triangle") {
            canFit = can_be_on_board(x - size1, y, size1 * 2 - 1, size1);
        } else if (type == "circle") {
            canFit = can_be_on_board(x, y, size1);
        }

        if (canFit) {
            shapes[shape_id++] = shape;
        } else {
            cout << "Error: shape cannot be placed outside the board or be bigger than board's size\n";
        }
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

    void save_board(const string& file_path) const {
        ofstream file(file_path);
        if (!file) {
            cout << "Error opening file" << endl;
            return;
        }

        for (const auto& [id, shape] : shapes) {
            file << shape->get_shapes_info() << "\n";
        }
        file.close();
    }

    void load_board(const string& file_path) {
        ifstream file(file_path);
        if (!file) {
            cout << "Error opening file" << endl;
            return;
        }

        clear_board();

        string shape_type;
        while (file >> shape_type) {
            if (shape_type == "circle") {
                int x, y, radius;
                file >> x >> y >> radius;
                addShape(make_shared<Circle>(x, y, radius), "circle", x, y, radius);
            } else if (shape_type == "rectangle") {
                int x, y, width, height;
                file >> x >> y >> width >> height;
                addShape(make_shared<Rectangle>(x, y, width, height), "rectangle", x, y, width, height);
            } else if (shape_type == "square") {
                int x, y, side;
                file >> x >> y >> side;
                addShape(make_shared<Square>(x, y, side), "square", x, y, side);
            } else if (shape_type == "triangle") {
                int x, y, height;
                file >> x >> y >> height;
                addShape(make_shared<Triangle>(x, y, height), "triangle", x, y, height);
            }
        }
        file.close();
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
            } else if (command == "save") {
                string file_path;
                cin >> file_path;
                board.save_board(file_path);
            } else if (command == "load") {
                string file_path;
                cin >> file_path;
                board.load_board(file_path);
            } else if (command == "add") {
                string shape_type;
                cin >> shape_type;
                if (shape_type == "rectangle") {
                    int x, y, width, height;
                    cin >> x >> y >> width >> height;
                    board.addShape(make_shared<Rectangle>(x, y, width, height), "rectangle", x, y, width, height);
                } else if (shape_type == "triangle") {
                    int x, y, height;
                    cin >> x >> y >> height;
                    board.addShape(make_shared<Triangle>(x, y, height), "triangle", x, y, height);
                }
                else if(shape_type == "circle") {
                    int x, y, radius;
                    cin >> x >> y >> radius;
                    board.addShape(make_shared<Circle>(x, y, radius), "circle", x, y, radius);
                }
                else if(shape_type == "square") {
                    int x, y, side;
                    cin >> x >> y >> side;
                    board.addShape(make_shared<Square>(x, y, side), "square", x, y, side);
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