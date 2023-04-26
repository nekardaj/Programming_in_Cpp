#include "TextProcessors.h"
#include <iostream>
#include <fstream>

using namespace std;
int main(int argc, char** argv)
{
    /**/
	if(argc > 3 || argc < 2)
	{
	    //cout << "Arguments error" << endl;
	    return 0;
	}
	/**/
    ifstream articles;
    ifstream queries;

    articles.open(argv[1]);
    if (argc == 3)
    {
        queries.open(argv[2]);
    }
    if(!articles.good())
    {
	    return 0;
    }

    ArticleProcessor processor;

    while (articles.peek() != '\n')
    {
        processor.ParseArticle(articles);
    }
    processor.ProcessArticles();
    //processor.ProcessQueries(cin);
    if (argc == 3)
    {
        processor.ProcessQueries(queries);
    }
    else
    {
        processor.ProcessQueries(cin);
    }
    //processor.PrintArticle();
    return 0;
}
