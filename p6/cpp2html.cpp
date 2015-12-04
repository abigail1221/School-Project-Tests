/*
 * CSc103 Project 5: Syntax highlighting, part two.
 * See readme.html for details.
 * Please list all references you made use of in order to complete the
 * assignment: your classmates, websites, etc.  Aside from the lecture notes
 * and the book, please list everything.  And remember- citing a source does
 * NOT mean it is okay to COPY THAT SOURCE.  What you submit here **MUST BE
 * YOUR OWN WORK**.
 * References: friend Laisa
 * Finally, please indicate approximately how many hours you spent on this:
 * #hours: ~8 hours (O.O)
 */

#include "fsm.h"
using namespace cppfsm;
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <set>
using std::set;
#include <map>
using std::map;
#include <initializer_list> // for setting up maps without constructors.

// enumeration for our highlighting tags:
enum {
	hlstatement,  // used for "if,else,for,while" etc...
	hlcomment,    // for comments
	hlstrlit,     // for string literals
	hlpreproc,    // for preprocessor directives (e.g., #include)
	hltype,       // for datatypes and similar (e.g. int, char, double)
	hlnumeric,    // for numeric literals (e.g. 1234)
	hlescseq,     // for escape sequences
	hlerror,      // for parse errors, like a bad numeric or invalid escape
	hlident       // for other identifiers.  Probably won't use this.
};

// usually global variables are a bad thing, but for simplicity,
// we'll make an exception here.
// initialize our map with the keywords from our list:
map<string, short> hlmap = {
#include "res/keywords.txt"
};
// note: the above is not a very standard use of #include...

// map of highlighting spans:
map<int, string> hlspans = {
	{hlstatement, "<span class='statement'>"},
	{hlcomment, "<span class='comment'>"},
	{hlstrlit, "<span class='strlit'>"},
	{hlpreproc, "<span class='preproc'>"},
	{hltype, "<span class='type'>"},
	{hlnumeric, "<span class='numeric'>"},
	{hlescseq, "<span class='escseq'>"},
	{hlerror, "<span class='error'>"}
};
// note: initializing maps as above requires the -std=c++0x compiler flag,
// as well as #include<initializer_list>.  Very convenient though.
// to save some typing, store a variable for the end of these tags:
string spanend = "</span>";

string translateHTMLReserved(char c) {
	switch (c) {
		case '"':
			return "&quot;";
		case '\'':
			return "&apos;";
		case '&':
			return "&amp;";
		case '<':
			return "&lt;";
		case '>':
			return "&gt;";
		case '\t': // make tabs 4 spaces instead.
			return "&nbsp;&nbsp;&nbsp;&nbsp;";
		default:
			char s[2] = {c,0};
			return s;
	}
}

int main() {
	// TODO: write the main program.
	// It may be helpful to break this down and write
	// a function that processes a single line, which
	// you repeatedly call from main().
	string s, x,cword;
	while(getline(cin,s)){

    	int cstate = start;
        for(unsigned int i = 0; i < s.length();i++){
			int oldstate = cstate;
			updateState(cstate,s[i]);
          // cout << cstate << "\t" << s[i]<< "\t" << cword << endl;   ////////////////// (Tester for which case there is a bug)
            switch(cstate){
            
                case start:
                  if(cword != ""){
				  	map<string, short>::iterator it;
				  	it = hlmap.find(cword);
					if(it != hlmap.end()){
						x += hlspans[it->second] + cword + spanend;
						cword = "";
					   }
					else{
						x += cword;
						cword = "";
						}
					}
					if(oldstate == strlit)
						 x += hlspans[hlstrlit] + s[i] + spanend;
					else
				 		  x += translateHTMLReserved(s[i]);
                    break;

                case scanid:
                    cword += s[i];
                    break;

                case comment:
                    x += hlspans[hlcomment] + s[i] + spanend;
                    break;

                case strlit:
					if(oldstate == readesc){
						x += hlspans[hlescseq] + cword + s[i] + spanend;
						cword = "";
					}
					else
                   		x += hlspans[hlstrlit] + s[i] + spanend;
                    break;

                case readfs:
					  cword += s[i];
					  if(oldstate == readfs){
					  	x += cword;
						cword = "";
					  }
					  else{
					  	x += hlspans[hlcomment] + cword + spanend;
						cword = "";
					  }

					break;

                case readesc:
                    cword += s[i];
                    break;

                case scannum:
                    x+= hlspans[hlnumeric] + s[i] + spanend;
                    break;

                case error:
					if(oldstate == readesc){
						x +=  hlspans[hlerror] + cword + s[i] + spanend;
						cword = "";
					}
					else
                   		x += hlspans[hlerror] + s[i] + spanend;
             		break;
    
            }
			
        }
    
        x+= '\n';
}
    cout << x << endl;
	return 0;
}
