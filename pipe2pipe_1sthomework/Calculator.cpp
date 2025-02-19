#include <iostream>
#include <fstream>
#include <stack>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include <unistd.h>

#define FIFO_NAME "calc_pipe"

int evaluatePolishNotation(const std::string &expression) {
    std::istringstream iss(expression);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    std::reverse(tokens.begin(), tokens.end()); 

    std::stack<int> st;
    for (const std::string &t : tokens) {
        if (isdigit(t[0])) { 
            st.push(t[0] - '0'); 
        } else {
            if (st.size() < 2) {
                std::cerr << "Ошибка: Недостаточно операндов\n";
                return -1;
            }
            int a = st.top(); st.pop();
            int b = st.top(); st.pop();
            int result = 0;

            switch (t[0]) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': 
                    if (b == 0) {
                        std::cerr << "Ошибка: Деление на ноль\n";
                        return -1;
                    }
                    result = a / b; 
                    break;
                default:
                    std::cerr << "Ошибка: Неизвестная операция '" << t << "'\n";
                    return -1;
            }
            st.push(result);
        }
    }

    return st.top();
}

int main() {
    mkfifo(FIFO_NAME, 0666); 

    std::string expression;
    std::cout << "Введите выражение в польской нотации: ";
    std::getline(std::cin, expression);

    int result = evaluatePolishNotation(expression);

    if (result != -1) {
        int fifo_fd = open(FIFO_NAME, O_WRONLY);
        if (fifo_fd != -1) {
            std::string res_str = std::to_string(result);
            write(fifo_fd, res_str.c_str(), res_str.size());
            close(fifo_fd);
        } else {
            std::cerr << "Ошибка: Не удалось открыть FIFO\n";
        }
    }

    return 0;
}

