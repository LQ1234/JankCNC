void printbuf(void* buf,int len){
    for(int i=0;i<len;i++){
        Serial.print(reinterpret_cast<unsigned char*>(buf)[i], HEX);
        Serial.print(" ");
    }
}
