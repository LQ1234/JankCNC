void printbuf(void* buf,int len){
    for(int i=0;i<len;i++)
        ets_printf("%x ", reinterpret_cast<unsigned char*>(buf)[i]);
}
