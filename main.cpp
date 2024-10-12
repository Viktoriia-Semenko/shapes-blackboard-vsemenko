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
    bool is_filled;
public:
    Shape(int x, int y, bool fill) : x(x), y(y), is_filled(fill) {}
    virtual void draw(vector<vector<char>>& grid) const = 0;
    virtual string get_shapes_info() const = 0;
    virtual bool is_equal(const shared_ptr<Shape>& other) const = 0;
    virtual ~Shape() = default;
};

class Triangle : public Shape {
    int height;
public:
    Triangle(int x, int y, int height, bool is_filled) : Shape(x, y, is_filled), height(height) {}

    void draw(vector<vector<char>>& grid) const override {
        if (is_filled) {
            for (int i = 0; i < height; ++i) {
                int left_most = x - i;
                int right_most = x + i;
                int position_y = y + i;

                if (position_y < BOARD_HEIGHT) {
                    for (int j = left_most; j <= right_most && j < BOARD_WIDTH; ++j) {
                        if (j >= 0)
                            grid[position_y][j] = '*';
                    }
                }
            }
        } else {
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
    }

    string get_shapes_info() const override {
        return (is_filled ? "fill " : "frame ") + string("triangle ") + to_string(x) + " " + to_string(y) + " " + to_string(height);
    }

    bool is_equal(const shared_ptr<Shape>& other) const override {
        auto other_triangle = dynamic_pointer_cast<Triangle>(other);
        return other_triangle != nullptr && is_filled == other_triangle->is_filled && x == other_triangle->x && y == other_triangle->y && height == other_triangle->height;
    }
};

class Rectangle : public Shape {
    int width, height;
public:
    Rectangle(int x, int y, int width, int height, bool is_filled) : Shape(x, y, is_filled), width(width), height(height) {}

    void draw(vector<vector<char>>& grid) const override {
        if (is_filled) {
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    if (x + j < BOARD_WIDTH && y + i < BOARD_HEIGHT && x + j >= 0 && y + i >= 0)
                        grid[y + i][x + j] = '*';
                }
            }
        } else {
            for (int i = 0; i < width; ++i) {
                if (x + i < BOARD_WIDTH) {
                    if (y >= 0 && y < BOARD_HEIGHT) grid[y][x + i] = '*';
                    if (y + height - 1 >= 0 && y + height - 1 < BOARD_HEIGHT)
                        grid[y + height - 1][x + i] = '*';
                }
            }
            for (int i = 0; i < height; ++i) {
                if (y + i < BOARD_HEIGHT) {
                    if (x >= 0 && x < BOARD_WIDTH) grid[y + i][x] = '*';
                    if (x + width - 1 >= 0 && x + width - 1 < BOARD_WIDTH)
                        grid[y + i][x + width - 1] = '*';
                }
            }
        }
    }

    string get_shapes_info() const override {
        return (is_filled ? "fill " : "frame ") + string("rectangle ") + to_string(x) + " " + to_string(y) + " " + to_string(width) + " " + to_string(height);
    }
    bool is_equal(const shared_ptr<Shape>& other) const override {
        auto other_rectangle = dynamic_pointer_cast<Rectangle>(other);
        return other_rectangle != nullptr && is_filled == other_rectangle->is_filled && x == other_rectangle->x && y == other_rectangle->y && width == other_rectangle->width && height == other_rectangle->height;
    }
};

class Circle : public Shape {
private:
    int radius;
public:
    Circle(int x, int y, int radius, bool is_filled) : Shape(x, y, is_filled), radius(radius) {}

    void draw(vector<vector<char>>& grid) const override {

        for (int i = -radius; i <= radius; ++i) {
            for (int j = -radius; j <= radius; ++j) {

                int distSquared = i * i + j * j;
                int radiusSquared = radius * radius;

                if ((is_filled && distSquared <= radiusSquared) ||
                    (!is_filled && distSquared >= radiusSquared - radius && distSquared <= radiusSquared + radius)) {
                    int posX = x + j;
                    int posY = y + i;
                    if (posX >= 0 && posX < BOARD_WIDTH && posY >= 0 && posY < BOARD_HEIGHT)
                        grid[posY][posX] = '*';
                }
            }
        }
    }

    string get_shapes_info() const override {
        return (is_filled ? "fill " : "frame ") + string("circle ") + to_string(x) + " " + to_string(y) + " " + to_string(radius);
    }
    bool is_equal(const shared_ptr<Shape>& other) const override {
        auto other_circle = dynamic_pointer_cast<Circle>(other);
        return other_circle != nullptr && is_filled == other_circle->is_filled && x == other_circle->x && y == other_circle->y && radius == other_circle->radius;
    }
};

class Square : public Shape {
private:
    int side;

public:
    Square(int x, int y, int side, bool is_filled) : Shape(x, y, is_filled), side(side) {}

    void draw(vector<vector<char>>& grid) const override {
        if (is_filled) {
            for (int i = 0; i < side; ++i) {
                for (int j = 0; j < side; ++j) {
                    if (x + j < BOARD_WIDTH && y + i < BOARD_HEIGHT && x + j >= 0 && y + i >= 0)
                        grid[y + i][x + j] = '*';
                }
            }
        } else {
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
    }

    string get_shapes_info() const override {
        return (is_filled ? "fill " : "frame ") + string("square ") + to_string(x) + " " + to_string(y) + " " + to_string(side);
    }
    bool is_equal(const shared_ptr<Shape>& other) const override {
        auto other_square = dynamic_pointer_cast<Square>(other);
        return other_square != nullptr && is_filled == other_square->is_filled && x == other_square->x && y == other_square->y && side == other_square->side;
    }
};

class Board {
    vector<vector<char>> grid;
    map<int, shared_ptr<Shape>> shapes;
    int shape_id = 1;


    static bool can_be_on_board(int x, int y, int width, int height) {
        return !(x >= BOARD_WIDTH || y >= BOARD_HEIGHT || x + width <= 0 || y + height <= 0);
    }

    static bool can_be_on_board_circle(int x, int y, int radius) {
        return !(x + radius < 0 || x - radius >= BOARD_WIDTH || y + radius < 0 || y - radius >= BOARD_HEIGHT);
    }

public:
    Board() : grid(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, ' ')) {}

    void add_shape(shared_ptr<Shape> shape, const string& type, int x, int y, int size1, int size2 = 0) {

        bool can_fit = false;

        for (const auto& [id, existing_shape] : shapes) {
            if (existing_shape->is_equal(shape)) {
                cout << "Error: shape with the same type and parameters already exists" << endl;
                return;
            }
        }

        if (type == "rectangle" || type == "square") {
            can_fit = can_be_on_board(x, y, size1, (type == "square") ? size1 : size2);
        } else if (type == "triangle") {
            can_fit = can_be_on_board(x - size1, y, size1 * 2 - 1, size1);
        } else if (type == "circle") {
            can_fit = can_be_on_board_circle(x, y, size1);
        }

        if (can_fit) {
            shapes[shape_id++] = shape;
        } else {
            cout << "Error: shape cannot be placed outside the board or be bigger than the board's size" << endl;
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

        string is_filled, shape_type;
        while (file >> is_filled >> shape_type) {
            bool filled = (is_filled == "fill");
            if (shape_type == "circle") {
                int x, y, radius;
                file >> x >> y >> radius;
                add_shape(make_shared<Circle>(x, y, radius, filled), "circle", x, y, radius);
            } else if (shape_type == "rectangle") {
                int x, y, width, height;
                file >> x >> y >> width >> height;
                add_shape(make_shared<Rectangle>(x, y, width, height, filled), "rectangle", x, y, width, height);
            } else if (shape_type == "square") {
                int x, y, side;
                file >> x >> y >> side;
                add_shape(make_shared<Square>(x, y, side, filled), "square", x, y, side);
            } else if (shape_type == "triangle") {
                int x, y, height;
                file >> x >> y >> height;
                add_shape(make_shared<Triangle>(x, y, height, filled), "triangle", x, y, height);
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

                string shape_type, fill_type;
                cin >> fill_type >> shape_type;
                bool filled = (fill_type == "fill");

                if (shape_type == "rectangle") {
                    int x, y, width, height;
                    cin >> x >> y >> width >> height;
                    board.add_shape(make_shared<Rectangle>(x, y, width, height, filled), "rectangle", x, y, width, height);
                } else if (shape_type == "triangle") {
                    int x, y, height;
                    cin >> x >> y >> height;
                    board.add_shape(make_shared<Triangle>(x, y, height, filled), "triangle", x, y, height);
                }
                else if(shape_type == "circle") {
                    int x, y, radius;
                    cin >> x >> y >> radius;
                    board.add_shape(make_shared<Circle>(x, y, radius, filled), "circle", x, y, radius);
                }
                else if(shape_type == "square") {
                    int x, y, side;
                    cin >> x >> y >> side;
                    board.add_shape(make_shared<Square>(x, y, side, filled), "square", x, y, side);
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