#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

class Interpreter {
private:
    std::map<std::string, std::string> variables;
    std::map<std::string, std::vector<std::string> > arrays;

    std::string evaluateExpression(const std::string& expression) {
        std::stringstream result;
        std::stringstream ss(expression);
        std::string token;

        while (ss >> token) {
            if (token[0] == '\'' && token[token.length() - 1] == '\'') {
                result << token.substr(1, token.length() - 2);
            } else if (variables.find(token) != variables.end()) {
                result << variables[token];
            } else if (arrays.find(token) != arrays.end()) {
                result << "[";
                for (const auto& element : arrays[token]) {
                    result << element << ", ";
                }
                result.seekp(-2, std::ios_base::end);
                result << "]";
            } else if (token == "input") {
                std::string input;
                std::getline(std::cin, input);
                result << input;
            } else {
                result << token;
            }
        }

        return result.str();
    }

    void executeStatement(const std::string& statement) {
        if (statement.find("print") == 0) {
            std::string expression = statement.substr(6);

            if (expression[0] == '"' && expression[expression.length() - 1] == '"') {
                std::cout << expression.substr(1, expression.length() - 2) << std::endl;
            } else if (variables.find(expression) != variables.end()) {
                std::cout << variables[expression] << std::endl;
            } else {
                std::string result = evaluateExpression(expression);
                std::cout << result << std::endl;
            }
        } else if (statement.find("def") == 0) {
            std::string functionName = statement.substr(4);
            size_t openParenIndex = functionName.find("(");
            size_t closeParenIndex = functionName.find(")");

            if (openParenIndex != std::string::npos && closeParenIndex != std::string::npos && closeParenIndex > openParenIndex + 1) {
                std::string parameters = functionName.substr(openParenIndex + 1, closeParenIndex - openParenIndex - 1);
                functionName = functionName.substr(0, openParenIndex);
                std::string functionBody;

                size_t endFunctionIndex = statement.find("end");
                if (endFunctionIndex != std::string::npos) {
                    functionBody = statement.substr(closeParenIndex + 1, endFunctionIndex - closeParenIndex - 1);
                }

                variables[functionName] = functionBody;
            }
        } else if (statement.find("if") == 0) {
            size_t thenIndex = statement.find("then");
            size_t elseIndex = statement.find("else");
            size_t endIfIndex = statement.find("end");

            if (thenIndex != std::string::npos && endIfIndex != std::string::npos && thenIndex < endIfIndex) {
                std::string condition = statement.substr(3, thenIndex - 3);
                std::string thenBlock = statement.substr(thenIndex + 4, elseIndex - thenIndex - 4);
                std::string elseBlock = statement.substr(elseIndex + 4, endIfIndex - elseIndex - 4);

                std::string result = evaluateExpression(condition);
                if (result == "true") {
                    executeStatement(thenBlock);
                } else {
                    executeStatement(elseBlock);
                }
            }
        } else if (statement.find("elif") == 0) {
            size_t thenIndex = statement.find("then");
            size_t elseIndex = statement.find("else");
            size_t endIfIndex = statement.find("end");

            if (thenIndex != std::string::npos && endIfIndex != std::string::npos && thenIndex < endIfIndex) {
                std::string condition = statement.substr(5, thenIndex - 5);
                std::string thenBlock = statement.substr(thenIndex + 4, elseIndex - thenIndex - 4);
                std::string elseBlock = statement.substr(elseIndex + 4, endIfIndex - elseIndex - 4);

                std::string result = evaluateExpression(condition);
                if (result == "true") {
                    executeStatement(thenBlock);
                } else {
                    executeStatement(elseBlock);
                }
            }
        } else if (statement.find("else") == 0) {
            size_t endIfIndex = statement.find("end");

            if (endIfIndex != std::string::npos) {
                std::string elseBlock = statement.substr(4, endIfIndex - 4);
                executeStatement(elseBlock);
            }
        } else if (statement.find("try") == 0) {
            size_t catchIndex = statement.find("catch");
            size_t endTryIndex = statement.find("end");

            if (catchIndex != std::string::npos && endTryIndex != std::string::npos && catchIndex < endTryIndex) {
                std::string tryBlock = statement.substr(3, catchIndex - 3);
                std::string catchBlock = statement.substr(catchIndex + 5, endTryIndex - catchIndex - 5);

                try {
                    executeStatement(tryBlock);
                } catch (const std::exception& e) {
                    variables["exception"] = e.what();
                    executeStatement(catchBlock);
                }
            }
        } else if (statement.find("while") == 0) {
            size_t doIndex = statement.find("do");
            size_t endWhileIndex = statement.find("end");

            if (doIndex != std::string::npos && endWhileIndex != std::string::npos && doIndex < endWhileIndex) {
                std::string condition = statement.substr(6, doIndex - 6);
                std::string whileBlock = statement.substr(doIndex + 2, endWhileIndex - doIndex - 2);

                std::string result = evaluateExpression(condition);
                while (result == "true") {
                    executeStatement(whileBlock);
                    result = evaluateExpression(condition);
                }
            }
        } else {
            std::string variable;
            std::string expression;

            size_t equalsIndex = statement.find('=');
            if (equalsIndex != std::string::npos) {
                variable = statement.substr(0, equalsIndex);
                expression = statement.substr(equalsIndex + 1);

                std::string result = evaluateExpression(expression);
                variables[variable] = result;
            } else if (statement.find("print") != std::string::npos) {
                std::string expression = statement.substr(6);
                std::string result = evaluateExpression(expression);
                std::cout << result << std::endl;
            }
        }
    }

public:
    void run(const std::string& filename) {
        std::ifstream inputFile(filename);

        if (!inputFile.is_open()) {
            std::cout << "Error: Failed to open file '" << filename << "'." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(inputFile, line)) {
            executeStatement(line);
        }

        inputFile.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Error: No input file specified." << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    Interpreter interpreter;
    interpreter.run(filename);

    return 0;
}
