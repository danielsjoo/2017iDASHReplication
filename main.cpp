#include "headers.h"
using namespace std;
using namespace seal;

void printresult(vector<double>);
void print_result_full(vector<double>);
void printrow(vector<double>, int);

int main(int argc, char *argv[]){

    /* IMPORT OF DATA */
    #pragma region
    if(argc != 2){
        cerr << "Invalid number of parameters" << endl;
        return 1;
    }
    cout << "\nParsing through data..." << endl;
    dataTools data_package = dataTools(argv[1], 683, 10);
    cout << "done" << endl;
    //dataTools::print3rows(data_package.train_data_padded,16);
    cout << "\nmoving on to intializing SEAL..." << endl;
    #pragma endregion

    /* 
    SETUP FOR CKKS ENCRYPTION:
        poly_modulus_degree   : 2^15 = 1024
        coeffModulus bit sizes: {50,30,30,30,30,30,30,30,50}
        scale                 : pow(2.0,30)
    */
    #pragma region
    EncryptionParameters parms(scheme_type::ckks);

    size_t poly_modulus_degree = 1024; //purposely broke
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, {50,30,30,30,30,30,30,30,50}));

    SEALContext context(parms);

    //print_parameters(context);
    //cout << endl;

    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    GaloisKeys galois_keys;
    keygen.create_galois_keys(galois_keys);
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    //cout << "check" <<endl;

    CKKSEncoder encoder(context);
    size_t slot_count = encoder.slot_count();
    //cout << "Number of slots: " << slot_count << endl;

    auto scale = pow(2.0, 30);

    cout << "done\n" << endl; 
    #pragma endregion

    /* SETUP FOR TRAINING LOOP */
    #pragma region SETUP
    cout << "0. BEFORE ANY CALCULATION" << endl;
    cout << "the first three rows of the data (ctz)" << endl;
    dataTools::print3rows(data_package.train_data_padded,16);
    cout << "the first three rows of the intial weights (ct0)" << endl;
    dataTools::print3rows(data_package.random,16);
    cout << endl;

    Plaintext data;
    encoder.encode(data_package.train_data_padded, scale, data);
    Ciphertext ctz;
    encryptor.encrypt(data, ctz);
    Plaintext rands;
    Ciphertext ct0;
    encoder.encode(data_package.random, scale, rands);
    encryptor.encrypt(rands, ct0);
    Ciphertext ct1;
    Ciphertext t1;
    Ciphertext t2;
    Plaintext temp_plaintext; // for printing intermediate results
    vector<double> temp_vector;
    #pragma endregion

    /* TRAINING LOOP */
    int epochs = 4;
    double learning_rate = 10;
    for (int i=0; i<epochs; i++){

        cout << "__________________" << "EPOCH " << i+1 << "__________________\n" << endl;
    
        //step 1
        evaluator.mod_switch_to(ctz, ct0.parms_id(), t1);
        evaluator.multiply(t1, ct0, ct1);
        evaluator.relinearize_inplace(ct1, relin_keys);
        evaluator.rescale_to_next_inplace(ct1);
        decryptor.decrypt(ct1, temp_plaintext); //now level 6
        encoder.decode(temp_plaintext, temp_vector);
        cout << "1. ct1 AFTER STEP 1 (first 3 rows)" << endl;
        printresult(temp_vector);

        //step 2 
        for (int j=0; j < log(data_package.cols_padded) / log(2); j++) {
            evaluator.rotate_vector(ct1, pow(2.0,j), galois_keys, t1);
            evaluator.add_inplace(ct1, t1);
        }
        decryptor.decrypt(ct1, temp_plaintext);
        encoder.decode(temp_plaintext, temp_vector);
        cout << "\n2. ct1 AFTER STEP 2" << endl;
        //print_result_full(temp_vector);
        printresult(temp_vector);
        //print_result_full(data_package.annihilation);

        //step 3
        encoder.encode(data_package.annihilation, scale, temp_plaintext);
        evaluator.mod_switch_to_inplace(temp_plaintext, ct1.parms_id());
        evaluator.multiply_plain_inplace(ct1, temp_plaintext);
        evaluator.relinearize_inplace(ct1,relin_keys);
        evaluator.rescale_to_next_inplace(ct1); 

        decryptor.decrypt(ct1, temp_plaintext);
        encoder.decode(temp_plaintext, temp_vector);
        cout << "\n3. ct1 AFTER STEP 3" << endl;
        printresult(temp_vector);

        //step 4
        for (int j=0; j < log(data_package.cols_padded) / log(2); j++) {
            evaluator.rotate_vector(ct1, -1 * pow(2.0,j), galois_keys, t1);
            evaluator.add_inplace(ct1, t1);
        }
        decryptor.decrypt(ct1, temp_plaintext);
        encoder.decode(temp_plaintext, temp_vector);
        cout << "\n4. ct1 AFTER STEP 4" << endl;
        printresult(temp_vector);

        /* STEP 5 */
        vector<double> temp_vector2;
        for (double i : temp_vector){
            temp_vector2.push_back(1/(1+ exp(i)));
        }
        encoder.encode(temp_vector2, scale, temp_plaintext);
        encryptor.encrypt(temp_plaintext, ct1);
        cout << "\n5. ct1 AFTER STEP 5 (PERFORMED IN PLAINTEXT)" << endl;
        printresult(temp_vector2);

        //calculate 0.81562 * (1/8)^3 * x^2 = 0.00159301 * (x^2)

        //add with -1.20096/8 = -0.15012

        //multiply by x

        //add with 0.5

        //step 6
        evaluator.mod_switch_to(ctz, ct1.parms_id(), t1);
        evaluator.multiply_inplace(ct1, t1);
        evaluator.relinearize_inplace(ct1, relin_keys);
        evaluator.rescale_to_next_inplace(ct1); // now level 4
        decryptor.decrypt(ct1, temp_plaintext);
        encoder.decode(temp_plaintext, temp_vector);
        cout << "\n6. ct1 AFTER STEP 6" << endl;
        printresult(temp_vector);

        //step 7
        int fplus1 = log(data_package.cols_padded) / log(2);
        for (int j = fplus1; j <= fplus1 + log2(32) -1 ; j++){
            evaluator.rotate_vector(ct1, pow(2.0,j), galois_keys, t1);
            evaluator.add_inplace(ct1, t1);
        }
        decryptor.decrypt(ct1, temp_plaintext);
        encoder.decode(temp_plaintext, temp_vector);
        cout << "\n7. ct1 AFTER STEP 7" << endl;
        printresult(temp_vector);

        //step 8
        Plaintext lr_coeff;
        encoder.encode(learning_rate, scale, lr_coeff);
        evaluator.mod_switch_to_inplace(lr_coeff, ct1.parms_id());
        evaluator.multiply_plain_inplace(ct1, lr_coeff);
        evaluator.relinearize_inplace(ct1,relin_keys);
        evaluator.rescale_to_next_inplace(ct1);
        evaluator.mod_switch_to_inplace(ct0, ct1.parms_id());
        ct1.scale() = scale;
        evaluator.add_inplace(ct0, ct1);
        decryptor.decrypt(ct0, temp_plaintext);
        encoder.decode(temp_plaintext, temp_vector);
        cout << "\n8. ct0 AFTER STEP 8 (did not implement learning rate)" << endl;
        printresult(temp_vector);
        cout << endl;

    }

    cout << "The intial parameters were:";
    printrow(data_package.random, 10);
    cout << endl;
    cout << "The final parametrs were:";
    printrow(temp_vector, 10);
    cout << endl;

    return 0;

}

#pragma region helper functions

void printresult(vector<double>result){
    for (int i=0; i<result.size(); i++){
        result[i] = round((result[i])*10000)/10000; //change
    }
    dataTools::print3rows(result,16);
}

void print_result_full(vector<double>result){
    for (int i=0; i<result.size(); i++){
        result[i] = round((result[i])*10000)/10000; //change
    }
    dataTools::print(result);
}

void printrow(vector<double>result, int cols){
    for (int i=0; i<result.size(); i++){
        result[i] = round((result[i])*100)/100; //change
    }
    for (int i=0; i<cols; i++){
        cout << result[i] << " ";
    }
}

#pragma endregion