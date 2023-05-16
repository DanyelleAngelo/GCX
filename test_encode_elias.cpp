//entendendo elias gama: https://docs.google.com/document/d/11RU0-33qzbvWtASmNyUiNel7zez_RDMRFZY-q3zyd6M/edit
#include <vector>

#include <sdsl/vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/coder.hpp>

using namespace std;
using namespace sdsl;

int main() {
    coder::elias_gamma eg;

    int_vector<> v = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    int_vector<> encoded;
    int_vector<> decoded;


    //encoded = int_vector<>(v);
    cout << "Codificando array com sdsl:\n";
    eg.encode(v, encoded);
    for(uint64_t i=0; i < encoded.size(); ++i)
        cout << encoded[i] << ",";
    cout << endl;

    cout << "Decodificando array com sdsl:\n";
    eg.decode(encoded, decoded);
    for(uint64_t i=0; i < decoded.size(); ++i)
        cout << decoded[i] << ",";
    cout << endl;

    cout << "Armazenando texto usando store da sdsl...\n";
    store_to_file(encoded,"store-text-elias.txt");
    cout << endl;

    cout << "Lendo texto armazenado usando sdsl...\n";
    int_vector<> read_text;
    load_from_file(read_text, "store-text-elias.txt");
    for(uint64_t i=0; i < read_text.size(); ++i)
        cout << read_text[i] << ",";
    cout << endl;
    
    cout << "Decodificando array lido de arquivo usando a sdsl...\n";
    int_vector<> decoded_text;
    eg.decode(read_text, decoded_text);
    for(uint64_t i=0; i < decoded_text.size(); ++i)
        cout << decoded_text[i] << ",";
    store_to_file(v,"store-text-elias.txt");

    // cout << "\nTeste problema na gramática:\n";
    // int_vector<> a = {14663720413492674580,65912984043850,1028346837206987792};
    // int_vector<> b2;
    // //int_vector<64> b;
    // //eg.encode(a, b2);
    // eg.decode(a, b2);
    // for(int i=0; i < b2.size(); i++)
    //     cout << b2[i] << ",";
    // cout << endl;
    // cout << endl;
}