#include "TextProcessors.h"

#include <string>
#include <istream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

void ArticleProcessor::ProcessArticles()
{
    sort(Articles.begin(), Articles.end(), CompareArticles); //sort by ID then process

    for (size_t i = 0; i < Articles.size();++i)
    {
        ProcessArticle(i);
    }
}
void ArticleProcessor::ParseArticle(std::istream &stream)
{
    Articles.push_back(Article());
    auto& article = Articles.back();
    getline(stream,article.ID);
    getline(stream,article.Name);
    getline(stream,article.Text);
}

void ArticleProcessor::ProcessQueries(std::istream& stream)
{
	while (true)
	{
        string query;
        bool inWord = false;
        size_t wordStart = 0;
        getline(stream, query);
        if(!stream.good()) //
        {
	        return;
        }
        if(query.empty())
        {
	        continue;
        }
        vector<string> words;
        size_t head = 0;
        size_t length = query.length();
        while (head < length)
        {
            char c = query[head];

            if (!isalpha(c))
            {
                if (inWord) //check index add if necessary
                {
                    string word = query.substr(wordStart, head - wordStart);
                    for (int i = 0; i < word.size();++i)
                    {
                        word[i] = std::tolower(word[i]);
                    }
                    words.push_back(word); //omits current char as we want
                }
                inWord = false;
            }
            else
            {
                if (!inWord) //just at start of word
                {
                    inWord = true;
                    wordStart = head;
                }
            }
            ++head;
        }
        if(inWord)
        {
            string word = query.substr(wordStart, head - wordStart);
            for (int i = 0; i < word.size();++i)
            {
                word[i] = std::tolower(word[i]);
            }
            words.push_back(word);
            //string ends with word so we dont detect end in loop
        }
        vector<WordRecord> records;
        bool nonexistent = false;
        for(auto && word : words)
        {
            auto record = revIndices.find(word);
            if(record == revIndices.end())
            {
                nonexistent = true;
                break; //no need to check further
            }
            records.push_back(record->second);
        }
        if(records.empty() || nonexistent)
        {
            cout << "No results" << endl << endl;
	        continue;
        }
        if(records.size()==1)
        {
            for(auto&& it : records[0]) //only one word, can be in multiple articles
            {
                if (Articles[it.first].Text.length() - it.second >=75)
                {
                    //string toWrite = Articles[it.first].Text.substr(it.second, 75);
                    cout << '[' << Articles[it.first].ID << "] " << Articles[it.first].Name << endl;
                    cout << Articles[it.first].Text.substr(it.second, 75) << "..." << endl;
                }
                else
                {
                    //string toWrite = Articles[it.first].Text.substr(it.second, Articles[it.first].Text.length() - it.second);
                    cout << '[' << Articles[it.first].ID << "] " << Articles[it.first].Name << endl;
                    cout << Articles[it.first].Text.substr(it.second, Articles[it.first].Text.length() - it.second) << "..." << endl;
                }
                
            }
            cout << endl;
            continue; //intersect needs >1 record
        }
        Intersect(records);
	}
}

void ArticleProcessor::Intersect(const std::vector<WordRecord>& records) //called with at least two records
{
    vector<WordRecord::const_iterator> iterators;
    for(auto&& record : records)
    {
        iterators.push_back(record.begin());
    }
    bool noResult = true;
    while (true)
    {
        int minRecord = iterators[0]->first;
        int minIndex = 0; //first record index and record
        bool same = true;
        
        for (int j = 1; j< iterators.size();++j)
        {
            int currRecord = iterators[j]->first;
	        if(currRecord != minRecord)
	        {
                same = false;
                if(currRecord < minRecord)
                {
                    minRecord = currRecord;
                    minIndex = j;
                }
	        }
        }
        if(!same)
        {
            ++iterators[minIndex];
            if(iterators[minIndex] == records[minIndex].end()) //iterator out of range - nothing more to print
            {
                if(noResult)
                {
                    cout << "No results" << endl << endl;
                }
	            return;
            }
            continue;
        }
        noResult = false;
        PrintQuery(iterators);
        for (int j = 0; j < iterators.size();++j)
        {
            ++iterators[j];
            if (iterators[j] == records[j].end()) //iterator out of range - nothing more to print
            {
                cout << endl;
                return;
            }
        }
        
    }
}

void ArticleProcessor::PrintQuery(const std::vector<WordRecord::const_iterator>& iterators)
{
    //75 or less chars + dots
    size_t minIndex = iterators[0]->second;
    size_t length = Articles[iterators[0]->first].Text.length();
    if(length - minIndex >= 75)
    {
        cout << '[' << Articles[iterators[0]->first].ID << "] " << Articles[iterators[0]->first].Name << endl;
        cout << Articles[iterators[0]->first].Text.substr(minIndex,75) << "..." << endl;
    }
    else
    {
        //string toWrite = Articles[iterators[minIndex]->first].Text.substr(iterators[minIndex]->second, length - iterators[minIndex]->second);
        cout << '[' << Articles[iterators[0]->first].ID << "] " << Articles[iterators[0]->first].Name << endl;
        cout << Articles[iterators[0]->first].Text.substr(minIndex, length - iterators[minIndex]->second) << "..." << endl;
    }
}

void ArticleProcessor::ProcessArticle(size_t index)
{
    size_t head = 0;
    size_t end = Articles[index].Text.size();
    auto& text = Articles[index].Text;
    bool inWord = false;
    size_t wordStart = 0;

    while (head < end)
    {
        char c = text[head];

        if(!isalpha(c))
        {
			if(inWord) //check index add if necessary
			{
                string word = text.substr(wordStart, head - wordStart); //omits current char as we want
                for (int i = 0; i < word.size();++i)
                {
                    word[i] = std::tolower(word[i]);
                }
                auto entry = revIndices.find(word);
                if(entry == revIndices.end()) //word wasnt in index
                {
                    WordRecord record;
                    record.push_back(make_pair(index,wordStart));
                    revIndices.insert(make_pair(word, record));
                }
                else //there already is word
                {
                    //the indices are always sorted in ascending order - if it isnt last one it isnt present
                	if(entry->second.empty() || entry->second.back().first != index)
                    {
                        entry->second.push_back(make_pair(index, wordStart));
                    }
                    //else the word already occured in this article no action
                }
			}
            inWord = false;
        }
        else
        {
	        if(!inWord) //just at start of word
	        {
                inWord = true;
                wordStart = head;
	        }
        }
        ++head;
    }
    //revIndices.push_back(reverseIndex); //every article has 
}

void ArticleProcessor::PrintArticle()
{
    for(auto && article : Articles)
    {
        cout << article.ID << ' ' << article.Name << endl;
    }
}


