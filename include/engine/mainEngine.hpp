#ifndef _eval_main_engine
#define _eval_main_engine
#include "iostream"
#include "iomanip"
using namespace std;

const string VALID_SET(".0123456789+-*/^()");

double eval(string &);
double eval(string &);
double convertToNumber(string &);
double operation(double, double, string &);
int precedence(char);
bool isDigit(char);
bool isOperand(string &);
bool isValidCharacter(char);
vector<string> breakIntoTokens(string &);
vector<string> convertToPostfixExp(vector<string> &);
void manageLeadingNegativeSign(vector<string> &);
void manageFirstBracketAfterOperand(vector<string> &);
bool contains_x(string &);
int mainEngine(argCVDef);



int mainEngine(argCVDef) {
    string exp;
    cout << dye::light_green("Eval ") << __VERSION << endl;
    cout << dye::grey("Enter '.' or '.exit' to exit") << endl;
    // Manual input
    while(true){
        cout << dye::grey("> ");

        fflush(stdin);
        getline(cin, exp);

        // Exit statements
        if (exp == "." || exp == ".exit"){
            cout << dye::grey("Bye :) !") << endl;
            return 0;
        }

        // Checks if there is any whitespace
        if (exp.find_first_not_of(" \t\n\v\f\r") == string::npos)
            continue;

        cout << dye::grey("< ") << setprecision(16) << dye::light_green(eval(exp)) << endl;
    }
    return 0;
}

float evalSingle(string &exp) {
    return eval(exp);
}



void manageFirstBracketAfterOperand(vector<string> &tokens) {
    // cout << "Manage first bracket" << endl;
    if(tokens.size() == 0)
        return;
    string multiplySign = "*";
    if(tokens.size() == 0) return;
    for (int i = 0; i < tokens.size() - 1; i++) {
        if (isOperand(tokens[i]) && tokens[i + 1] == "(") {
            i++;
            tokens.insert(tokens.begin() + i, multiplySign);
        }
    }
}
void manageLeadingNegativeSign(vector<string> &tokens) {
    // cout << "Manage Leading Negative Sign" << endl;
    if(tokens.size() == 0)
        return;
    string openBracket = "(";
    string closeBracket = ")";
    string zero = "0";

    // If the expression starts with '-' sign add '0' before it
    if (tokens[0][0] == '-') {
        tokens.insert(tokens.begin(), zero);
    }

    for (int i = 0; i < tokens.size() - 1; i++) {
        // If there is '-' sign after any operator "*"
        if (!isOperand(tokens[i]) && tokens[i + 1] == "-") {
            // if there is '(' after that add the end bracket after matching the
            // correct bracket
            if (tokens.begin() + i + 2 != tokens.end() &&
                tokens[i + 2] == "(") {
                tokens.insert(tokens.begin() + ++i, openBracket);
                tokens.insert(tokens.begin() + ++i, zero);
                i += 2;
                int bracesCount = 1;
                int j = i + 1;
                while (bracesCount != 0 &&
                       (tokens.begin() + j != tokens.end())) {
                    char currentCh = tokens[j][0];
                    if (currentCh == '(')
                        bracesCount++;
                    else if (currentCh == ')')
                        bracesCount--;
                    j++;
                }
                tokens.insert(tokens.begin() + j, closeBracket);
            } else {
                tokens.insert(tokens.begin() + ++i, openBracket);
                tokens.insert(tokens.begin() + ++i, zero);
                i += 2;
                tokens.insert(tokens.begin() + ++i, closeBracket);
            }
        }
    }
}
double eval(string &exp) {
    vector<string> tokens = breakIntoTokens(exp);
    // if(tokens.size() == 0) return -1;
    manageFirstBracketAfterOperand(tokens);
    manageLeadingNegativeSign(tokens);
    tokens = convertToPostfixExp(tokens);
    stack<double> excStack;
    for (auto &token : tokens) {
        if (isOperand(token)) {
            excStack.push(convertToNumber(token));
        } else {
            double op1 = excStack.top();
            excStack.pop();
            double op2 = excStack.top();
            excStack.pop();
            excStack.push(operation(op1, op2, token));
        }
    }
    return excStack.empty() ? -1 : excStack.top();
}


vector<string> breakIntoTokens(string &exp) {
    // cout << "Breaking Into Tokens" << endl;

    vector<string> tokens;
    string currentToken = "";

    for (char &ch : exp) {
        if (ch == ' ' || ch == '\n' || ch == '\t') continue;

        if (!isValidCharacter(ch)) {
            cout << dye::light_red("Invalid Expression ");
            return tokens;
        }

        if (isDigit(ch))
            currentToken.push_back(ch);
        else {
            if (currentToken.length() != 0) {
                tokens.push_back(currentToken);
                currentToken = "";
            }
            string s = "";
            s.push_back(ch);
            tokens.push_back(s);
        }
    }
    if (currentToken.length() != 0) tokens.push_back(currentToken);

    return tokens;
}

vector<string> convertToPostfixExp(vector<string> &tokens) {
    // cout << "Convert to postfix" << endl;
    vector<string> postfix;
    stack<string> stk;
    if(tokens.size() == 0)
        return tokens;
    for (auto &token : tokens) {
        if (isOperand(token))
            postfix.push_back(token);

        else if (token == "(")
            stk.push(token);

        else if (token == ")") {
            while (!stk.empty() && stk.top() != "(") {
                postfix.push_back(stk.top());
                stk.pop();
            }

            if (!stk.empty() && stk.top() != "(")
                break;
            else
                stk.pop();

        } else {
            while (!stk.empty() &&
                   precedence(token[0]) <= precedence(stk.top()[0])) {
                postfix.push_back(stk.top());
                stk.pop();
            }
            stk.push(token);
        }
    }

    while (!stk.empty()) {
        postfix.push_back(stk.top());
        stk.pop();
    }
    return postfix;
}




double convertToNumber(string &token) {
    char num[token.length()];
    strcpy(num, token.c_str());
    return atof(num);
}

double operation(double op1, double op2, string &token) {
    switch (token[0]) {
        case '+':
            return op2 + op1;
        case '-':
            return op2 - op1;
        case '*':
            return op2 * op1;
        case '/':
            return op2 / op1;
        case '^':
            return pow(op2, op1);
    }
    return 0;
}

int precedence(char ch) {
    switch (ch) {
        case '+':
        case '-':
            return 1;

        case '*':
        case '/':
            return 2;

        case '^':
            return 3;
    }
    return -1;
}

bool isOperand(string &s) {
    if (s.length() == 1)
        return isdigit(s[0]);
    else
        return true;
}
bool isValidCharacter(char ch) {
    string validSet = VALID_SET;
    for (auto &c : validSet)
        if (ch == c) return true;
    return false;
}

bool isDigit(char c) { return c == '.' || (c >= '0' && c <= '9'); }


#endif