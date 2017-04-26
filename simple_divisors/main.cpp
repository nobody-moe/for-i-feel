#include <iostream>
#include <vector>
#include <string>

using namespace std;

vector< vector<int> > simple_divisors( int N){

    vector< vector<int> > divs(N);
    vector<int> numbers(N);

    for(int i = 0; i < N; i++)
        numbers[i] = i+1;

    divs[0].push_back(1);

    if(N > 1)
    for(int i = 1; i < N; i++){
        int val = i + 1;
        if(numbers[i] != 1)
        for(int j = i; j < N; j+=val){
            do{
                divs[j].push_back(val);
                numbers[j] /= val;
            } while (numbers[j] != 1 && numbers[j]%val == 0);
        }
    }

    return divs;
}

int main(int argc, char *argv[])
{
    cout << "Simple Divisors" << endl;

    int N;
    while(true){
        cout << "-----------------" << endl;
        cout << "N = ";
        cin >> N;

        cout << "N = " << N << endl;

        if(N > 1){

            std::vector< std::vector<int> > d = simple_divisors(N);

            for(int i = 0; i < d.size(); i++){
                string s = to_string(i+1) + ": ";
                for(int j = 0; j < d[i].size(); j++)
                    s += to_string(d[i][j]) + " ";
                cout << s << endl;
            }
        }
        else {
            cout << "bye";
            break;
        }
    }

    return 0;
}
