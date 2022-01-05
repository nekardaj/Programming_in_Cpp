#ifndef HEADER_DERIVATIVE
#define HEADER_DERIVATIVE
#include <memory>
#include <iostream>
#include <istream>


class Node;
class OperatorNode;
using NodePtr = std::unique_ptr<Node>;
using OperatorPtr = std::unique_ptr<OperatorNode>;
class DeriveAdd;

class Node
{
public:
	virtual ~Node() = default;
	virtual const OperatorNode* SelfToOperator() = 0;
	virtual void Print() = 0;
	/// <summary>
	/// Derives the subexpression with this as root in AST.
	///	We might need original expression parts so copy points to equivalent node in unaltered tree.
	/// </summary>
	/// <param name="copy">Pointer to underived equivalent of this</param>
	virtual  void DeriveSelf(const OperatorNode* copy) = 0; //It may always be operator copy as we dont need the copy in value nodes
	virtual NodePtr Clone () const = 0;
	/// <summary>
	/// Recursively simpliefies tree in postorder.
	/// </summary>
	/// <returns>Returns null if the node wasnt simplified</returns>
	virtual NodePtr SimplifySelf() = 0;
	virtual bool IsValue() = 0;
	virtual char GetVariable() = 0;
	virtual double GetValue() = 0;
	virtual void SetChildren(NodePtr left, NodePtr right) = 0;

private:
};

class OperatorNode : public Node
{
public:
	~OperatorNode() override = default;
	void Print() override
	{
		std::cout << '(';
		Left->Print();
		std::cout << ' ' << _operator << ' ';
		Right->Print();
		std::cout << ')';
	}

	char _operator;

	void DeriveSelf(const OperatorNode* copy) override; //assumes copy is not null, as operand of binary operator has to be defined

	NodePtr SimplifySelf() override;
	const OperatorNode* SelfToOperator() override //allows to avoid frequent conversion
	{
		return this;
	}
	//We gotta store original pointer(if we need it in derivative formula)
	//so we need copy mechanism copy - new node with children copy left and right
	NodePtr Clone() const override
	{
		NodePtr node = std::make_unique<OperatorNode>(Left->Clone(), Right->Clone(), _operator);
		return node;
	}
	OperatorNode (NodePtr left, NodePtr right, char op) : _operator(op),Left(std::move(left)), Right(std::move(right))
	{
		
	}
	bool IsValue() override
	{
		return false;
	}
	void SetChildren(NodePtr left, NodePtr right) override
	{
		Left = std::move(left);
		Right = std::move(right);
	}
	double GetValue() override
	{
		return  0.0;
	}
	char GetVariable() override
	{
		return _operator; //return doesnt matter, should be obvious it is not variable, ensures only numeric nodes return \0
	}
private:
	NodePtr Left;
	NodePtr Right;
};

class ValueNode : public Node //why do I need public
{
public:
	~ValueNode() override = default;
	void Print() override
	{
		if(Variable != '\0')
		{
			std::cout << Variable;
			return;
		}
		std::cout << Value;
	}
	NodePtr SimplifySelf() override
	{
		return nullptr;
	}
	void SetChildren(NodePtr left, NodePtr right) override
	{
		throw std::exception("Unsupported operation");
	}
	void DeriveSelf(const OperatorNode* copy) override //does not use copy so we dont care that it is null
	{
		if(Variable != '\0')
		{
			Value = 1; //Value is held only if 
			Variable = '\0';
		}
		else
		{
			Value = 0;
		}
	}
	bool IsValue() override
	{
		return true;
	}
	//We gotta store original pointer(if we need it in derivative formula)
	//so we need copy mechanism - new node with children copy left and right
	NodePtr Clone() const override
	{
		NodePtr node = std::make_unique<ValueNode>(Value,Variable);
		return node;
	}
	ValueNode(double value, char _variable) : Value(value), Variable(_variable)
	{
	}
	double GetValue() override
	{
		return Value;
	}
	char GetVariable() override
	{
		return Variable;
	}
	const OperatorNode* SelfToOperator() override
	{
		return nullptr;
	}
	double Value;
	char Variable; //if node represents varible is not x00 and holds name
};

//ADD is variable name func to check - modifiable
bool IsOperator(char c);
bool IsVariable(char c);
bool TryParse(std::string str, double& num);
bool CompareOperators(char c1, char c2);

class DeriveAdd
{
public:
	NodePtr operator()(NodePtr left, NodePtr right)
	{
		//return nullptr;
		NodePtr root = std::make_unique<OperatorNode>(nullptr,nullptr,'+' );

	}
};

NodePtr ParseInfix(std::istream& stream);


#endif