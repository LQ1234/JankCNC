namespace NinaSPI{

    Pair createFloatPair(const char* key,float value){
        Pair pair;
        strcpy(pair.key,key);
        dtostrf(value,(10+2),10,pair.value);
        return(pair);
    }
    void sendUpdatePair(const Pair pair){
        SPIMsgCommand cmd;

        cmd.type=SPIMsgType::SETPAIR;
        cmd.data.setpair=pair;

        SPIMsgResult res;
        if(!NinaSPI::vertsend(reinterpret_cast<char*>(&cmd),sizeof(cmd),reinterpret_cast<char*>(&res),sizeof(res)))return;

        //Serial.println("SAMD: Updating");
    }

    void updateFloatPair(const Pair pair, const char* key,float &value){
        if(strcmp(pair.key,key)==0){
            value=strtof(pair.value,nullptr);
            sendUpdatePair(createFloatPair(key, value));
        }
    }
    void (*valueUpdateHandler)(Pair p);
    void updateValues(){
        SPIMsgCommand cmd;
        cmd.type=SPIMsgType::GETPAIR;

        SPIMsgResult res;
        if(!NinaSPI::vertsend(reinterpret_cast<char*>(&cmd),sizeof(cmd),reinterpret_cast<char*>(&res),sizeof(res)))return;

        Pair pair=res.data.getpair;

        if(!pair.key[0])return;

        /*
        Serial.print("SAMD: received ");
        Serial.print(pair.key);
        Serial.print(" -> ");
        Serial.println(pair.value);
        */
        valueUpdateHandler(pair);
    }
}
