function initSerial(addHandler){
    let serialHistory=document.getElementById("serial-history");
    let serialInputfield=document.getElementById("serial-inputfield");
    let serialArrowpad=document.getElementById("serial-arrowpad");

    let serialArrowpadArrows={
        up: document.getElementById("serial-arrowpad-up"),
        down: document.getElementById("serial-arrowpad-down"),
        left: document.getElementById("serial-arrowpad-left"),
        right: document.getElementById("serial-arrowpad-right"),
    };


    addHandler("serial",(text)=>{
        serialHistory.appendChild(document.createTextNode(text));
    })
    let arrowpad={up:false,down:false,left:false,right:false}

    function updateArrowpad(){
        for (var arrow in serialArrowpadArrows) {
            if (serialArrowpadArrows.hasOwnProperty(arrow)) {
                if(arrowpad[arrow]){
                    serialArrowpadArrows[arrow].style.backgroundColor="black";
                    serialArrowpadArrows[arrow].style.color="white";
                }else{
                    serialArrowpadArrows[arrow].style.backgroundColor="white";
                    serialArrowpadArrows[arrow].style.color="black";
                }
            }
        }
    }

    serialArrowpad.addEventListener("keydown", async (e)=>{
        let msg=null;
        if(e.keyCode==38&&!arrowpad.up){
            msg="vertup";
            arrowpad.up=true;
        }
        if(e.keyCode==40&&!arrowpad.down){
            msg="vertdown";
            arrowpad.down=true;
        }
        if(e.keyCode==39&&!arrowpad.right){
            msg="horleft";
            arrowpad.right=true;
        }
        if(e.keyCode==37&&!arrowpad.left){
            msg="horright";
            arrowpad.left=true;
        }
        updateArrowpad();
        if(msg){
            await fetch("test", {
                method: "POST",
                mode: "same-origin",
                headers: {},
                body: "serial: "+msg
            });
        }
    });
    window.addEventListener("keyup", async (e)=>{
        let msg=null;
        if((e.keyCode==38||e.keyCode==40)&&(arrowpad.up||arrowpad.down)){
            msg="vertstop";
            arrowpad.up=false;
            arrowpad.down=false;
        }
        if((e.keyCode==39||e.keyCode==37)&&(arrowpad.left||arrowpad.right)){
            msg="horstop";
            arrowpad.left=false;
            arrowpad.right=false;
        }
        updateArrowpad();
        if(msg){
            await fetch("test", {
                method: "POST",
                mode: "same-origin",
                headers: {},
                body: "serial: "+msg
            });
        }
    });

    serialInputfield.addEventListener("keyup", async (e)=>{
        e.preventDefault();

        if(e.keyCode!=13)return;

        await fetch("test", {
            method: "POST",
            mode: "same-origin",
            headers: {},
            body: "serial: "+serialInputfield.value
        });
        serialInputfield.value="";
    })

}
