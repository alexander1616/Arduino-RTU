#ifndef a_analogButton_h
#define a_analogButton_h 1

#define KEY_SW0 0 //No key pressed
#define KEY_SW1 1
#define KEY_SW2 2
#define KEY_SW3 3
#define KEY_SW4 4
#define KEY_SW5 5
#define KEY_SWX 6 //Unknown value

class analogKey {
public:
  char getKey();
private:
  unsigned int keyBucket[7]; //keys from 1-5 and nonpress
  void saveKey(char key){
    keyBucket[key]++;
  }
  char rank();
  void clear();
};

#endif