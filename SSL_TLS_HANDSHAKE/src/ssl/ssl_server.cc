#include "ssl_server.h"
#include "channels.h"
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "hkdf.h"
#include "sha.h"
#include "dh.h"
#include "integer.h"
#include "osrng.h"
#include "secblock.h"
#include "crypto_adaptor.h"
#include "tcp.h"
#include "logger.h"
#include "utils.h"
#include "hex.h"
#include "filters.h"
#include "cryptlib.h"
#include "rsa.h"
#include "base64.h"


using namespace std;
using namespace CryptoPP;

 AutoSeededRandomPool rng;


void GenerateDiffieHellmanParameters(AutoSeededRandomPool &rng, Integer &p, Integer &q, Integer &g)
{
    DH dh;
    dh.AccessGroupParameters().GenerateRandomWithKeySize(rng, 1024); // Use your desired key size

    p = dh.GetGroupParameters().GetModulus();
    q = dh.GetGroupParameters().GetSubgroupOrder();
    g = dh.GetGroupParameters().GetGenerator();
}


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


string base64_decode(const string &input) {
    string base64Decoded;
    StringSource ss(input, true,
                    new Base64Decoder(
                        new StringSink(base64Decoded)
                    ));
    return base64Decoded;
}

CryptoPP::SecByteBlock Base64StringToSecByteBlock(const string& base64Str) {
    string decodedStr;
    CryptoPP::StringSink stringSink(decodedStr);
    CryptoPP::StringSource(base64Str, true,
        new CryptoPP::Base64Decoder(new CryptoPP::Redirector(stringSink)));

    CryptoPP::SecByteBlock decoded(reinterpret_cast<const unsigned char*>(decodedStr.data()), decodedStr.size());
    return decoded;
}
SslServer::SslServer() {
  string datetime;
  if ( get_datetime(&datetime, "%Y%m%d-%H%M%S") != 0 ) {
    exit(1);
  }
  this->logger_ = new Logger(("ssl_server_"+datetime+".log"));
  this->tcp_->set_logger(this->logger_);

  get_datetime(&datetime, "%Y/%m/%d %H:%M:%S");
  this->logger_->log("Server Log at " + datetime);

  this->closed_ = false;

  // init dhe
  generate_pqg(this->dh_p_, this->dh_q_, this->dh_g_);

  // init rsa
  generate_rsa_keys(this->private_key_, this->public_key_);
}

SslServer::~SslServer() {
  if ( !this->closed_ ) {
    this->shutdown();
  }
  delete this->logger_;
}


int SslServer::start(int num_clients) {
  if ( this->closed_ ) {
    return -1;
  }

  return this->tcp_->socket_listen(num_clients);
}

SSL* SslServer::accept() {
  if ( this->closed_ ) {
    return NULL;
  }

  TCP* cxn = this->tcp_->socket_accept();
  if ( cxn == NULL ) {
    cerr << "error when accepting" << endl;
    return NULL;
  }

  cxn->set_logger(this->logger_);

  SSL* new_ssl_cxn = new SSL(cxn);
  this->clients_.push_back(new_ssl_cxn);


  // IMPLEMENT HANDSHAKE HERE


  //============================CLIENT_HELLO_RECV============================//
     //cout<<"reciving hello request from client";
     Record recv_record1;
     new_ssl_cxn->recv(&recv_record1);
   // cout<<"\n";
   // cout<<"S: data recieved: "<<recv_record1.data<<endl;

//==================================SLICING===============================//

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
  //  cout<<"S: Sliced_String: "<<string_array<<endl;
//==================================SLICING===============================//

//============================HEXING=====================================//
   string hexStr = string_array;
    stringstream ss1;
    ss1 << hexStr;
    int hexVal ;
    ss1 >> hex >> hexVal;
   // cout << "C: Hexadecimal value: " << hexStr << endl;
  //  cout << "C: Decimal value: " << hexVal << endl;
//============================HEXING=====================================//
// Receive server random string through TCP

string clientRandomBase64Str=split_strings[2]; // This is the base64-encoded string received through TCP

CryptoPP::Base64Decoder base64Decoder;
SecByteBlock clientRandomDecoded(32);
base64Decoder.Attach(new CryptoPP::ArraySink(clientRandomDecoded, clientRandomDecoded.size()));
base64Decoder.Put(reinterpret_cast<const byte*>(clientRandomBase64Str.data()), clientRandomBase64Str.size());
base64Decoder.MessageEnd();

string clientRandomDecodedStr(reinterpret_cast<const char*>(clientRandomDecoded.data()), clientRandomDecoded.size());
//cout << "S: Client Random: " << clientRandomDecodedStr << endl;

// Convert server random string to byte array



    if(hexVal ==SSL::KE_RSA)
    {
   //   cout<<"S:_SERVER AFFIRMATIVE"<<endl;


//===========================SENDING_SERVER_HELLO===============================//
	// hexadecimal to string (hello handshake)	

    uint8_t hex_input = SSL::HS_SERVER_HELLO ;
    stringstream stream;
    stream << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_input);
    string hex_string1 = stream.str();
    //cout << "Hexadecimal value as string: " << hex_string1 << endl;
	
	// hexadecimal to sring (cipher tpe)	
	  uint8_t hex_input1 = SSL::KE_RSA;
    stringstream stream1;
    stream1 << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_input1);
    string hex_string2 = stream1.str();
    //cout << "Hexadecimal value as string: " << hex_string2 << endl;

    //generating server random
 SecByteBlock serverRandomBlock(32);
rng.GenerateBlock(serverRandomBlock.data(), serverRandomBlock.size());
  string serverRandomStr(reinterpret_cast<const char*>(serverRandomBlock.data()), serverRandomBlock.size());

  Base64Encoder encoder;
encoder.Put(serverRandomBlock.data(), serverRandomBlock.size());
encoder.MessageEnd();

string serverRandomBase64;
size_t encodedSize = encoder.MaxRetrievable();
if (encodedSize) {
    serverRandomBase64.resize(encodedSize);
    encoder.Get(reinterpret_cast<byte*>(&serverRandomBase64[0]), serverRandomBase64.size());
}

/*
    cout<<"\n";
    cout<<"S: Server Random: "<<serverRandomStr<<endl;
    cout<<"\n";
*/
    Record send_record1;
    send_record1.hdr.type = REC_HANDSHAKE;
    send_record1.hdr.version = VER_99;
    string body1 = hex_string1+"," + hex_string2+"," + serverRandomBase64; 
    //cout<<"S: data sent is : "<<body1<<endl;
    char* data1 = (char*)malloc(body1.length()*sizeof(char));
    memcpy(data1, body1.c_str(), body1.length());
    send_record1.data = data1;
    send_record1.hdr.length = body1.length();
	  new_ssl_cxn->send(send_record1);
   // int ret_code = new_ssl_cxn->send(send_record1);
  //  cout<<"S: server sent code: "<<ret_code<<endl;



   string derPublicKey;
    StringSink sink(derPublicKey);
    this->public_key_.DEREncodePublicKey(sink);

    // Print the DER-encoded public key in base64 format
    string base64PublicKey;
    StringSource(derPublicKey, true,
                 new Base64Encoder(
                     new StringSink(base64PublicKey),
                     false ));



    // Decode the DER-encoded public key
    RSA::PublicKey decodedPublicKey;
    StringSource source(derPublicKey, true /* pump all */);
    decodedPublicKey.BERDecodePublicKey(source, true /* parametersPresent */, derPublicKey.size());

     // DER-encode the decoded public key and encode it in base64 format
    string derDecodedPublicKey;
    StringSink derSink(derDecodedPublicKey);
    decodedPublicKey.DEREncodePublicKey(derSink);

    string base64DecodedPublicKey;
    StringSource(derDecodedPublicKey, true,
                 new Base64Encoder(
                     new StringSink(base64DecodedPublicKey),
                     false ));


  Record send_recordrpk;
  send_recordrpk.hdr.type = REC_HANDSHAKE;
  send_recordrpk.hdr.version = VER_99;
  string bodyrpk = base64DecodedPublicKey;
  char* datarpk = (char*)malloc(bodyrpk.length()*sizeof(char));
  memcpy(datarpk, bodyrpk.c_str(), bodyrpk.length());
  send_recordrpk.data = datarpk;
  send_recordrpk.hdr.length = bodyrpk.length();
  new_ssl_cxn->send(send_recordrpk);
  //cout<<"S: Server_Public_Key: "<<base64DecodedPublicKey<<endl;
  //int ret_code1 = new_ssl_cxn->send(send_recordrpk);
 //cout<<"\n";

//===========================SENDING_SERVER_HELLO===============================//

//===================SENDING_SERVER_HELLO_DONE========================//


	// hexadecimal to sring (cipher tpe)	
	  uint8_t hex_inputsh = SSL::HS_SERVER_HELLO_DONE;
    stringstream streamsh;
    streamsh << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputsh);
    string hex_stringsh = streamsh.str();
    //cout << "Hexadecimal value as string: " << hex_string2 << endl;

    Record send_recordsh;
    send_recordsh.hdr.type = REC_HANDSHAKE;
    send_recordsh.hdr.version = VER_99;
    string bodysh = hex_stringsh;
    //cout<<"S: data sent is : "<<body1<<endl;
    char* datash = (char*)malloc(bodysh.length()*sizeof(char));
    memcpy(datash, bodysh.c_str(), bodysh.length());
    send_recordsh.data = datash;
    send_recordsh.hdr.length = bodysh.length();
	  new_ssl_cxn->send(send_recordsh);
    //	  int ret_codesh = new_ssl_cxn->send(send_recordsh);
    //cout<<"S: server sent code: "<<ret_codesh<<endl;


//===================SENDING_SERVER_HELLO_DONE========================//

//=====================RECV_PREMASTER_KEY==================//
  Record recv_record1;
    new_ssl_cxn->recv(&recv_record1);
    //cout<<"S: data recieved: "<<recv_record1.data<<endl;
    //cout<<"\n";

//===============================SLICING to get premaster key============================//

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
    //cout<<"C: Sliced_String: "<<string_array<<endl;
//===============================SLICING to get premaster key============================//



string receivedBase64EncodedCiphertext = string_array;
string receivedCiphertext = base64_decode(receivedBase64EncodedCiphertext);
//cout<<"S: receivedCiphertext: "<<receivedCiphertext<<endl;
string plain_text;

// Decrypt the cipher text using the private key
rsa_decrypt(this->private_key_, &plain_text, receivedCiphertext);


//cout<<"\n";
//cout<<"S: Premaster: "<<plain_text<<endl;
// Convert the recovered premaster secret string to a SecByteBlock
SecByteBlock recoveredPremaster(reinterpret_cast<const byte*>(plain_text.data()), plain_text.size());



SecByteBlock derivedKey; 
derivedKey.resize(16);


size_t keySize = 16;

SecByteBlock serverKey1(keySize);

derive_key(recoveredPremaster, serverRandomBlock, clientRandomDecoded, keySize, serverKey1);

string serverKeyStr1(reinterpret_cast<const char*>(serverKey1 .data()), serverKey1 .size());

//cout<<"S: serverkey: "<<serverKeyStr1<<endl;

    new_ssl_cxn->set_shared_key(serverKey1.data(), serverKey1.size());
//====================server recive hanshake finish:=====================//

 Record recv_recordf1;
    new_ssl_cxn->recv(&recv_recordf1);
    string hexStrf = recv_recordf1.data;
    stringstream ssf1;
    ssf1 << hexStrf;
    int hexValf ;
    ssf1 >> hex >> hexValf;
/*
 if(hexValf ==SSL::HS_FINISHED)
    {
    cout<<"handshake completed"<<endl;
 }
 */
    }

    if(hexVal ==SSL::KE_DHE)
     {
    //   cout<<"S:_SERVER AFFIRMATIVE"<<endl;

//==========================GENERATING_P_G_Q=======================//

AutoSeededRandomPool rng;
Integer dh_p, dh_q, dh_g;

GenerateDiffieHellmanParameters(rng, dh_p, dh_q, dh_g);



DH serverDH;
serverDH.AccessGroupParameters().Initialize(dh_p, dh_q, dh_g);



stringstream sspqg;
sspqg << hex << dh_p;
string p_str = sspqg.str();
sspqg.str("");
sspqg << hex << dh_q;
string q_str = sspqg.str();
sspqg.str("");
sspqg << hex << dh_g;
string g_str = sspqg.str();

/*
cout<<"S: P"<<p_str<<endl;
cout<<"S: q"<<q_str<<endl;
cout<<"S: g"<<g_str<<endl;
*/
//==========================GENERATING_P_G_Q=======================//

//****************server hello send********************************	
	// hexadecimal to sring (hello handshake)	

    uint8_t hex_inputdh = SSL::HS_SERVER_HELLO ;
    stringstream streamdh;
    streamdh << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputdh);
    string hex_stringdh = streamdh.str();
    //cout << "Hexadecimal value as string: " << hex_string1 << endl;
	
	// hexadecimal to sring (cipher tpe)	
	  uint8_t hex_inputdh1 = SSL::KE_DHE;
    stringstream streamdh1;
    streamdh1 << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputdh1);
    string hex_stringdh1 = streamdh1.str();
    //cout << "Hexadecimal value as string: " << hex_string2 << endl;

        //generating server random
 SecByteBlock serverRandomBlock1(32);
rng.GenerateBlock(serverRandomBlock1.data(), serverRandomBlock1.size());
  string serverRandomStr1(reinterpret_cast<const char*>(serverRandomBlock1.data()), serverRandomBlock1.size());

  Base64Encoder encoder;
encoder.Put(serverRandomBlock1.data(), serverRandomBlock1.size());
encoder.MessageEnd();

string serverRandomBase64;
size_t encodedSize = encoder.MaxRetrievable();
if (encodedSize) {
    serverRandomBase64.resize(encodedSize);
    encoder.Get(reinterpret_cast<byte*>(&serverRandomBase64[0]), serverRandomBase64.size());
}
    Record send_recorddh;
    send_recorddh.hdr.type = REC_HANDSHAKE;
    send_recorddh.hdr.version = VER_99;
    string bodydh = hex_stringdh+"," + hex_stringdh1+"," + serverRandomBase64+","+p_str+","+g_str+","+q_str; 
    //cout<<"S: data sent is : "<<bodydh<<endl;
    char* datadh = (char*)malloc(bodydh.length()*sizeof(char));
    memcpy(datadh, bodydh.c_str(), bodydh.length());
    send_recorddh.data = datadh;
    send_recorddh.hdr.length = bodydh.length();
	  new_ssl_cxn->send(send_recorddh);
  //int ret_codedh = new_ssl_cxn->send(send_recorddh);
   // cout<<"S: server sent code: "<<ret_codedh<<endl;
     

//===================SENDING_SERVER_HELLO_DONE========================//


	// hexadecimal to sring (cipher tpe)	
	  uint8_t hex_inputsh = SSL::HS_SERVER_HELLO_DONE;
    stringstream streamsh;
    streamsh << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputsh);
    string hex_stringsh = streamsh.str();
    //cout << "Hexadecimal value as string: " << hex_string2 << endl;

    Record send_recordsh;
    send_recordsh.hdr.type = REC_HANDSHAKE;
    send_recordsh.hdr.version = VER_99;
    string bodysh = hex_stringsh;
    //cout<<"S: data sent is : "<<body1<<endl;
    char* datash = (char*)malloc(bodysh.length()*sizeof(char));
    memcpy(datash, bodysh.c_str(), bodysh.length());
    send_recordsh.data = datash;
    send_recordsh.hdr.length = bodysh.length();
	  new_ssl_cxn->send(send_recordsh);
    // int ret_codesh = new_ssl_cxn->send(send_recordsh);
    //cout<<"S: server sent code: "<<ret_codesh<<endl;


//===================SENDING_SERVER_HELLO_DONE========================//
//====================KEY_EXCHANGE_BEGINS============================//


    // Generate server's private and public keys
    SecByteBlock privateKey(serverDH.PrivateKeyLength());
    SecByteBlock publicKey(serverDH.PublicKeyLength());
    serverDH.GenerateKeyPair(rng,privateKey, publicKey);


    //=========RECIVE_PK_FROM_CLIENT===============================//
     Record recv_recordpk;
     new_ssl_cxn->recv(&recv_recordpk);
  //   cout<<"\n";
 //    cout<<"S: Data recieved (Pk): "<<recv_recordpk.data<<endl;
    //=========RECIVE_PK_FROM_CLIENT===============================//

//======================SENDING_PK TO CLIENT=================================//

 //=========STRINGING===================//
 SecByteBlock public_key = publicKey;// the public key as a SecByteBlock

// Encode the public key as a base64 string
string public_key_str;
StringSink sink(public_key_str);
ArraySource(public_key.data(), public_key.size(), true,
    new Base64Encoder(new Redirector(sink), false /* do not insert line breaks */));

//=======================SERVER_KEY_HS_ATTACH====================//

  
    uint8_t hex_inputc2 = SSL::HS_SERVER_KEY_EXCHANGE;
    stringstream streamc12;
    streamc12 << "0x" <<hex <<setw(2) <<setfill('0') << static_cast<int>(hex_inputc2);
    string hex_stringc22 = streamc12.str();

//=======================SERVER_KEY_HS_ATTACH====================//
    
//=========STRINGING===================//
   Record send_recorddhpk;
    send_recorddhpk.hdr.type = REC_HANDSHAKE;
    send_recorddhpk.hdr.version = VER_99;
    string bodydhpk = hex_stringc22+ "," +public_key_str;
    char* datadhpk = (char*)malloc(bodydhpk.length()*sizeof(char));
    memcpy(datadhpk, bodydhpk.c_str(), bodydhpk.length());
    send_recorddhpk.data = datadhpk;
  //  cout<<"S: data sent is (Pk) : "<<bodydhpk<<endl;
    send_recorddhpk.hdr.length = bodydhpk.length();
	  new_ssl_cxn->send(send_recorddhpk);
  // int ret_codedhpk = new_ssl_cxn->send(send_recorddhpk);
  //  cout<<"S: client sent code: "<<ret_codedhpk<<endl;

//======================SENDING_PK TO CLIENT=================================//


//=======================GENERATING_MASTER_SECRET============================//


//=======================DESTRINGING==============//
//==================================SLICING===============================//

    string concatenated_string = recv_recordpk.data;
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

SecByteBlock clientPublicKey = Base64StringToSecByteBlock(string_array);

//=======================DESTRINGING==============//

SecByteBlock preMasterSecret(serverDH.AgreedValueLength());
if (!serverDH.Agree(preMasterSecret, privateKey, clientPublicKey)) {
    cerr << "S: Failed to reach the pre-master secret agreement." << endl;
}


// Encode the public key as a base64 string
string premaster;
StringSink sink1(premaster);
ArraySource(preMasterSecret.data(), preMasterSecret.size(), true,
    new Base64Encoder(new Redirector(sink1), false /* do not insert line breaks */));
//cout << "S: Pre-master secret value: "<<premaster<<endl;
//=======================GENERATING_MASTER_SECRET============================//


size_t keySize = 16; // Desired size of the derived keys in bytes

SecByteBlock serverKey(keySize);

derive_key(preMasterSecret, serverRandomBlock1, clientRandomDecoded, keySize, serverKey);

string serverKeyStr(reinterpret_cast<const char*>(serverKey .data()), serverKey .size());

//cout<<"S: serverkey: "<<serverKeyStr<<endl;

    new_ssl_cxn->set_shared_key(serverKey.data(), serverKey.size());
//====================server recive hanshake finish:=====================//

 Record recv_recordf2;
    new_ssl_cxn->recv(&recv_recordf2);
    string hexStrf = recv_recordf2.data;
    stringstream ssf1;
    ssf1 << hexStrf;
    int hexValf ;
    ssf1 >> hex >> hexValf;

 if(hexValf ==SSL::HS_FINISHED)
    {
   // cout<<"handshake completed"<<endl;
 }

}

  return new_ssl_cxn;
}

int SslServer::shutdown() {
  if ( this->closed_ ) {
    return -1;
  }

  // pop all clients
  while ( !this->clients_.empty() ) {
    SSL* cxn = this->clients_.back();
    this->clients_.pop_back();
    if ( cxn != NULL ) {
      delete cxn;
    }
  }
  return 0;
}

vector<SSL*> SslServer::get_clients() const {
  return vector<SSL*>(this->clients_);
}

int SslServer::broadcast(const string &msg) {
  if ( this->closed_ ) {
    return -1;
  }

  int num_sent = 0;

  // this->logger_->log("broadcast:");
  // this->logger_->log_raw(msg);

  for ( vector<SSL*>::iterator it = this->clients_.begin() ;
        it != this->clients_.end() ; ++it ) {
    ssize_t send_len;
    send_len = (*it)->send(msg);
    if ( send_len == (unsigned int)msg.length() ) {
      num_sent += 1;
    }
  }

  return num_sent;
}
