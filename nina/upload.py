import os
from pathlib import Path
import mimetypes
import subprocess

root=Path(__file__).parent
datastr="""
const unsigned char file_404[]=R"!!!(
    404 not found :(
)!!!";
"""
mapstr="""
struct ResponseData {
    char* mime;
    const unsigned char* data;
    int length;
    int code;
    ResponseData(char* _mime,const unsigned char* _data,int _length,int _code){
        mime=_mime;
        data=_data;
        length=_length;
        code=_code;
    }
};
ResponseData get_response(const char* inp){
    if(strcmp(inp,"/")==0)return(get_response("/index.html"));
"""

tempi=0
servePath=Path(root,"..","serve")
for path in servePath.rglob('*.*'):
    bytes=path.read_bytes()
    length=len(bytes)
    mime=mimetypes.guess_type(path.name)[0] or "application/octet-stream"
    pathStr="/"+"/".join(path.relative_to(servePath).parts)

    datastr+=f'const unsigned char file_{tempi}[] = {{ {", ".join([hex(byte) for byte in bytes])} }};\n'
    mapstr+=f'''
    if(strcmp(inp,"{pathStr}")==0)return(ResponseData("{mime}",file_{tempi},{length},200));
    '''
    tempi+=1
mapstr+="""
    return(ResponseData("text/html",file_404,strlen(reinterpret_cast<const char *>(file_404)),404));
}
"""
resstr=datastr+mapstr
Path(root,"main","web","serve.h").write_text(resstr)

subprocess.run(f"cd {root} && make -j4 flash",shell=True)
