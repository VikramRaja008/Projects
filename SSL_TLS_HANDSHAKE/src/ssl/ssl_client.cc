#include "ssl_client.h"
#include "secblock.h"
#include "channels.h"
#include "stdlib.h"
#include "string.h"
#include<sstream>
#include <iostream>
#include <iomanip>
#include "dh.h"
#include "integer.h"
#include "osrng.h"
#include "hkdf.h"
#include "sha.h"
#include "tcp.h"
#include "crypto_adaptor.h"
#include "logger.h"
#include "utils.h"
#include "elgamal.h"
#include "integer.h"
#include "sha.h"

#include "hmac.h"
#include "hkdf.h"
#include "osrng.h"
#include "hex.h"
#include "filters.h"
#include "cryptlib.h"
#include "rsa.h"
#include "base64.h"

#include "files.h"

using namespace std;
using namespace CryptoPP;

 AutoSeededRandomPool rng;

void derive_key(const SecByteBlock &recoveredPremaster,
                const SecByteBlock &serverRandom,
                const SecByteBlock &clientRandom,
                size_t keySize,
                SecByteBlock &key) {
    // Concatenate server random and client random to form the seed
    SecByteBlock seed(serverRandom.size() + clientRandom.size());
    memcpy(seed.data(), serverRandom.data(), serverRandom.size());
    memcpy(seed.data() + serverRandom.size(), clientRandom.data(), clientRandom.size());

    // Derive the key using HKDF with HMAC-SHA256
    HKDF<SHA256>().DeriveKey(key.data(), key.size(),
                             recoveredPremaster.data(), recoveredPremaster.size(),
                             seed.data(), seed.size(),
                             reinterpret_cast<const byte*>("key expansion"), strlen("key expansion"));
}




string base64_encode(const string &input) {
    string base64Encoded;
    StringSource ss(input, true,
                    new Base64Encoder(
                        new StringSink(base64Encoded),
                        false // Do not insert line breaks
                    ));
    return base64Encoded;
}

CryptoPP::SecByteBlock Base64StringToSecByteBlock(const string& base64Str) {
    string decodedStr;
    CryptoPP::StringSink stringSink(decodedStr);
    CryptoPP::StringSource(base64Str, true,
        new CryptoPP::Base64Decoder(new CryptoPP::Redirector(stringSink)));

    CryptoPP::SecByteBlock decoded(reinterpret_cast<const unsigned char*>(decodedStr.data()), decodedStr.size());
    return decoded;
}

SslClient::SslClient() {
  string datetime;
  if ( get_datetime(&datetime, "%Y%m%d-%H%M%S") != 0 ) {
    exit(1);
  }
  this->logger_ = new Logger(("ssl_client_"+datetime+".log"));
  this->tcp_->set_logger(this->logger_);

  get_datetime(&datetime, "%Y/%m/%d %H:%M:%S");
  this->logger_->log("Client Log at " + datetime);


}

SslClient::~SslClient() {
  if ( this->logger_ ) {
    delete this->logger_;
    this->logger_ = NULL;
    this->tcp_->set_logger(NULL);
  }
}

int SslClient::connect(const string &ip, int port, uint16_t cxntype) {

  // connect
  if ( this->tcp_->socket_connect(ip, port) != 0 ) {
    cerr << "Couldn't connect" << endl;
    return -1;
  }


         // IMPLEMENT HANDSHAKE HERE
if (cxntype == SSL::KE_RSA){
	
//=====================CLIENT_HELLO_SEND========================//
    cout <<"C: DHE_IMPLEMNTATION "<<endl;
	
	// hexadecimal to sring (hello handshake)	

    uint8_t hex_input = SSL::HS_CLIENT_HELLO;
    stringstream stream;
    stream << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_input);
    string hex_string1 = stream.str();
    //cout << "Hexadecimal string: " << hex_string1 <<endl;

	
	// hexadecimal to sring (cipher tpe)	

    uint8_t hex_input1 = SSL::KE_RSA;
    stringstream stream1;
    stream1 << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_input1);
    string hex_string2 = stream1.str();
   // cout << "Hexadecimal string: " << hex_string2 <<endl;
    

    //generating client random 

SecByteBlock clientRandomBlock(32);
rng.GenerateBlock(clientRandomBlock, 32);

CryptoPP::Base64Encoder base64Encoder;
string clientRandomBase64Str;
base64Encoder.Attach(new CryptoPP::StringSink(clientRandomBase64Str));
base64Encoder.Put(clientRandomBlock.data(), clientRandomBlock.size());
base64Encoder.MessageEnd();

  string clientRandomStr(reinterpret_cast<const char*>(clientRandomBlock.data()), clientRandomBlock.size());

//cout<<"C: client random: "<<clientRandomStr<<endl;


    Record send_record1;
    send_record1.hdr.type = REC_HANDSHAKE;
    send_record1.hdr.version = VER_99;
    string body1 = hex_string1 +","+ hex_string2+","+ clientRandomBase64Str; 
    char* data1 = (char*)malloc(body1.length()*sizeof(char));
    memcpy(data1, body1.c_str(), body1.length());
    send_record1.data = data1;
    //cout<<"C: data sent is : "<<body1<<endl;
    send_record1.hdr.length = body1.length();
	  this->send(send_record1);
    //int ret_code1 = this->send(send_record1);
   // cout<<"C: client sent code: "<<ret_code1<<endl;
	
//===================SERVER_HELLO_RECEIVED==============//
    //cout << this->get_port()<<endl;
    Record recv_record1;
    this->recv(&recv_record1);
    cout<<"\n";
    //cout<<"C: data recieved: "<<recv_record1.data<<endl;

//===============================SLICING============================//

    string concatenated_string = recv_record1.data;
    vector<string> split_strings;
    stringstream ss(concatenated_string);
    string token;
    char delimiter = ',';

    while (getline(ss, token, delimiter)) {
        split_strings.push_back(token);
    }

    // Convert the vector to an array
    string string_array;
    string_array = split_strings[1];
   // cout<<"C: Sliced_String: "<<string_array<<endl;
//===============================SLICING============================//

//============================HEXING=====================================//
   string hexStr = string_array;
    stringstream ss1;
    ss1 << hexStr;
    int hexVal;
    ss1 >> hex >> hexVal;
   // cout << "C: Hexadecimal value: " << hexStr << endl;
   // cout << "C: Decimal value: " << hexVal << endl;

//============================HEXING=====================================//


string serverRandomBase64=split_strings[2];

Base64Decoder decoder;
decoder.Put(reinterpret_cast<const byte*>(serverRandomBase64.data()), serverRandomBase64.size());
decoder.MessageEnd();

SecByteBlock decodedServerRandomBlock(32);
size_t decodedSize = decoder.MaxRetrievable();
if (decodedSize) {
    decodedServerRandomBlock.resize(decodedSize);
    decoder.Get(decodedServerRandomBlock.data(), decodedServerRandomBlock.size());
}

  string serverRandomStr(reinterpret_cast<const char*>(decodedServerRandomBlock.data()), decodedServerRandomBlock.size());

//cout<<"C: Server Random: "<<serverRandomStr<<endl;


    if(hexVal ==SSL::KE_RSA)
    {
    //  cout<<"C: CLIENT_AFFIRMATIVE"<<endl;
    }
//=============================RECIEVING_PK_FROM_SERVER=============//
    Record recv_record;
    this->recv(&recv_record);
   // cout<<"C: Recieved data: "<<recv_record.data<<endl;
   // cout<<"\n";
//=============================RECIEVING_PK_FROM_SERVER=============//


//===================RECEVING_SERVER_HELLO_DONE====================//

    Record recv_recordsh;
    this->recv(&recv_recordsh);
    cout<<"\n";
    //cout<<"C: data recieved: "<<recv_recordsh.data<<endl;

//===================RECEVING_SERVER_HELLO_DONE====================//


  // Base64-encoded public key
string base64PublicKeyString = recv_record.data;

   string publicKeyString;

    // Decode the base64-encoded public key
    StringSource(base64PublicKeyString, true,
                 new Base64Decoder(
                     new StringSink(publicKeyString)));

    // Decode the DER-encoded public key
    RSA::PublicKey publicKey;
    StringSource publicKeySource(publicKeyString, true);
    publicKey.BERDecodePublicKey(publicKeySource, true /* parametersPresent */, publicKeyString.size());

// Generate premaster secret
SecByteBlock premasterSecret(32); // Assuming a 384-bit premaster secret
AutoSeededRandomPool rng;
rng.GenerateBlock(premasterSecret, premasterSecret.size());

// Convert the premaster secret to a string
string premasterSecretStr(reinterpret_cast<const char*>(premasterSecret.data()), premasterSecret.size());
//cout<<"C: Premaster: "<<premasterSecretStr<<endl;

string plaintext = premasterSecretStr; // the plaintext to be encrypted
string ciphertext; // the output ciphertext

// encrypt the plaintext using the public key
if (rsa_encrypt(publicKey, &ciphertext, plaintext) == 0) {
  // print the ciphertext
// cout<<"\n";

string base64EncodedCiphertext = base64_encode(ciphertext);

//=======================CLIENT_KEY_HS_ATTACH====================//

  
    uint8_t hex_inputc = SSL::HS_CLIENT_KEY_EXCHANGE;
    stringstream streamc1;
    streamc1 << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputc);
    string hex_stringc2 = streamc1.str();

//=======================CLIENT_KEY_HS_ATTACH====================//

     //this is now sending cypher text
    Record send_record1;
    send_record1.hdr.type = REC_HANDSHAKE;
    send_record1.hdr.version = VER_99;
    string body1 = hex_stringc2 +","+base64EncodedCiphertext;
    char* data1 = (char*)malloc(body1.length()*sizeof(char));
    memcpy(data1, body1.c_str(), body1.length());
    send_record1.data = data1;
    send_record1.hdr.length = body1.length();
    this->send(send_record1);
  //int ret_code = this->send(send_record1);
 //   cout<<"sent the cypher text"<<ret_code<<endl;
  //   cout<<"\n";

} else {
  // handle error
  cerr << "Encryption failed." << endl;
}

    SecByteBlock derivedKey; 
    derivedKey.resize(16);


size_t keySize = 16; // Desired size of the derived keys in bytes

// for deriving client key from premaster
SecByteBlock clientKey1(keySize);

derive_key(premasterSecret, decodedServerRandomBlock, clientRandomBlock, keySize, clientKey1);

string clientKeyStr1(reinterpret_cast<const char*>(clientKey1.data()), clientKey1.size());

//cout<<"C: client key: "<<clientKeyStr1<<endl;
   this->set_shared_key(clientKey1.data(), clientKey1.size());

//======================CLIENT_FINISHED_KEY_EXCHANGE===================//
 uint8_t hex_inputfdh = SSL::HS_FINISHED;
    stringstream streamfdh;
    streamfdh << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputfdh);
    string hex_stringfdh1 = streamfdh.str();
    //cout << "Hexadecimal string: " << hex_string1 <<endl;

 Record send_recordfdh1;
    send_recordfdh1.hdr.type = REC_HANDSHAKE;
    send_recordfdh1.hdr.version = VER_99;
    string bodyfdh1 = hex_stringfdh1;  
    char* datafdh1 = (char*)malloc(bodyfdh1.length()*sizeof(char));
    memcpy(datafdh1, bodyfdh1.c_str(), bodyfdh1.length());
    send_recordfdh1.data = datafdh1;
   // cout<<"C: data sent is : "<<bodyfdh1<<endl;
    send_recordfdh1.hdr.length = bodyfdh1.length();
	  this->send(send_recordfdh1);
   // int ret_codefdh1 = this->send(send_recordfdh1);
   // cout<<"C: clien finish: "<<ret_codefdh1<<endl;
}

if (cxntype == SSL::KE_DHE){

  //=========================CLIENT_HELLO_SEND======================//
  cout <<"C: DHE_IMPLEMNTATION "<<endl;

	// hexadecimal to string (hello handshake)	

    uint8_t hex_inputdh = SSL::HS_CLIENT_HELLO;
    stringstream streamdh;
    streamdh << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputdh);
    string hex_stringdh = streamdh.str();
    //cout << "Hexadecimal string: " << hex_string1 <<endl;

	
	// hexadecimal to sring (cipher tpe)	

    uint8_t hex_inputdh1 = SSL::KE_DHE;
    stringstream streamdh1;
    streamdh1 << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputdh1);
    string hex_stringdh1 = streamdh1.str();
   // cout << "Hexadecimal string: " << hex_string2 <<endl;


    SecByteBlock clientRandomBlock1(32);
    rng.GenerateBlock(clientRandomBlock1, 32);

    CryptoPP::Base64Encoder base64Encoder;
    string clientRandomBase64Str1;
    base64Encoder.Attach(new CryptoPP::StringSink(clientRandomBase64Str1));
    base64Encoder.Put(clientRandomBlock1.data(), clientRandomBlock1.size());
    base64Encoder.MessageEnd();

    string clientRandomStr1(reinterpret_cast<const char*>(clientRandomBlock1.data()), clientRandomBlock1.size());

   // cout<<"C: client random: "<<clientRandomStr1<<endl;

    Record send_recorddh1;
    send_recorddh1.hdr.type = REC_HANDSHAKE;
    send_recorddh1.hdr.version = VER_99;
    string bodydh1 = hex_stringdh1 +","+ hex_stringdh1+","+ clientRandomBase64Str1; 
    char* datadh1 = (char*)malloc(bodydh1.length()*sizeof(char));
    memcpy(datadh1, bodydh1.c_str(), bodydh1.length());
    send_recorddh1.data = datadh1;
   // cout<<"C: data sent is : "<<bodydh1<<endl;
    send_recorddh1.hdr.length = bodydh1.length();
	  this->send(send_recorddh1);
  //  int ret_codedh1 = this->send(send_recorddh1);
   // cout<<"C: client sent code: "<<ret_codedh1<<endl;

  //============================SERVER_HELLO_RECV=====================//
    //cout << this->get_port()<<endl;
    Record recv_recorddh;
    this->recv(&recv_recorddh);
   cout<<"\n";
  // cout<<"C: data recieved: "<<recv_recorddh.data<<endl;

//===============================SLICING============================//

    string concatenated_stringdh = recv_recorddh.data;
    vector<string> split_stringsdh;
    stringstream ssdh(concatenated_stringdh);
    string tokendh;
    char delimiterdh = ',';

    while (getline(ssdh, tokendh, delimiterdh)) {
        split_stringsdh.push_back(tokendh);
    }

    // Convert the vector to an array
    string string_arraydh;
    string_arraydh = split_stringsdh[1];
   // cout<<"C: Sliced_String: "<<string_array<<endl;
//===============================SLICING============================//


string serverRandomBase64=split_stringsdh[2];

Base64Decoder decoder;
decoder.Put(reinterpret_cast<const byte*>(serverRandomBase64.data()), serverRandomBase64.size());
decoder.MessageEnd();

SecByteBlock decodedServerRandomBlock1(32);
size_t decodedSize = decoder.MaxRetrievable();
if (decodedSize) {
    decodedServerRandomBlock1.resize(decodedSize);
    decoder.Get(decodedServerRandomBlock1.data(), decodedServerRandomBlock1.size());
}

  string serverRandomStr1(reinterpret_cast<const char*>(decodedServerRandomBlock1.data()), decodedServerRandomBlock1.size());

//cout<<"C: Server Random: "<<serverRandomStr1<<endl;

//============================HEXING=====================================//
   string hexStrdh = string_arraydh;
    stringstream ss1dh;
    ss1dh << hexStrdh;
    int hexValdh;
    ss1dh >> hex>> hexValdh;
   // cout << "C: Hexadecimal value: " << hexStr << endl;
   // cout << "C: Decimal value: " << hexVal << endl;

//============================HEXING=====================================//


    if(hexValdh ==SSL::KE_DHE)
    {
 //     cout<<"C: CLIENT_AFFIRMATIVE"<<endl;
    }
//=============================SETTING_P_G_Q_VALUES===================//

  AutoSeededRandomPool rnd;
  Integer dh_p_1(split_stringsdh[3].c_str());
  Integer dh_g_1(split_stringsdh[4].c_str());
  Integer dh_q_1(split_stringsdh[5].c_str());

   DH clientDH;
    clientDH.AccessGroupParameters().Initialize(dh_p_1, dh_q_1, dh_g_1);


//=============================SETTING_P_G_Q_VALUES===================//


//===================RECEVING_SERVER_HELLO_DONE====================//

    Record recv_recordsh;
    this->recv(&recv_recordsh);
    cout<<"\n";
   // cout<<"C: data recieved: "<<recv_recordsh.data<<endl;

//===================RECEVING_SERVER_HELLO_DONE====================//



    // Generate client's private and public keys
    SecByteBlock privateKey(clientDH.PrivateKeyLength());
    SecByteBlock publicKey(clientDH.PublicKeyLength());
    clientDH.GenerateKeyPair(rng, privateKey, publicKey);


//======================SENDING_PK TO SERVER=================================//


//=========STRINGING===================//
 SecByteBlock public_key = publicKey;// the public key as a SecByteBlock

// Encode the public key as a base64 string
string public_key_str;
StringSink sink(public_key_str);
ArraySource(public_key.data(), public_key.size(), true,
    new Base64Encoder(new Redirector(sink), false /* do not insert line breaks */));
    
//=========STRINGING===================//

//=======================CLIENT_KEY_HS_ATTACH====================//

  
    uint8_t hex_inputc2 = SSL::HS_CLIENT_KEY_EXCHANGE;
    stringstream streamc12;
    streamc12 << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputc2);
    string hex_stringc22 = streamc12.str();

//=======================CLIENT_KEY_HS_ATTACH====================//

    Record send_recorddh;
    send_recorddh.hdr.type = REC_HANDSHAKE;
    send_recorddh.hdr.version = VER_99;
    string bodydh = hex_stringc22+ "," +public_key_str;
    char* datadh = (char*)malloc(bodydh.length()*sizeof(char));
    memcpy(datadh, bodydh.c_str(), bodydh.length());
    send_recorddh.data = datadh;
 //   cout<<"C: data sent is (Pk): "<<bodydh<<endl;
    send_recorddh.hdr.length = bodydh.length();
    this->send(send_recorddh);
 // int ret_codedh = this->send(send_recorddh);
 //   cout<<"C: client sent code: "<<ret_codedh<<endl;
//======================SENDING_PK TO SERVER=================================//


//======================RECIVING_PK From SERVER=================================//
    //cout << this->get_port()<<endl;
    Record recv_recorddhpk;
    this->recv(&recv_recorddhpk);
   // cout<<"\n";
  //  cout<<"C: data recieved(Pk): "<<recv_recorddhpk.data<<endl;

//======================RECIVING_PK From SERVER=================================//



//=======================GENERATING_MASTER_SECRET============================//
//=======================DESTRINGING==============//

//==================================SLICING===============================//

    string concatenated_string = recv_recorddhpk.data;
    vector<string> split_strings;
    stringstream ss(concatenated_string);
    string token;
    char delimiter = ',';

    while (getline(ss, token, delimiter)) {
        split_strings.push_back(token);
    }

    // Convert the vector to an array
    string string_array;
    string_array = split_strings[1];
  //  cout<<"S: Sliced_String: "<<string_array<<endl;
//==================================SLICING===============================//

SecByteBlock ServerPublicKey = Base64StringToSecByteBlock(string_array);

//=======================DESTRINGING==============//

  SecByteBlock preMasterSecret(clientDH.AgreedValueLength());
    if (!clientDH.Agree(preMasterSecret, privateKey, ServerPublicKey)) {
        cerr << "C: Failed to reach the pre-master secret agreement." << endl;
    }
// Encode the public key as a base64 string
string premaster;
StringSink sink1(premaster);
ArraySource(preMasterSecret.data(), preMasterSecret.size(), true,
    new Base64Encoder(new Redirector(sink1), false /* do not insert line breaks */));

size_t keySize = 16; // Desired size of the derived keys in bytes

SecByteBlock clientKey(keySize);
derive_key(preMasterSecret, decodedServerRandomBlock1, clientRandomBlock1, keySize, clientKey);

string clientKeyStr(reinterpret_cast<const char*>(clientKey.data()), clientKey.size());

//cout<<"C: client key: "<<clientKeyStr<<endl;
   this->set_shared_key(clientKey.data(), clientKey.size());
//======================clien finish===================
 uint8_t hex_inputfdh = SSL::HS_FINISHED;
    stringstream streamfdh;
    streamfdh << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputfdh);
    string hex_stringfdh1 = streamfdh.str();
    //cout << "Hexadecimal string: " << hex_string1 <<endl;

 Record send_recordfdh1;
    send_recordfdh1.hdr.type = REC_HANDSHAKE;
    send_recordfdh1.hdr.version = VER_99;
    string bodyfdh1 = hex_stringfdh1;  
    char* datafdh1 = (char*)malloc(bodyfdh1.length()*sizeof(char));
    memcpy(datafdh1, bodyfdh1.c_str(), bodyfdh1.length());
    send_recordfdh1.data = datafdh1;
   // cout<<"C: data sent is : "<<bodyfdh1<<endl;
    send_recordfdh1.hdr.length = bodyfdh1.length();
	  this->send(send_recordfdh1);
  //  int ret_codefdh1 = this->send(send_recordfdh1);
   // cout<<"C: clien finish: "<<ret_codefdh1<<endl;


}

    // IMPLEMENT HANDSHAKE HERE


  return 1;
}

int SslClient::close() {
  int ret_code;
  ret_code = this->tcp_->socket_close();
  return ret_code;
}
