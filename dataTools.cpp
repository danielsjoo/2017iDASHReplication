#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <sstream>

using namespace std;

class dataTools{

    public:
        int rows;
        int cols;
        int train_rows;
        int test_rows;
        int train_rows_padded; 
        int cols_padded;
        int test_rows_padded;
        vector<double> data;
        vector<double> train_data;
        vector<double> test_data;
        vector<double> train_data_padded;
        vector<double> test_data_padded;
        vector<double> random;
        vector<double> annihilation;
        dataTools(char *file, int size, int fields);
    
    string toString(){
        return "Size of data: " + to_string(rows) + "x" + to_string(cols)
            + "\nTrain set has " + to_string(train_rows) + " rows"
            + "\nTest set has " + to_string(test_rows) + " rows"
            + "\nTrain_padded set has " + to_string(train_rows_padded) + " rows";
    }

    static void print(std::vector <double> const &a) {
        std::cout << "The vector elements are : ";
        for(int i=0; i < a.size(); i++)
            std::cout << a.at(i) << ' ';
        cout << endl;
        cout << "overall size: " << a.size() << endl;
    }

    void initialWeights(int rows, int cols){
        double initialparams[cols];
        rand();
        for (int i=0; i<cols; i++){
            initialparams[i] = round(((double)rand()/RAND_MAX)*100)/100;
        }
        for (int i=0; i<rows; i++){
            for (int j=0; j<cols; j++){
                random.push_back(initialparams[j]);
            }
        }
    }

    static void print3rows(std::vector <double> const &a, int cols){
        for(int i = 0; i<3*cols; i++){
            if(i%cols==0 && i!= 0){
                cout << endl;
            }
            cout << a[i] << " ";
        }
        cout << endl;
    }

};

dataTools::dataTools(char *file, int size, int fields){

    /* READ FILE */
    fstream myFile;
    myFile.open(file, ios::in);
    if(myFile.is_open()){
        string line;
        while (getline(myFile, line)){
            string buffer;
            stringstream s(line);
            while (getline(s, buffer, ',')){
                data.push_back(std::stod(buffer));
            }
        }
        myFile.close();
    }
    //print(data);

    /* SPLIT DATA */
    rows = size;
    cols = fields;
    train_rows = ceil(rows * 0.04);
    test_rows = floor(rows * 0.60);
    //cout << rows << " " << train_rows << " " << test_rows << endl;
    int counter = 0;
    for (double i: data){
        if(counter < train_rows * cols){
            train_data.push_back(i);
        }else{
            test_data.push_back(i);
        }
        counter++;
    }
    //print(train_data);
    //print(test_data);

    /* PAD DATA */
    train_rows_padded = pow(2.0,ceil(log(train_rows)/log(2)));
    test_rows_padded = pow(2.0,ceil(log(test_rows)/log(2)));
    cols_padded = pow(2.0,ceil(log(cols)/log(2)));
    counter = 0; 
    for (double i : train_data){
        while(counter % cols_padded >= cols){
            train_data_padded.push_back(0);
            counter++;
        }
        train_data_padded.push_back(i);
        counter++;
    }
    while(counter % cols_padded >= cols){
        train_data_padded.push_back(0);
        counter++;
    }
    
    counter = 0;
    for (double i : test_data){
        while(counter % cols_padded >= cols){
            test_data_padded.push_back(0);
            counter++;
        }
        test_data_padded.push_back(i);
        counter++;
    }
    while(counter % cols_padded >= cols){
        test_data_padded.push_back(0);
        counter++;
    }
    //print(train_data_padded);

    initialWeights(train_rows, cols_padded);

    for (int i=0; i < 512; i++){
        if (i%cols_padded == 0){
            annihilation.push_back(1);
        }else{
            annihilation.push_back(0);
        }
    }
    //print(annihilation);

    //print(random);
    //print(train_data_padded);

}