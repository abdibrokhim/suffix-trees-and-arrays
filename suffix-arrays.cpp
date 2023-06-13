


#include <iostream>
#include <algorithm>
#include <cstring>
#include <chrono>
using namespace std;
using namespace std::chrono;

struct suffix {
   int index;
   string suff;
};

int strCompare(string st1, string st2, int n) {
   int i = 0;
   while(n--) {
      if(st1[i] != st2[i])
         return st1[i] - st2[i];
      i++;
   }
   return 0;
}

//compare two strings for sorting
bool comp(suffix suff1, suffix suff2) { 
   if(suff1.suff<suff2.suff)
      return true;
   return false;
}

void fillSuffixArray(string mainString, int suffArr[]) {
   int n = mainString.size();
   suffix allSuffix[n]; //array to hold all suffixes

   for(int i = 0; i<n; i++) {
      allSuffix[i].index = i;
      //from ith position to end
      allSuffix[i].suff = mainString.substr(i); 
   }

   sort(allSuffix, allSuffix+n, comp);
   for(int i = 0; i<n; i++)
   //indexes of all sorted suffix
      suffArr[i] = allSuffix[i].index; 
}

void suffixArraySearch(
    string mainString, string pattern, int suffArr[], 
    int array[], int *index) {
   int patLen = pattern.size();
   int strLen = mainString.size();
   
   //left and right for binary search
   int left = 0, right = strLen - 1; 

   while(left <= right) {
      int mid = left + (right - left)/2;
      string tempStr = mainString.substr(suffArr[mid]);
      int result = strCompare(pattern,tempStr, patLen);
        
      //the pattern is found
      if(result == 0) { 
        (*index)++;
        array[(*index)] = suffArr[mid];
      }

      if(result < 0)
         right = mid -1;
      else
         left = mid +1;
   }
}


int main() {
    string mainString = "THIS IS A TEST TEXT$";
    string pattern = "THIS";
    int locArray[mainString.size()];
    int index = -1;
    
    int suffArr[mainString.size()];
    
    auto start = high_resolution_clock::now();
    fillSuffixArray(mainString, suffArr);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    
    cout << "Construction Time: " 
    << duration.count() 
    << " microseconds" << "\n";
    
    start = high_resolution_clock::now();
    suffixArraySearch(mainString, pattern, suffArr, locArray, &index);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    
    cout << "Substring Search Time: " 
    << duration.count() << " microseconds" << "\n";
    
    cout << "Pattern found at positions: ";
    for (int i = 0; i <= index; i++) {
        cout << locArray[i] << " ";
    }   
    cout << "\n";
    
    int spaceUsage = sizeof(mainString) 
    + sizeof(suffArr) + sizeof(locArray) + sizeof(index);
    cout << "Space Usage: " 
    << spaceUsage << " bytes" << "\n";
    
    return 0;
}