#ifndef FULLTEXT_TEXTPROCESSORS_H
#define FULLTEXT_TEXTPROCESSORS_H
#include <string>
#include <istream>
#include <iostream>
#include <vector>
#include <map>
using WordRecord = std::vector<std::pair<int, size_t>>;
using ReverseIndex = std::map<std::string, WordRecord>;

struct Article
{
public:
    std::string Name;
    std::string ID;
    std::string Text;
};
inline bool CompRecordElement(const std::pair<int, size_t>& p1, const std::pair<int, size_t>& p2)
{
    return p1.first < p2.first;
}

inline bool CompareArticles(const Article& a1, const Article& a2)
{
    return a1.ID < a2.ID;
}

class ArticleProcessor
{
public:
    void ParseArticle(std::istream & stream);
    void ProcessQueries(std::istream& stream);
    void Intersect(const std::vector<WordRecord>& records);
    void PrintQuery(const std::vector<WordRecord::const_iterator>& iterators);
    void PrintArticle();
    void ProcessArticles();
private:
    std::vector<Article> Articles;
	ReverseIndex revIndices; //each word has asociated vector of article number and first occurence
    void ProcessArticle(size_t index);
};

#endif //FULLTEXT_TEXTPROCESSORS_H
