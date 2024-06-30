#pragma once
#include <string>
#include <map>

using std::string;

class HTMLParser
{
public:
	static string ConverHTMLToNormal(string data)
	{
        std::map<string,string> m;
        m["&quot;"] = "\'";
        m["&apos;"] = "\'";
        m["&#34;"] = "\'";
        m["&#039;"] = "\'";
        m["&amp;"] = "&";
        m["&gt;"] = ">";
        m["&lt;"] = "<";
        m["&frasl;"] = "/";
        m["&nbsp;"] = " ";
        m["&reg;"] = "®";
        m["&copy;"] = "©";

        for (auto& entry : m)
        {
            replaceAll(data, entry.first, entry.second);
        }

        return data;

	}



private:

	static void replaceAll(string& input, string replace_word, string replace_by)
	{

        // Find the first occurrence of the substring 
        size_t pos = input.find(replace_word);

        // Iterate through the string and replace all 
        // occurrences 
        while (pos != string::npos) {
            // Replace the substring with the specified string 
            input.replace(pos, replace_word.size(), replace_by);

            // Find the next occurrence of the substring 
            pos = input.find(replace_word,
                pos + replace_by.size());
        }
	}

	HTMLParser();

};

