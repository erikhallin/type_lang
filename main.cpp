#define _WIN32_WINNT 0x0500
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

using namespace std;

struct st_word
{
    string theword;
    string thetype;
};

int rand_val(int upper_limit);
void toClipboard(HWND hwnd, const std::string &s);

int main()
{
    /*
    //TESTING ø
    //if(isascii('å')) cout<<"hej"<<endl;
    string printval="-x21";
    for(int i=0;i<100;i++)
    {
        printval[0]='\\';
        cout<<printval[3]<<"\t";

    }
    cout<<endl;

    char inp;
    cin>>inp;
    cout<<inp<<"  -<"<<endl;
    string x16="ø";
    cout<<x16<<endl;

    char xxx='ø';
    cout<<xxx<<endl;
    cout<<"\x91 \x86 - \xED \xef --- \xea \xeb \xec \xed"<<endl; //æ (ø) å
    cout<<"Starting"<<endl;
    string test="feff00f8";
    cout<<test<<endl;
    */

    cout<<"Starting"<<endl;
    srand(time(0));

    HWND hwnd_console=GetConsoleWindow();
    //toClipboard(hwnd_console,"test");

    //load text database
    cout<<"Loading database...";
    ifstream infile("liste.txt");
    if(infile==0)
    {
        cout<<"ERROR: Did not find the database\n";
        return 1;
    }
    //fill list
    vector<st_word> vec_list;
    string line;
    getline(infile,line); //skip first line
    while(getline(infile,line))
    {
        //find word
        //ignore first 2 tabs and stop at the third
        int tab_count=0;
        for(int i=0;i<(int)line.size();i++)
        {
            if(line[i]=='\t') tab_count++;
            bool have_word=false;
            string word, type;
            if(tab_count==2)
            {
                for(int j=i+1;j<(int)line.length();j++)
                {
                    if(line[j]=='\t')
                    {
                        have_word=true;
                        i=j;
                        break;
                    }
                    if(line[j]=='å') line[j]='\x86';
                    if(line[j]=='æ') line[j]='\x91';
                    if(line[j]=='ø') line[j]='\xed';
                    if(line[j]=='é') line[j]='\x82';
                    if(line[j]=='è') line[j]='\x8a';
                    if(line[j]=='à') line[j]='\x85';
                    if(line[j]=='ê') line[j]='\x88';
                    word.append(1,line[j]);

                }
            }
            if(have_word)
            {
                //save word type
                for(int j=i+1;j<(int)line.size();j++)
                {
                    if(line[j]==' ')
                    {
                        break;
                    }
                    type.append(1,line[j]);
                }
                st_word new_word;
                new_word.theword=word;
                new_word.thetype=type;
                //if(type!="adj")
                vec_list.push_back(new_word);
                break;
            }
        }
    }
    infile.close();
    cout<<" done\n";
    cout<<" Database contains "<<(int)vec_list.size()<<" words\n\n";

    //start game loop
    int rounds_counter=0;
    float time_start=clock();
    cout<<">Words will appear - Type them and press ENTER\n\n>Type q to quit\n>Press ENTER to start\n";
    string command;
    getline(cin,command);
    if(command=="q") ; //quit
    else while(true) //start
    {
        rounds_counter++;
        //pick random word
        int word_ind=rand_val((int)vec_list.size());
        //cout<<word_ind<<endl;
        string totype=vec_list[word_ind].theword;
        //if(vec_list[word_ind].thetype=="adj") continue;
        //print word
        cout<<"> "<<totype<<" <";
        //balance length with whitespaces
        for(int s=20;s-(int)totype.length()>0;s--) cout<<" ";
        cout<<"\t("<<vec_list[word_ind].thetype<<")\n> ";

        string input;
        getline(cin,input);
        if(input=="q") break; //quit

        //move word to clipboard
        toClipboard(hwnd_console,totype);

        //test if correct
        bool correct=true;
        for(int i=0;i<(int)totype.length();i++)
        {
            if(totype[i]!=input[i])
            {
                //å æ ø exception
                     if(totype[i]=='\xed' && input[i]=='o') ;//ø is ok
                else if(totype[i]=='\xed' && input[i]=='\x94') ;//ö is ok
                else if(totype[i]=='\x91' && input[i]=='\x84') ;//ä is ok
                else
                {
                    correct=false;
                    break;
                }
            }
        }
        if(correct)
        {
            cout<<"  CORRECT\n\n";
        }
        else
        {
            //print score
            float time_play=clock()-time_start;
            float time_per_round=time_play/(float)rounds_counter/1000.0;
            cout<<"  -WRONG- Press ENTER to start again\n";
            cout<<"Score: "<<rounds_counter<<"\tWords\t"<<time_per_round<<" Seconds per word\n";

            getline(cin,input);
            if(input=="q") break; //quit

            time_start=clock();//restart time
        }
        //next word
    }


    //quit
    cout<<"Shutting down\n";

    return 0;
}

int rand_val(int upper_limit)
{
    int retval=upper_limit;
    while(retval>=upper_limit)
    {
        int subval=rand();
        int block_max=int((float)upper_limit/(float)RAND_MAX);
        if(block_max<1) return rand()%upper_limit;
        int blockval=rand()%(block_max);
        retval=blockval*RAND_MAX+subval;
    }

    return retval;
}

void toClipboard(HWND hwnd, const std::string &s)
{
	OpenClipboard(hwnd);
	EmptyClipboard();
	HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,s.size()+1);
	if (!hg){
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg),s.c_str(),s.size()+1);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT,hg);
	CloseClipboard();
	GlobalFree(hg);
}
