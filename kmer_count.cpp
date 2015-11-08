#include <bits/stdc++.h>
using namespace std;

#define K 20
#define sample 100
int main(){
	string reference;
    cout<<"Reading the ref"<<endl;
	cin>>reference;
    cout<<"Reading the ref"<<endl;
	unordered_map<string,long> m;
	for (auto it = reference.begin(); it <= reference.end() - K; it+=10) {
			std::string temp(it, it + K);
			//TODO: this if needed?
			if(std::distance(reference.begin(),it)%10000000 == 0){
				cout<<(std::distance(reference.begin(),it))<<" k mers inserted"<<endl;
			}
			m[(temp)]++;
	}

	vector<long> kmer_count;
	for(auto it :m){
		kmer_count.push_back(it.second);
	}

    cout<<"Sorting.."<<endl;
	sort(kmer_count.begin(),kmer_count.end(),[](int i,int j){return (i>j);});
    
    cout<<"Sorting.."<<endl;
	long size = kmer_count.size();
	vector<float> fraction{0.00001,0.0001,0.001,0.01,0.02,0.05};
	cout<<"Total kmers "<<kmer_count.size();
	cout<<"Displaying kmer counts "<<endl;
    for (auto it:fraction){
        long i = size*it;
        cout<<it<<" "<<kmer_count[i]<<endl;
    }
    /*top_kmers = 10;
	for(auto it: kmer_count){
		if(top_kmers){
			cout<<it<<endl;
		}
        else
            break;
        top_kmers--;		
	}*/
	return 0;
}
