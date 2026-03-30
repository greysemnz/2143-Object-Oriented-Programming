#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

vector<string> valid= {"--grayscale","--brightness","--rotate","--blur"};

bool argTester(string arg){
    for(auto &item : valid){
        if(arg==item){
            return true;
        }
    }
    return false;
}

int main(int argc, char** argv){

    for(int i=0;i<argc;i++){
        cout<<i<<". "<<argv[i]<<endl;
    }

    





    ////////////////////////////////////////////////////////////////////////////////////
    // if(argc < 2){
    //     cout<<"Error: need filename\n";
    //     cout<<"Usage: ./deux <filename>\n";
    //     exit(1);
    // }

    // ifstream fin;
    // fin.open(argv[1]);
    // int x;

    // while(!fin.eof()){
    //     fin>>x;
    //     cout<<x<<" ";
    // }

    ////////////////////////////////////////////////////////////////////////////////////
    // cout<<argc<<endl;
    // cout<<argv[0]<<endl;
}