#include "parse.hpp"
#include <iostream>

namespace parse
{

  list<string> tokenize(const string& input)
  {
    list<string> tokens;
    string_size i = 0;
    while (i != input.size()) {				

      // skip whitespace
      while (i != input.size() && isspace(input[i])) 
	i++;
      
      // if it's a paren, add it as a token
      if (isparen(input[i])) {
	tokens.push_back(input.substr(i,1));
	i++;
	continue;
      }

      // otherwise go until it's a paren or whitespace
      string_size j = i;
      while (j != input.size() && !isspace(input[j]) && !isparen(input[j]))
	j++;
      
      if (i != j) {
	tokens.push_back(input.substr(i, j - i));
	i = j;
      }
    }
    return tokens;
  }

  bool isparen(const char& c)
  {
    return c == '(' || c == ')';
  }

  Parser::Parser(const string& s) : tokens{tokenize(s)} {}

  unique_ptr<exp> Parser::parse()
  {
    cur = tokens.begin();
    end = tokens.end();
    return parse_exp();
  }

  unique_ptr<exp>
  Parser::parse_exp()
  {
    if (isNumeric(*cur)) {
      return unique_ptr<exp>{new num(std::stoi(*cur++))};
    } else if (*cur == "(") {
      string first = *++cur;
      cur++;
      if (first == "+") {
	return unique_ptr<exp>{new add(parse_until_close())};
      } else if (first == "-") {
	return unique_ptr<exp>{new sub(parse_until_close())};
      } else if (first == "*") {
	return unique_ptr<exp>{new mul(parse_until_close())};
      } else if (first == "/") {
	return unique_ptr<exp>{new expressions::div(parse_until_close())};
      } else if (first == "lambda") {
	if (*cur != "(")
	  throw "ill-formed lambda";
	cur++;
	list<string> params;
	while (*cur != ")" && cur != end) {
	  params.push_back(*cur);
	  cur++;
	}
	if (*cur != ")")
	  throw "ill-formed lambda";
	cur++;
	unique_ptr<exp> body = parse_exp();
	if (*cur != ")")
	  throw "ill-formed lambda";
	cur++;
	return unique_ptr<exp>{new lambda(params, move(body))};
      } else {
	cur--;
	std::cout << "app" << std::endl;
	return unique_ptr<exp>{new app(parse_until_close())};
      }
    } else {
      std::cout << "var " << *cur << std::endl;
      auto tv = *cur;
      cur++;
      return unique_ptr<exp>{new var(tv)};
    }
  }

  list<unique_ptr<exp>>
  Parser::parse_until_close()
  {
    list<unique_ptr<exp>> exps;
    while (*cur != ")" && cur != end) {
      exps.push_back(move(parse_exp()));
    }
    if (cur == end)
      throw "unmatched parens";
    cur++;
    return exps;
  }

  
  bool isNumeric(const string& str)
  {
    return std::regex_match(str, std::regex("\\d+"));
  }

  string run(string exp_string)
  {
    Parser p{exp_string};
    unique_ptr<exp> e = p.parse();
    return e->eval(nullptr)->toString();
  }


} // namespace parse
