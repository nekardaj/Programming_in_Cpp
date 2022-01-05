#include "Derivatives.h"
#include <string>
#include <sstream>
#include <stack>
#include <vector>
using namespace std;
void OperatorNode::DeriveSelf(const OperatorNode* copy)
{
	//if deriveself is called with null its a value node which wont use it
	Left->DeriveSelf(copy->Left->SelfToOperator());
	Right->DeriveSelf(copy->Right->SelfToOperator());
	switch (_operator)
	{
	case '+':
	{
		break;
	}
	case '-':
	{
		break; //we derive in postorder so the children in working tree will be derived already so we dont need to do anything
	}
	case '*':
	{
		_operator = '+';
		Left = make_unique<OperatorNode>(move(Left), copy->Right->Clone(), '*');
			// (a*b)' means we need to get a'*b and a*b' so we move the derivative as we have it as child and need it once
		Right = make_unique<OperatorNode>(copy->Left->Clone(), move(Right), '*');
		break;
	}
	case '/':
	{
		//_operator = '/'; operator stays same
		OperatorPtr leftTemp = make_unique<OperatorNode>(move(Left), copy->Right->Clone(),'*');
		OperatorPtr rightTemp = make_unique<OperatorNode>(copy->Left->Clone(),move(Right),'*');
			//temp store a*b' and a'*b which are then inserted under - operator node
		Left = make_unique<OperatorNode>(move(leftTemp),move(rightTemp),'-');
		Right = make_unique<OperatorNode>(copy->Right->Clone(), copy->Right->Clone(), '*');
		break;
	}
	default:
	{
		throw std::exception("Unknown operator");
	}
	}
}

NodePtr OperatorNode::SimplifySelf()
{
	NodePtr newLeft = Left->SimplifySelf();
	NodePtr newRight = Right->SimplifySelf();
	//if null is returned the node didnt change its type, else overwrite ptr which disposes old subtree
	if(newLeft != nullptr)
	{
		Left = move(newLeft);
	}
	if(newRight != nullptr)
	{
		Right = move(newRight);
	}
	switch (_operator)
	{
		case '+':
		{
			if (Left->GetVariable() == '\0' && Left->GetValue() == 0.0) //one operand is 0
			{
				return move(Right);
			}
			if (Right->GetVariable() == '\0' && Right->GetValue() == 0.0)
			{
				return move(Left);
			}
			break;
		}
		case '-':
		{
			if (Right->GetVariable() == '\0' && Right->GetValue() == 0.0)
			{
				return move(Left);
			}
			break;
		}
		case '*':
		{
			if (Left->GetVariable() == '\0' && Left->GetValue() == 0.0) //one operand is 0
			{
				return make_unique<ValueNode>(0.0,'\0'); // 0 / or * anything is 0 (0/0 is defined as 0 here)
			}
			if (Right->GetVariable() == '\0' && Right->GetValue() == 0.0)
			{
				return make_unique<ValueNode>(0.0, '\0'); // 0 / or * anything is 0 (0/0 is defined as 0 here)
			}
			if(Left->GetVariable() == '\0' && Left->GetValue() == 1.0)
			{
				return move(Right);
			}
			if(Right->GetVariable() == '\0' && Right->GetValue() == 1.0)
			{
				return  move(Left);
			}
			break;
		}
		case '/':
		{
			if (Left->GetVariable() == '\0' && Left->GetValue() == 0.0) //one operand is 0
			{
				return make_unique<ValueNode>(0.0, '\0'); // 0 / anything is 0 (0/0 is defined as 0 here)
			}
			break;
		}
		default:
		{
			throw exception("unknown operator");
		}
	}

	if(Left->GetVariable() == '\0' && Right->GetVariable() == '\0') // both constants
	{
		switch (_operator)
		{
		case '+':
		{
			return make_unique<ValueNode>(Left->GetValue() + Right->GetValue(), '\0');
		}
		case '-':
		{
			return make_unique<ValueNode>(Left->GetValue() - Right->GetValue(), '\0');
		}
		case '*':
		{
			return make_unique<ValueNode>(Left->GetValue() * Right->GetValue(), '\0');
		}
		case '/':
		{
			return make_unique<ValueNode>(Left->GetValue() / Right->GetValue(), '\0');
		}
		default:
		{
			throw exception("unknown operator");
		}
		}
	}
	return nullptr; //no simplification found
}

NodePtr ParseInfix(std::istream& stream)
{
	stack<OperatorPtr> operators_st;
	vector<NodePtr> postfix;
	while (true)
	{
		string expr;
		stream >> expr;
		if(stream.fail())
		{
			break;
		}
		stack<NodePtr> prefix; //RPN pushed into stack -> prefix
		if(expr.length() == 1)
		{
			if(IsOperator(expr[0]))
			{
				// ( is handeled separately
				while (!operators_st.empty() && operators_st.top()->_operator != '(' && CompareOperators(operators_st.top()->_operator, expr[0]))
				{
					OperatorPtr op = move(operators_st.top());
					operators_st.pop(); //remove the robbed ptr
					postfix.push_back(move(op));
				}
				operators_st.push(make_unique<OperatorNode>(nullptr, nullptr, expr[0]));
				continue;
			}
			if (expr[0] == '(')
			{
				operators_st.push(make_unique<OperatorNode>(nullptr, nullptr, expr[0]));
				continue;
			}
			if (expr[0] == ')')
			{
				bool closed = false;
				while (!operators_st.empty()) //push operators until ( is reached
				{
					if(operators_st.top()->_operator == '(')
					{
						operators_st.pop(); //discard ( as RPN doesnt need it
						closed = true;
						break;
					}
					postfix.push_back(move(operators_st.top()));
					operators_st.pop();//pop robbed ptr
				}
				if(closed)
				{
					continue;
				}
				throw exception("Unpaired parenthesis");
			}
			if(IsVariable(expr[0]))
			{
				postfix.push_back(make_unique<ValueNode>(1.0, 'x'));
				continue;
			}
		}
		double num;
		if(TryParse(expr,num)) //no wspace ensured by stream
		{
			postfix.push_back(make_unique<ValueNode>(num, '\0'));
			continue;
		}
		throw exception("Unknown token");
	}
	while (!operators_st.empty())
	{
		postfix.push_back(move(operators_st.top()));
		operators_st.pop();
	}
	//parse the RPN
	NodePtr temp = nullptr;
	stack<NodePtr> st;
	for (auto&& node : postfix)
	{
		if(node->IsValue()) //value node
		{
			if(temp != nullptr)
			{
				st.push(move(temp));
			}
			temp = move(node);
			continue;
		}
		if(!st.empty() && temp != nullptr)
		{
			//temp is right
			node->SetChildren(move(st.top()),move(temp));
			st.pop();
			st.push(move(node));
			temp = nullptr;
			continue;
		}
		if(st.size() >= 2)
		{
			NodePtr right = move(st.top());
			st.pop();
			node->SetChildren(move(st.top()), move(right));
			st.pop();
			st.push(move(node));
		}
	}
	if(st.size() != 1)
	{
		//incorrect expression
		throw exception("Invalid expression");
	}
	return std::move(st.top());
}
bool IsOperator(char c)
{
	return c == '+' || c == '-' || c == '*' || c == '/';
}
bool IsVariable(char c)
{
	return c == 'x';
}
//returns true if first operator has higher or equal priority than second, assumes c1 is not parenthesis
bool CompareOperators(char c1, char c2)
{
	if(c2 == '(')
	{
		return false;
	}
	if(c1 == '/' || c1 == '*')
	{
		return true;
	}
	if ((c1 == '+' || c1 == '-') && (c2 == '+'|| c2 == '-'))
	{
		return true;
	}
	return false;
}


//assumes no wspace
bool TryParse(string input, double& var)
{
	stringstream buffer(input);

	return buffer >> var && buffer.eof();
}


