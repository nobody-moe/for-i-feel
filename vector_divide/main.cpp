#include <iostream>
#include <vector>

using namespace std;

vector< pair<int, int> > divide_vector( int N, int M){
    vector< pair<int, int> > divs(M);
    int length = N/M;
    int first  = (N%M)/2;

    for(int i = 0; i < M; i++){
        divs[i] = pair<int, int>(first+i*length, first+(i+1)*length-1);
    }
    return divs;
}


int main(int argc, char *argv[])
{
    cout << "Divide Vector" << endl;

    int N, M;
    while(true){
        cout << "-----------------" << endl;
        cout << "N = ";
        cin >> N;

        cout << "M = ";
        cin >> M;

        cout << " N = " << N << " M = " << M << endl;

        if(N > M && M > 0){
            for(auto const &it : divide_vector(N, M))
                cout << it.first << ":" << it.second << endl;
        }
        else {
            cout << "bye";
            break;
        }
    }

    return 0;
}
