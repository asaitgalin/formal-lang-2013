#ifndef _LOLCODE_TYPE_H_
#define _LOLCODE_TYPE_H_

class Type {
private:
   
    enum dataTypes_t {
        dtUntyped = 0, // NOOB
        dtBoolean, // TROOF
        dtInteger, // NUMBR
        dtFloat, // NUMBAR
        dtString // YARN
    };
    
    dataTypes_t type_;
    
    Type(dataTypes_t type):
        type_(type)
    { }

public:
   static Type *getMaxType(Type *lhs, Type *rhs);

   static Type * _untyped; 
   static Type * _boolean;
   static Type * _integer;
   static Type * _float; 
   static Type * _string;
};

#endif /* _LOLCODE_TYPE_H_ */
