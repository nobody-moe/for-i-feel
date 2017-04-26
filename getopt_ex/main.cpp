#include <iostream>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

struct ParamType{
    static const char INT       = 'i';
    static const char BOOL      = 'b';
    static const char FLOAT     = 'f';
    static const char STRING    = 's';
    char val;

    ParamType(char c = 0){val = c;}
    operator char&() {return val;}
};

// struct for store parameters
struct  Param {
    string      name;
    bool        required;
    string      help;
    ParamType   type;
    string      str_val;

    Param(){}
    Param(string name, bool required, char type, string help){
        this->name = name;
        this->required = required;
        this->help = help;
        this->type = type;
    }

    // validate type of parameter
    string testType(){

        switch(type){

        case ParamType::BOOL:
            transform(str_val.begin(), str_val.end(), str_val.begin(), ::toupper);
            if(str_val == "TRUE" || str_val == "FALSE")
                return "";
            return str_val + " is not valid bool value";

        case(ParamType::FLOAT):
            try{ stof(str_val); return "";} catch(...){}
            return str_val + " is not float value";

        case ParamType::INT:
            try{ stoi(str_val); return "";} catch(...){}
            return str_val + " is not int value";

        case ParamType::STRING:
            return "";

        default :
           return string(1, type) + " Not valid type for parameter";

        }
        return "";
    }
};

// getopt analog function
string getopt(int argc, char *argv[], map<string, Param> &params){
    const string prefix("--");
    string error;

    for (int i = 0; i < argc; i++){

        string s = string(argv[i]);
        if(s.substr(0,2) == prefix){
            s = s.substr(2);

            // --- help ---
            if(s == "help"){
                string use_str;
                cout <<"--------- help --------- " << endl;
                for(auto const &it : params) {
                    cout << it.second.name << ":\t" << it.second.help << endl;
                    if(it.second.required)
                        use_str += "--" + it.second.name + " " + it.second.name + "_value ";
                    else
                        use_str += "[--" + it.second.name + " " + it.second.name + "_value] ";
                }
                cout <<"-------- usage --------- " << endl;
                cout << argv[0] << " " << use_str  << endl;
                cout <<"------------------------ " << endl;
            }

            // --- params ---
            auto it = params.find(s);
            if(it != params.end()){
                Param &p = it->second;
                if( (i < argc - 1) && (string(argv[i+1]).substr(0,2) != prefix)){
                    p.str_val = argv[i+1];
                    error += p.testType();
                }

            }
        }
    };

    for(auto const &it : params) {
        if(it.second.str_val.empty() && it.second.required)
            error += it.second.name + " is empty" + "\n";
    }

    return error;
}


int main(int argc, char *argv[])
{
    cout << "--- Getopt Test ---" << endl;

    // init params
    map<string, Param> params;
    params[string("param_1")] = Param("param_1", true,  ParamType::STRING, "help for param_1");
    params[string("param_2")] = Param("param_2", true,  ParamType::BOOL,   "help for param_2");
    params[string("param_3")] = Param("param_3", false, ParamType::INT,    "help for param_3");

    // call getopt
    string err_string = getopt(argc, argv, params);

    if(! err_string.empty())
        cout << "Error in parameters" << endl << err_string << endl;

    // do something with parameters
    else{
        cout << "Parameters setted" << endl;
        for(auto const &it : params) {
            cout << it.second.name <<": " << it.second.str_val << endl;
        }
    }

    string s;
    cin >> s;

    return 0;
}
